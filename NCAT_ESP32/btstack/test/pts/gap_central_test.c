/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#define BTSTACK_FILE__ "gap_central_test.c"

/*
 * gatt.c : Tool for testig BLE central
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "btstack_config.h"

#include "ad_parser.h"
#include "ble/att_db.h"
#include "ble/att_server.h"
#include "ble/le_device_db.h"
#include "ble/sm.h"
#include "btstack_event.h"
#include "gap.h"
#include "hci.h"
#include "hci_dump.h"
#include "l2cap.h"
#include "btstack_stdin.h"
#include "ble/att_db_util.h"
#include "bluetooth_gatt.h"

// test profile
#include "gap_central_test.h"

// Non standard IXIT
#define PTS_USES_RECONNECTION_ADDRESS_FOR_ITSELF
#define PTS_UUID128_REPRESENTATION

typedef enum {
    CENTRAL_IDLE,
    CENTRAL_W4_NAME_QUERY_COMPLETE,
    CENTRAL_W4_NAME_VALUE,
    CENTRAL_W4_RECONNECTION_ADDRESS_QUERY_COMPLETE,
    CENTRAL_W4_PERIPHERAL_PRIVACY_FLAG_QUERY_COMPLETE,
    CENTRAL_W4_SIGNED_WRITE_QUERY_COMPLETE,
    CENTRAL_W4_PRIMARY_SERVICES,
    CENTRAL_ENTER_SERVICE_UUID_4_DISCOVER_CHARACTERISTICS,
    CENTRAL_ENTER_START_HANDLE_4_DISCOVER_CHARACTERISTICS,
    CENTRAL_ENTER_END_HANDLE_4_DISCOVER_CHARACTERISTICS,
    CENTRAL_W4_CHARACTERISTICS,
    CENTRAL_W4_DISCOVER_CHARACTERISTIC_DESCRIPTORS,
    CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_HANDLE,
    CENTRAL_ENTER_HANDLE_4_READ_CHARACTERISTIC_VALUE_BY_UUID,
    CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_UUID,
    CENTRAL_ENTER_OFFSET_4_READ_LONG_CHARACTERISTIC_VALUE_BY_HANDLE,
    CENTRAL_W4_READ_LONG_CHARACTERISTIC_VALUE_BY_HANDLE,
    CENTRAL_W4_READ_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE,
    CENTRAL_ENTER_OFFSET_4_READ_LONG_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE,
    CENTRAL_W4_READ_LONG_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE,
    CENTRAL_W4_READ_MULTIPLE_CHARACTERISTIC_VALUES,
    CENTRAL_W4_WRITE_WITHOUT_RESPONSE,
    CENTRAL_W4_WRITE_CHARACTERICISTIC_VALUE,
    CENTRAL_ENTER_HANDLE_4_WRITE_LONG_CHARACTERISTIC_VALUE,
    CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_VALUE,
    CENTRAL_W4_RELIABLE_WRITE,
    CENTRAL_W4_WRITE_CHARACTERISTIC_DESCRIPTOR,
    CENTRAL_ENTER_HANDLE_4_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR,
    CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR,
    CENTRAL_W4_SIGNED_WRITE,

    CENTRAL_W4_ENTER_HANDLE_4_PREPARE_WRITE,
    CENTRAL_W4_ENTER_OFFSET_4_PREPARE_WRITE,

    CENTRAL_GPA_ENTER_UUID,
    CENTRAL_GPA_ENTER_START_HANDLE,
    CENTRAL_GPA_ENTER_END_HANDLE,
    CENTRAL_GPA_W4_RESPONSE,
    CENTRAL_GPA_W4_RESPONSE2,
    CENTRAL_GPA_W4_RESPONSE3,
    CENTRAL_GPA_W4_RESPONSE4,
} central_state_t;

typedef struct advertising_report {
    uint8_t   type;
    uint8_t   event_type;
    uint8_t   address_type;
    bd_addr_t address;
    uint8_t   rssi;
    uint8_t   length;
    uint8_t * data;
} advertising_report_t;

static const uint8_t gpa_format_type_len[] = {
    /* 0x00 */
    1,1,1,1,1,
    /* 0x05 */
    2,2,
    /* 0x07 */
    3,4,6,8,16,
    /* 0x0c */
    1,2,2,3,4,6,8,16,
    /* 0x14 */
    4,8,2,4,4
};


// IXIT
static uint8_t TSPX_advertising_data[] = {  0x02, 0x01, 0x00, /* */ 8, 0x09, 'B', 'T', 's', 't', 'a', 'c', 'k'};

static uint8_t test_irk[] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static int gap_privacy = 0;
static int gap_bondable = 1;
static char gap_device_name[20];
static int gap_connectable = 0;
static gap_security_level_t security_level;

static char * sm_io_capabilities = NULL;
static int sm_mitm_protection = 0;
static int sm_sc;
static int sm_sc_only_mode;

static int sm_have_oob_data = 0;
static uint8_t * sm_oob_data_A = (uint8_t *) "0123456789012345"; // = { 0x30...0x39, 0x30..0x35}
static uint8_t * sm_oob_data_B = (uint8_t *) "3333333333333333"; // = { 0x30...0x39, 0x30..0x35}
static int sm_min_key_size = 7;
static uint8_t pts_privacy_flag;

static int ui_passkey = 0;
static int ui_digits_for_passkey = 0;
static int ui_uint16 = 0;
static int ui_uint16_request = 0;
static int ui_uint16_pos = 0;
static int ui_uuid16 = 0;
static int ui_uuid128_request = 0;
static int ui_uuid128_pos     = 0;
static uint8_t ui_uuid128[16];
static int      ui_handles_count;
static int      ui_handles_index;
static uint16_t ui_handles[10];
static uint16_t ui_attribute_handle;
static int      ui_attribute_offset;
static int      ui_value_request = 0;
static uint8_t  ui_value_data[50];
static int      ui_value_pos = 0;
static uint16_t ui_start_handle;
static uint16_t ui_end_handle;
static uint8_t ui_presentation_format[7];
static uint16_t ui_aggregate_handle;
static uint16_t handle = 0;

//static bd_addr_t public_pts_address = {0x00, 0x1B, 0xDC, 0x07, 0x32, 0xef};
//static bd_addr_t public_pts_address = {0x00, 0x1B, 0xDC, 0x08, 0xe2, 0x72};
static bd_addr_t public_pts_address = {0x00, 0x1B, 0xDC, 0x08, 0xe2, 0x5C};

static int       public_pts_address_type = 0;
static bd_addr_t current_pts_address;
static int       current_pts_address_type;
static int       reconnection_address_set = 0;
static bd_addr_t our_private_address;
static bool      connect_to_resolvable_address;

static uint16_t pts_signed_write_characteristic_uuid = 0xb00d;
static uint16_t pts_signed_write_characteristic_handle = 0x00b1;
static uint8_t signed_write_value[] = { 0x12 };
static int le_device_db_index;
static sm_key_t signing_csrk;

static central_state_t central_state = CENTRAL_IDLE;
static gatt_client_characteristic_t gap_name_characteristic;
static gatt_client_characteristic_t gap_reconnection_address_characteristic;
static gatt_client_characteristic_t gap_peripheral_privacy_flag_characteristic;
static gatt_client_characteristic_t signed_write_characteristic;
static gatt_client_service_t        service;

static btstack_packet_callback_registration_t hci_event_callback_registration;
static btstack_packet_callback_registration_t sm_event_callback_registration;
static gatt_client_notification_t gatt_client_notification;

static void show_usage(void);
///

static void printUUID(uint8_t * uuid128, uint16_t uuid16){
    if (uuid16){
        printf("%04x",uuid16);
    } else {
        printf("%s", uuid128_to_str(uuid128));
    }
}

static const char * att_errors[] = {
    "OK",
    "Invalid Handle",
    "Read Not Permitted",
    "Write Not Permitted",
    "Invalid PDU",
    "Insufficient Authentication",
    "Request No Supported",
    "Invalid Offset",
    "Insufficient Authorization",
    "Prepare Queue Full",
    "Attribute Not Found",
    "Attribute Not Long",
    "Insufficient Encryption Size",
    "Invalid Attribute Value Length",
    "Unlikely Error",
    "Insufficient Encryption",
    "Unsupported Group Type",
    "Insufficient Resource"
};
static const char * att_error_reserved = "Reserved";
static const char * att_error_application = "Application Error";
static const char * att_error_common_error = "Common Profile and Service Error Codes";
static const char * att_error_timeout = "Timeout";

static const char * att_error_string_for_code(uint8_t code){
    if (code >= 0xe0) return att_error_common_error;
    if (code >= 0xa0) return att_error_reserved;
    if (code >= 0x80) return att_error_application;
    if (code == 0x7f) return att_error_timeout;
    if (code >= 0x12) return att_error_reserved;
    return att_errors[code];
}

const char * ad_event_types[] = {
    "Connectable undirected advertising",
    "Connectable directed advertising",
    "Scannable undirected advertising",
    "Non connectable undirected advertising",
    "Scan Response"
};

static void handle_advertising_event(uint8_t * packet, int size){
    UNUSED(size);
    // filter PTS
    bd_addr_t addr;
    gap_event_advertising_report_get_address(packet, addr);
    uint8_t addr_type = gap_event_advertising_report_get_address_type(packet);
    // always request address resolution
    sm_address_resolution_lookup(addr_type, addr);

    uint8_t adv_event_type = gap_event_advertising_report_get_advertising_event_type(packet);
    int adv_size = gap_event_advertising_report_get_data_length(packet);
    const uint8_t * adv_data = gap_event_advertising_report_get_data(packet);

    // identify advertisements from PTS by address of test data
    if (memcmp(addr, current_pts_address, 6) == 0){
        // public address PTS
    } else if ( (adv_size >= sizeof(TSPX_advertising_data)) && (memcmp(TSPX_advertising_data, adv_data, sizeof(TSPX_advertising_data)) == 0)){
        // private address pts
    } else {
        return;
    }

    printf("Advertisement: %s - %s, ", bd_addr_to_str(addr), ad_event_types[adv_event_type]);

    // check flags
    ad_context_t context;
    for (ad_iterator_init(&context, adv_size, (uint8_t *)adv_data) ; ad_iterator_has_more(&context) ; ad_iterator_next(&context)){
        uint8_t data_type = ad_iterator_get_data_type(&context);
        // uint8_t size      = ad_iterator_get_data_len(&context);
        const uint8_t * data = ad_iterator_get_data(&context);
        switch (data_type){
            case 1: // AD_FLAGS
                if (*data & 1) printf("LE Limited Discoverable Mode, ");
                if (*data & 2) printf("LE General Discoverable Mode, ");
                break;
            default:
                break;
        }
    }

    // dump data
    printf("Data: ");
    printf_hexdump(adv_data, adv_size);
}

static uint8_t gap_adv_type(void){
    // if (gap_scannable) return 0x02;
    // if (gap_directed_connectable) return 0x01;
    if (!gap_connectable) return 0x03;
    return 0x00;
}

static uint8_t adv_data_type[3];
static void update_advertisement_params(void){
    uint8_t adv_type = gap_adv_type();
    printf("GAP: Connectable = %u -> advertising_type %u (%s)\n", gap_connectable, adv_type, ad_event_types[adv_type]);
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    uint16_t adv_int_min = 0x800;
    uint16_t adv_int_max = 0x800;
    switch (adv_type){
        case 0:
        case 2:
        case 3:
            gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
            break;
        case 1:
        case 4:
            gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, public_pts_address_type, public_pts_address, 0x07, 0x00);
            break;
    }
    adv_data_type[0] = 2;
    adv_data_type[1] = 1;
    adv_data_type[2] = 2; // LE General Discoverable Mode = 1, BR/EDR Not Supported = 0
    if (gap_connectable){
        gap_advertisements_set_data(sizeof(adv_data_type), adv_data_type);
    } else {
        gap_advertisements_set_data(0, NULL);
    }
}

static void app_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    uint16_t aHandle;
    bd_addr_t event_address;

    switch (packet_type) {
            
        case HCI_EVENT_PACKET:
            switch (packet[0]) {
                
                case BTSTACK_EVENT_STATE:
                    if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
                        // ready
                        printf("Central test ready\n");
                    }
                    break;

                case HCI_EVENT_PIN_CODE_REQUEST:
                    // inform about pin code request
                    printf("Pin code request - using '0000'\n");
                    hci_event_pin_code_request_get_bd_addr(packet, event_address);
                    gap_pin_code_response(event_address, "0000");
                    break;

                case HCI_EVENT_USER_CONFIRMATION_REQUEST:
                    // ssp: inform about user confirmation request
                    printf("SSP User Confirmation Request with numeric value '%06"PRIu32"'\n",
                           hci_event_user_confirmation_request_get_numeric_value(packet));
                    break;

                case HCI_EVENT_LE_META:
                    switch (hci_event_le_meta_get_subevent_code(packet)) {
                        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                            handle = little_endian_read_16(packet, 4);
                            printf("Connection complete, handle 0x%04x\n", handle);
                            break;

                        default:
                            break;
                    }
                    break;

                case HCI_EVENT_CONNECTION_COMPLETE:
                    if (hci_event_connection_complete_get_status(packet) != ERROR_CODE_SUCCESS) break;
                    handle = hci_event_connection_complete_get_connection_handle(packet);
                    printf("BR/EDR Connection with handle 0x%04x\n", handle);
                    break;

                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    aHandle = little_endian_read_16(packet, 3);
                    printf("Disconnected from handle 0x%04x\n", aHandle);
                    break;
                    
                case SM_EVENT_PASSKEY_INPUT_NUMBER: 
                    // store peer address for input
                    printf("\nGAP Bonding: Enter 6 digit passkey: '");
                    fflush(stdout);
                    ui_passkey = 0;
                    ui_digits_for_passkey = 6;
                    break;

                case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
                    printf("\nGAP Bonding: Display Passkey '%06u\n", sm_event_passkey_display_number_get_passkey(packet));
                    break;

                case SM_EVENT_PASSKEY_DISPLAY_CANCEL: 
                    printf("\nGAP Bonding: Display cancel\n");
                    break;

                case SM_EVENT_JUST_WORKS_REQUEST:
                    // auto-authorize connection if requested
                    sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
                    printf("Just Works request confirmed\n");
                    break;

                case SM_EVENT_NUMERIC_COMPARISON_REQUEST:
                    printf("SM_EVENT_NUMERIC_COMPARISON_REQUEST: %u\n", sm_event_numeric_comparison_request_get_passkey(packet));
                    sm_numeric_comparison_confirm(sm_event_numeric_comparison_request_get_handle(packet));
                    break;

                case SM_EVENT_AUTHORIZATION_REQUEST:
                    // auto-authorize connection if requested
                    sm_authorization_grant(sm_event_authorization_request_get_handle(packet));
                    break;

                case GAP_EVENT_ADVERTISING_REPORT:
                    if (connect_to_resolvable_address){
                        // resolve address
                        gap_event_advertising_report_get_address(packet, event_address);
                        sm_address_resolution_lookup(gap_event_advertising_report_get_address_type(packet), event_address);
                    } else {
                        handle_advertising_event(packet, size);
                    }
                    break;
                case SM_EVENT_IDENTITY_CREATED:
                    le_device_db_index =  sm_event_identity_created_get_index(packet);
                    printf("SM_EVENT_IDENTITY_CREATED index %u\n", le_device_db_index);
                    break;
                case SM_EVENT_IDENTITY_RESOLVING_SUCCEEDED:
                    reverse_bd_addr(&packet[5], event_address);
                    // skip already detected pts
                    if (memcmp(event_address, current_pts_address, 6) == 0) break;
                    memcpy(current_pts_address, event_address, 6);
                    current_pts_address_type =  packet[4];
                    le_device_db_index       =  little_endian_read_16(packet, 11);
                    printf("Address resolving succeeded: resolvable address %s, addr type %u\n",
                        bd_addr_to_str(current_pts_address), current_pts_address_type);
                    if (connect_to_resolvable_address){
                        connect_to_resolvable_address = false;
                        gap_stop_scan();
                        printf("Connecting..\n");
                        gap_auto_connection_start(current_pts_address_type, current_pts_address);
                    }
                    break;

                case GAP_EVENT_INQUIRY_RESULT:
                    gap_event_inquiry_result_get_bd_addr(packet, event_address);
                    // print info
                    printf("Device found: %s\n",  bd_addr_to_str(event_address));
                    // get remote name
                    if (memcmp(event_address, public_pts_address, 6) == 0){
                        gap_remote_name_request(event_address, gap_event_inquiry_result_get_page_scan_repetition_mode(packet),
                                                gap_event_inquiry_result_get_clock_offset(packet));
                    }
                    break;

                case HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE:
                    hci_event_remote_name_request_complete_get_bd_addr(packet, event_address);
                    printf("Name: '%s'\n", &packet[9]);
                    break;

                case L2CAP_EVENT_CHANNEL_OPENED:
                    printf("L2CAP Channel Opened, status 0x%02x\n", l2cap_event_channel_opened_get_status(packet));
                    break;
                default:
                    break;
            }
    }
}

static void use_public_pts_address(void){
    memcpy(current_pts_address, public_pts_address, 6);
    current_pts_address_type = public_pts_address_type;                    
}

static void extract_service(gatt_client_service_t * aService, uint8_t * data){
    aService->start_group_handle = little_endian_read_16(data, 0);
    aService->end_group_handle   = little_endian_read_16(data, 2);
    aService->uuid16 = 0;
    reverse_128(&data[4], aService->uuid128);
    if (uuid_has_bluetooth_prefix(aService->uuid128)){
        aService->uuid16 = big_endian_read_32(aService->uuid128, 0);
    }
}

static void extract_characteristic(gatt_client_characteristic_t * characteristic, uint8_t * packet){
    characteristic->start_handle = little_endian_read_16(packet, 4);
    characteristic->value_handle = little_endian_read_16(packet, 6);
    characteristic->end_handle =   little_endian_read_16(packet, 8);
    characteristic->properties =   little_endian_read_16(packet, 10);
    characteristic->uuid16 = 0;
    reverse_128(&packet[12], characteristic->uuid128);
    if (uuid_has_bluetooth_prefix(characteristic->uuid128)){
        characteristic->uuid16 = big_endian_read_32(characteristic->uuid128, 0);
    }
}

static void handle_gatt_client_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);
    UNUSED(size);
    if (packet_type != HCI_EVENT_PACKET) return;

    uint8_t address_type;
    bd_addr_t flipped_address;
    gatt_client_characteristic_t characteristic;
    uint8_t *           value;
    uint16_t            value_handle;
    uint16_t            value_length;
    uint16_t            value_offset;
    uint8_t             status;

    switch(packet[0]){
        case GATT_EVENT_SERVICE_QUERY_RESULT:
            switch (central_state){
                case CENTRAL_W4_PRIMARY_SERVICES:
                case CENTRAL_ENTER_SERVICE_UUID_4_DISCOVER_CHARACTERISTICS:
                    extract_service(&service, &packet[4]);
                    printf("Primary Service with UUID ");
                    printUUID(service.uuid128, service.uuid16);
                    printf(", start group handle 0x%04x, end group handle 0x%04x\n", service.start_group_handle, service.end_group_handle);
                    break;
                    extract_service(&service, &packet[4]);
                    printf("Primary Service with UUID ");
                    printUUID(service.uuid128, service.uuid16);
                    printf(", start group handle 0x%04x, end group handle 0x%04x\n", service.start_group_handle, service.end_group_handle);
                    break;
                default:
                    break;
                }
            break;
        case GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT:
            value_handle = little_endian_read_16(packet, 4);
            extract_service(&service, &packet[6]);
            printf("Included Service at 0x%004x: ", value_handle);
            printf("start group handle 0x%04x, end group handle 0x%04x with UUID ", service.start_group_handle, service.end_group_handle);
            printUUID(service.uuid128, service.uuid16);
            printf("\n");
            break;
        case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
            extract_characteristic(&characteristic, packet);
            switch (central_state) {
                case CENTRAL_W4_NAME_QUERY_COMPLETE:
                    gap_name_characteristic = characteristic;
                    printf("GAP Name Characteristic found, value handle: 0x04%x\n", gap_name_characteristic.value_handle);
                    break;
                case CENTRAL_W4_RECONNECTION_ADDRESS_QUERY_COMPLETE:
                    gap_reconnection_address_characteristic = characteristic;
                    printf("GAP Reconnection Address Characteristic found, value handle: 0x04%x\n", gap_reconnection_address_characteristic.value_handle);
                    break;
                case CENTRAL_W4_PERIPHERAL_PRIVACY_FLAG_QUERY_COMPLETE:
                    gap_peripheral_privacy_flag_characteristic = characteristic;
                    printf("GAP Peripheral Privacy Flag Characteristic found, value handle: 0x04%x\n", gap_peripheral_privacy_flag_characteristic.value_handle);
                    break;
                case CENTRAL_W4_SIGNED_WRITE_QUERY_COMPLETE:
                    signed_write_characteristic = characteristic;
                    printf("Characteristic for Signed Write found, value handle: 0x%04x\n", signed_write_characteristic.value_handle);
                    break;
                case CENTRAL_W4_CHARACTERISTICS:
                    printf("Characteristic found at 0x%04x with value handle 0x%04x, uuid ", characteristic.start_handle, characteristic.value_handle);
                    if (characteristic.uuid16){
                        printf("%04x\n", characteristic.uuid16);
                    } else {
                        printf_hexdump(characteristic.uuid128, 16);                        
                    }
                    break;
                case CENTRAL_GPA_W4_RESPONSE2:
                    switch (ui_uuid16){
                        case GATT_CHARACTERISTIC_PRESENTATION_FORMAT:
                        case GATT_CHARACTERISTIC_AGGREGATE_FORMAT:
                            ui_attribute_handle = characteristic.value_handle;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT: {
            uint16_t descriptor_handle = little_endian_read_16(packet, 4);
            uint8_t uuid128[16];
            reverse_128(&packet[6], uuid128);
            if (uuid_has_bluetooth_prefix(uuid128)){
                printf("Characteristic descriptor at 0x%04x with UUID %04x\n", descriptor_handle, big_endian_read_32(uuid128, 0));
            } else {
                printf("Characteristic descriptor at 0x%04x with UUID %s\n", descriptor_handle, uuid128_to_str(uuid128));
            }
            break;
        }
        case GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT:
            value_handle = little_endian_read_16(packet, 4);
            value_length = little_endian_read_16(packet, 6);
            value = &packet[8];
            switch (central_state){
                case CENTRAL_W4_NAME_VALUE:
                    value[value_length] = 0;
                    printf("GAP Service: Device Name: %s\n", value);
                    break;
                case CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_HANDLE:
                case CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_UUID:
                case CENTRAL_W4_READ_MULTIPLE_CHARACTERISTIC_VALUES:
                    printf("Value: ");
                    printf_hexdump(value, value_length);
                    break;
                case CENTRAL_GPA_W4_RESPONSE:
                    switch (ui_uuid16){
                        case GATT_PRIMARY_SERVICE_UUID:
                            printf ("Attribute handle 0x%04x, primary service 0x%04x\n", value_handle, little_endian_read_16(value,0));
                            break;
                        case GATT_SECONDARY_SERVICE_UUID:
                            printf ("Attribute handle 0x%04x, secondary service 0x%04x\n", value_handle, little_endian_read_16(value,0));
                            break;
                        case GATT_INCLUDE_SERVICE_UUID:
                            printf ("Attribute handle 0x%04x, included service attribute handle 0x%04x, end group handle 0x%04x, uuid %04x\n",
                             value_handle, little_endian_read_16(value,0), little_endian_read_16(value,2), little_endian_read_16(value,4));
                            break;
                        case GATT_CHARACTERISTICS_UUID:
                            printf ("Attribute handle 0x%04x, properties 0x%02x, value handle 0x%04x, uuid ",
                             value_handle, value[0], little_endian_read_16(value,1));
                            if (value_length < 19){
                                printf("%04x\n", little_endian_read_16(value, 3));
                            } else {
                                uint8_t uuid128[16];
                                reverse_128(&value[3], uuid128);
                                printf("%s\n", uuid128_to_str(uuid128));
                            }
                            break;
                        case GATT_CHARACTERISTIC_EXTENDED_PROPERTIES:
                            printf ("Attribute handle 0x%04x, gatt characteristic properties 0x%04x\n", value_handle, little_endian_read_16(value,0));
                            break;
                        case GATT_CHARACTERISTIC_USER_DESCRIPTION:
                            // go the value, but PTS 6.3 requires another request
                            printf("Read by type request received, store attribute handle for read request\n");
                            ui_attribute_handle = value_handle;
                            break;                            
                        case GATT_CLIENT_CHARACTERISTICS_CONFIGURATION:
                            printf ("Attribute handle 0x%04x, gatt client characteristic configuration 0x%04x\n", value_handle, little_endian_read_16(value,0));
                            break;
                        case GATT_CHARACTERISTIC_AGGREGATE_FORMAT:
                            ui_handles_count = value_length >> 1;
                            printf ("Attribute handle 0x%04x, gatt characteristic aggregate format. Handles: ", value_handle);
                            for (ui_handles_index = 0; ui_handles_index < ui_handles_count ; ui_handles_index++){
                                ui_handles[ui_handles_index] = little_endian_read_16(value, (ui_handles_index << 1));
                                printf("0x%04x, ", ui_handles[ui_handles_index]);
                            }
                            printf("\n");
                            ui_handles_index = 0;
                            ui_aggregate_handle = value_handle;
                            break;
                        case GATT_CHARACTERISTIC_PRESENTATION_FORMAT:
                            printf("Presentation format: ");
                            printf_hexdump(value, value_length);
                            memcpy(ui_presentation_format, value, 7);
                            break;
                        default:
                            printf("Value: ");
                            printf_hexdump(value, value_length);
                            break;
                    }
                    break;
                case CENTRAL_GPA_W4_RESPONSE3:
                    switch (ui_uuid16){
                        case GATT_CHARACTERISTIC_PRESENTATION_FORMAT:
                            printf("Value: ");
                            printf_hexdump(value, value_length);
                            printf("Format 0x%02x, Exponent 0x%02x, Unit 0x%04x\n",
                                ui_presentation_format[0], ui_presentation_format[1], little_endian_read_16(ui_presentation_format, 2));                            
                            break;
                        case GATT_CHARACTERISTIC_AGGREGATE_FORMAT:
                            printf("Aggregated value: ");
                            printf_hexdump(value, value_length);
                            memcpy(ui_value_data, value, value_length);
                            ui_value_pos = 0;      
                            central_state = CENTRAL_GPA_W4_RESPONSE4;                 
                        default:
                            break;
                    }
                    break;
               default:
                    break;
            }
            break;
        case GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT:
            value = &packet[10];
            value_offset = little_endian_read_16(packet, 6);
            value_length = little_endian_read_16(packet, 8);
            central_state = CENTRAL_IDLE;
            printf("Value (offset %02u): ", value_offset);
            printf_hexdump(value, value_length);
            break;
        case GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT:
            value_handle = little_endian_read_16(packet, 4);
            value_length = little_endian_read_16(packet, 6);
            value = &packet[8];
            switch (central_state){
                case CENTRAL_GPA_W4_RESPONSE2:
                    switch (ui_uuid16){
                        case GATT_CHARACTERISTIC_USER_DESCRIPTION:
                            value[value_length] = 0;
                            printf ("Attribute handle 0x%04x, characteristic user descriptor: %s\n", value_handle, value);
                            break;
                        default:
                            break;
                    }
                    break;
                case CENTRAL_GPA_W4_RESPONSE4:
                    // only characteristic aggregate format
                    printf("Value: ");
                    printf_hexdump(&ui_value_data[ui_value_pos], gpa_format_type_len[value[0]]);
                    ui_value_pos +=  gpa_format_type_len[value[0]];
                    printf("Format 0x%02x, Exponent 0x%02x, Unit 0x%04x\n",
                        value[0], value[1], little_endian_read_16(value, 2));                            
                    break;
                default:
                    printf("Value: ");
                    printf_hexdump(value, value_length);
                    break;
            }
            break;
        case GATT_EVENT_LONG_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT:
            value = &packet[10];
            value_offset = little_endian_read_16(packet, 6);
            value_length = little_endian_read_16(packet, 8);
            printf("Value (offset %02u): ", value_offset);
            printf_hexdump(value, value_length);
            break;

        case GATT_EVENT_QUERY_COMPLETE:
            status = packet[4];
            if (status){
                central_state = CENTRAL_IDLE;
                printf("GATT_EVENT_QUERY_COMPLETE: %s 0x%02x\n",
                att_error_string_for_code(status),  status);
                break;
            }
            switch (central_state){
                case CENTRAL_W4_NAME_QUERY_COMPLETE:
                    central_state = CENTRAL_W4_NAME_VALUE;
                    gatt_client_read_value_of_characteristic(handle_gatt_client_event, handle, &gap_name_characteristic);
                    break;
                case CENTRAL_W4_RECONNECTION_ADDRESS_QUERY_COMPLETE:
                    central_state = CENTRAL_IDLE;
                    gap_le_get_own_address(&address_type, our_private_address);
                    printf("Our private address: %s\n", bd_addr_to_str(our_private_address));
                    reverse_bd_addr(our_private_address, flipped_address);
                    gatt_client_write_value_of_characteristic(handle_gatt_client_event, handle, gap_reconnection_address_characteristic.value_handle, 6, flipped_address);
                    reconnection_address_set = 1;
#ifdef PTS_USES_RECONNECTION_ADDRESS_FOR_ITSELF
                    memcpy(current_pts_address, our_private_address, 6);
                    current_pts_address_type = 1;
#endif
                    break;
                case CENTRAL_W4_PERIPHERAL_PRIVACY_FLAG_QUERY_COMPLETE:
                    central_state = CENTRAL_IDLE;
                    switch (pts_privacy_flag){
                        case 0:
                            use_public_pts_address();
                            printf("Peripheral Privacy Flag set to FALSE, connecting to public PTS address again\n");
                            gatt_client_write_value_of_characteristic(handle_gatt_client_event, handle, gap_peripheral_privacy_flag_characteristic.value_handle, 1, &pts_privacy_flag);
                            break;
                        case 1:
                            printf("Peripheral Privacy Flag set to TRUE\n");
                            gatt_client_write_value_of_characteristic(handle_gatt_client_event, handle, gap_peripheral_privacy_flag_characteristic.value_handle, 1, &pts_privacy_flag);
                            break;
                        default:
                            break;
                        }
                    break;
                case CENTRAL_W4_SIGNED_WRITE_QUERY_COMPLETE:
                    printf("Signed write on Characteristic with UUID 0x%04x\n", pts_signed_write_characteristic_uuid);
                    gatt_client_signed_write_without_response(handle_gatt_client_event, handle, signed_write_characteristic.value_handle, sizeof(signed_write_value), signed_write_value);
                    break;
                case CENTRAL_W4_PRIMARY_SERVICES:
                    printf("Primary Service Discovery complete\n");
                    central_state = CENTRAL_IDLE;
                    break;
                case CENTRAL_ENTER_SERVICE_UUID_4_DISCOVER_CHARACTERISTICS:
                    gatt_client_discover_characteristics_for_service(handle_gatt_client_event, handle, &service);
                    central_state =  CENTRAL_W4_CHARACTERISTICS;                  
                    break;
                case CENTRAL_GPA_W4_RESPONSE:
                    switch (ui_uuid16){
                        case GATT_CHARACTERISTIC_USER_DESCRIPTION:
                            central_state = CENTRAL_GPA_W4_RESPONSE2;
                            printf("Sending Read Characteristic Descriptor at 0x%04x\n", ui_attribute_handle);
                            gatt_client_read_characteristic_descriptor_using_descriptor_handle(handle_gatt_client_event, handle, ui_attribute_handle);
                            break;
                        case GATT_CHARACTERISTIC_PRESENTATION_FORMAT:
                        case GATT_CHARACTERISTIC_AGGREGATE_FORMAT:
                            {
                                printf("Searching Characteristic Declaration\n");
                                central_state = CENTRAL_GPA_W4_RESPONSE2;
                                gatt_client_service_t aService;                  
                                aService.start_group_handle = ui_start_handle;
                                aService.end_group_handle   = ui_end_handle;
                                gatt_client_discover_characteristics_for_service(handle_gatt_client_event, handle, &aService);
                                break;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case CENTRAL_GPA_W4_RESPONSE2:
                    switch(ui_uuid16){
                        case GATT_CHARACTERISTIC_PRESENTATION_FORMAT:
                        case GATT_CHARACTERISTIC_AGGREGATE_FORMAT:
                            printf("Reading characteristic value at 0x%04x\n", ui_attribute_handle);
                            central_state = CENTRAL_GPA_W4_RESPONSE3;
                            gatt_client_read_value_of_characteristic_using_value_handle(handle_gatt_client_event, handle, ui_attribute_handle);
                            break;
                        default:
                            break;
                    }
                    break;
                case CENTRAL_GPA_W4_RESPONSE4:
                    // so far, only GATT_CHARACTERISTIC_AGGREGATE_FORMAT
                    if (ui_handles_index < ui_handles_count) {
                        printf("Reading Characteristic Presentation Format at 0x%04x\n", ui_handles[ui_handles_index]);
                        gatt_client_read_characteristic_descriptor_using_descriptor_handle(handle_gatt_client_event, handle, ui_handles[ui_handles_index]);
                        ui_handles_index++;
                        break;
                    }
                    if (ui_handles_index == ui_handles_count ) {
                        // PTS rqequires to read the characteristic aggregate descriptor again (no idea why)
                        gatt_client_read_value_of_characteristic_using_value_handle(handle_gatt_client_event, handle, ui_aggregate_handle);
                        ui_handles_index++;
                    }
                    break;
                default:
                    central_state = CENTRAL_IDLE;
                    break;
            }
            break;
        case GATT_EVENT_NOTIFICATION:
            value_handle = little_endian_read_16(packet, 4);
            value_length = little_endian_read_16(packet, 6);
            value = &packet[8];
            printf("Notification handle 0x%04x, value: ", value_handle);
            printf_hexdump(value, value_length);
            break;
        case GATT_EVENT_INDICATION:
            value_handle = little_endian_read_16(packet, 4);
            value_length = little_endian_read_16(packet, 6);
            value = &packet[8];
            printf("Indication handle 0x%04x, value: ", value_handle);
            printf_hexdump(value, value_length);
            break;
        default:
            break;
    }
}

uint16_t value_handle = 1;
uint16_t attribute_size = 1;
int scanning_active = 0;

int num_rows = 0;
int num_lines = 0;
const char * rows[100];
const char * lines[100];
const char * empty_string = "";
const int width = 70;

static void reset_screen(void){
    // free memory
    int i = 0;
    for (i=0;i<num_rows;i++) {
        free((void*)rows[i]);
        rows[i] = NULL;
    }
    num_rows = 0;
    for (i=0;i<num_lines;i++) {
        free((void*)lines[i]);
        lines[i] = NULL;
    }
    num_lines = 0;
}

static void print_line(const char * format, ...){
    va_list argptr;
    va_start(argptr, format);
    char * line = malloc(80);
    vsnprintf(line, 80, format, argptr);
    va_end(argptr);
    lines[num_lines] = line;
    num_lines++;
}

static void printf_row(const char * format, ...){
    va_list argptr;
    va_start(argptr, format);
    char * row = malloc(80);
    vsnprintf(row, 80, format, argptr);
    va_end(argptr);
    rows[num_rows] = row;
    num_rows++;
}

static void print_screen(void){

    // clear screen
    printf("\e[1;1H\e[2J");

    // full lines on top
    int i;
    for (i=0;i<num_lines;i++){
        printf("%s\n", lines[i]);
    }
    printf("\n");

    // two columns
    int second_half = (num_rows + 1) / 2;
    for (i=0;i<second_half;i++){
        int pos = strlen(rows[i]);
        printf("%s", rows[i]);
        while (pos < width){
            printf(" ");
            pos++;
        }
        if (i + second_half < num_rows){
            printf("|  %s", rows[i+second_half]);
        }
        printf("\n");
    }
    printf("\n");
}

static void show_usage(void){
    uint8_t iut_address_type;
    bd_addr_t      iut_address;
    gap_le_get_own_address(&iut_address_type, iut_address);

    reset_screen();

    print_line("--- CLI for BR/EDR/LE Central ---");
    print_line("PTS: addr type %u, addr %s", current_pts_address_type, bd_addr_to_str(current_pts_address));
    print_line("IUT: addr type %u, addr %s", iut_address_type, bd_addr_to_str(iut_address));
    print_line("--------------------------");
    print_line("GAP: connectable %u, bondable %u", gap_connectable, gap_bondable);
    print_line("SM: %s, MITM protection %u, SC %u - SC-only Mode %u", sm_io_capabilities, sm_mitm_protection, sm_sc, sm_sc_only_mode);
    print_line("SM: key range [%u..16], OOB data: %s", sm_min_key_size, 
        sm_have_oob_data ? (sm_have_oob_data == 1 ? (const char*) sm_oob_data_A : (const char*) sm_oob_data_B) : "None");
    print_line("Privacy %u", gap_privacy);
    print_line("Device name: %s", gap_device_name);
    print_line("Security Level: %u", (int) security_level);

    printf_row("c/C - connectable off");
    printf_row("d/D - bondable off/on");
    printf_row("---");
    printf_row("0   - Set Security Level 0");
    printf_row("1   - enable privacy using random non-resolvable private address");
    printf_row("2   - Set Security Level 2");
    printf_row("3   - Set Security Level 3");
    printf_row("4   - Set Security Level 4");
    printf_row("9   - create HCI Classic connection to addr %s", bd_addr_to_str(public_pts_address));
    printf_row("N   - Create L2CAP Connection to PSM 0x1001");
    printf_row("l   - dedicated bonding, MITM %u", sm_mitm_protection);
    printf_row("s/S - passive/active scanning");
    printf_row("a   - enable Advertisements");
    printf_row("b   - start bonding");
    printf_row("n   - query GAP Device Name");
    printf_row("o   - set GAP Reconnection Address");
    printf_row("t   - terminate connection, stop connecting");
    printf_row("f   - set PTS address (for RPA tests)");
    printf_row("F   - Drop bonding information");
    printf_row("H   - Switch to DB for Signed Write Test");
    printf_row("p   - auto connect to PTS");
    printf_row("P   - connect to PTS with RPA");
    printf_row("w   - signed write on characteristic with UUID %04x", pts_signed_write_characteristic_uuid);
    printf_row("W   - signed write on attribute with handle 0x%04x and value 0x12", pts_signed_write_characteristic_handle);
    printf_row("z   - Update Connection Parameters via L2CAP");
    printf_row("Z   - Update Connection Parameters via LL");
    printf_row("---");
    printf_row("A   - Send MTU Exchange");
    printf_row("e   - Discover all Primary Services");
    printf_row("g   - Read Attribute Handle");
    printf_row("i   - General BR/EDR inquiry");
    printf_row("I   - Limited BR/EDR inquiry");
    printf_row("---");
    printf_row("5   - IO_CAPABILITY_DISPLAY_YES_NO");
    printf_row("6   - IO_CAPABILITY_NO_INPUT_NO_OUTPUT");
    printf_row("7   - IO_CAPABILITY_KEYBOARD_ONLY");
    printf_row("8   - IO_CAPABILITY_KEYBOARD_DISPLAY");
    printf_row("j/J - Secure Connections off/on");
    printf_row("k/K - Secure Connections Only Mode off/on");
    printf_row("m/M - MITM protection off");
    printf_row("x/X - encryption key range [7..16]/[16..16]");
    printf_row("y/Y - OOB data off/on/toggle A/B");
    printf_row("---");
    printf_row("Ctrl-c - exit");

    print_screen();
}

static void update_security_level(gap_security_level_t level){
    printf("Security Level: %u (outgoing)\n", (int) level);
    security_level = level;
    gap_set_security_level(level);
}

static void update_auth_req(void){
    uint8_t auth_req = 0;
    if (sm_mitm_protection){
        auth_req |= SM_AUTHREQ_MITM_PROTECTION;
    }
    if (gap_bondable){
        auth_req |= SM_AUTHREQ_BONDING;
    }
    if (sm_sc){
        auth_req |= SM_AUTHREQ_SECURE_CONNECTION;
    }
    sm_set_authentication_requirements(auth_req);
    sm_set_secure_connections_only_mode(sm_sc_only_mode);
    gap_set_secure_connections_only_mode(sm_sc_only_mode);
    //
    if (gap_bondable){
        if (sm_mitm_protection){
            gap_ssp_set_authentication_requirement(SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_GENERAL_BONDING);
        } else {
            gap_ssp_set_authentication_requirement(SSP_IO_AUTHREQ_MITM_PROTECTION_NOT_REQUIRED_GENERAL_BONDING);
        }
    } else {
        if (sm_mitm_protection){
            gap_ssp_set_authentication_requirement(SSP_IO_AUTHREQ_MITM_PROTECTION_REQUIRED_NO_BONDING);
        } else {
            gap_ssp_set_authentication_requirement(SSP_IO_AUTHREQ_MITM_PROTECTION_NOT_REQUIRED_NO_BONDING);
        }
    }
}

static void att_signed_write_handle_cmac_result(uint8_t hash[8]){
    int value_length = sizeof(signed_write_value);
    l2cap_reserve_packet_buffer();
    uint8_t * request = l2cap_get_outgoing_buffer();
    request[0] = ATT_SIGNED_WRITE_COMMAND;
    little_endian_store_16(request, 1, pts_signed_write_characteristic_handle);
    memcpy(&request[3], signed_write_value, value_length);
    little_endian_store_32(request, 3 + value_length, 0);
    reverse_64(hash, &request[3 + value_length + 4]);
    l2cap_send_prepared_connectionless(handle, L2CAP_CID_ATTRIBUTE_PROTOCOL, 3 + value_length + 12);
}

static int hexForChar(char c){
    if (c >= '0' && c <= '9'){
        return c - '0';
    } 
    if (c >= 'a' && c <= 'f'){
        return c - 'a' + 10;
    }      
    if (c >= 'A' && c <= 'F'){
        return c - 'A' + 10;
    } 
    return -1;
}

static bool ui_bd_addr_request;

static void ui_process_bd_addr_request(char c){
    int hex = hexForChar(c);
    if (hex < 0){
        return;
    }
    printf("%c", c);
    ui_uint16 = ui_uint16 << 4 | hex;
    ui_uint16_pos++;

    if (ui_uint16_pos == 2){
        current_pts_address[ui_value_pos++] = ui_uint16;
        ui_uint16_pos = 0;
        ui_uint16 = 0;
        if (ui_value_pos < 6){
            printf(":");
        } else {
            printf("\n");
        }
    }
    fflush(stdout);

    if (ui_value_pos == 6){
        ui_bd_addr_request = false;
        if ((current_pts_address[0] & 0xc0) != 0){
            current_pts_address_type = BD_ADDR_TYPE_LE_RANDOM;
        } else {
            current_pts_address_type = BD_ADDR_TYPE_LE_PUBLIC;
        }
        printf("New address: %s, type %u\n", bd_addr_to_str(current_pts_address), current_pts_address_type);
    }
}

static void ui_request_uint16(const char * message){
    printf("%s", message);
    fflush(stdout);
    ui_uint16_request = 1;
    ui_uint16 = 0;
    ui_uint16_pos = 0;
}

static void ui_request_data(const char * message){
    printf("%s", message);
    fflush(stdout);
    ui_value_request = 1;
    ui_value_pos = 0;
    memset(ui_value_data, 0, sizeof(ui_value_data));
}

static int ui_process_digits_for_passkey(char buffer){
    if (buffer < '0' || buffer > '9') {
        return 0;
    }
    printf("%c", buffer);
    fflush(stdout);
    ui_passkey = ui_passkey * 10 + buffer - '0';
    ui_digits_for_passkey--;
    if (ui_digits_for_passkey == 0){
        printf("\nSending Passkey %u (0x%x)\n", ui_passkey, ui_passkey);
        sm_passkey_input(handle, ui_passkey);
    }
    return 0;
}

static int ui_process_uint16_request(char buffer){
    if (buffer == 0x7f || buffer == 0x08) {
        if (ui_uint16_pos){
            printf("\b \b");
            fflush(stdout);
            ui_uint16 >>= 4;
            ui_uint16_pos--;
        }
        return 0;
    }
    if (buffer == '\n' || buffer == '\r'){
        ui_uint16_request = 0;
        printf("\n");
        switch (central_state){
            case CENTRAL_W4_PRIMARY_SERVICES:
                printf("Discover Primary Services with UUID16 %04x\n", ui_uint16);
                gatt_client_discover_primary_services_by_uuid16(handle_gatt_client_event, handle, ui_uint16);
                return 0;
            case CENTRAL_ENTER_SERVICE_UUID_4_DISCOVER_CHARACTERISTICS:
                printf("Discover Primary Services with UUID16 %04x\n", ui_uint16);
                gatt_client_discover_primary_services_by_uuid16(handle_gatt_client_event, handle, ui_uint16);
                return 0;
            case CENTRAL_ENTER_START_HANDLE_4_DISCOVER_CHARACTERISTICS:
                ui_attribute_handle = ui_uint16;
                ui_request_uint16("Please enter end handle: ");
                central_state = CENTRAL_ENTER_END_HANDLE_4_DISCOVER_CHARACTERISTICS;
                return 0;
            case CENTRAL_ENTER_END_HANDLE_4_DISCOVER_CHARACTERISTICS: {
                printf("Discover Characteristics from 0x%04x to 0x%04x\n", ui_attribute_handle, ui_uint16);
                central_state = CENTRAL_W4_CHARACTERISTICS;
                gatt_client_service_t aService;
                aService.start_group_handle = ui_attribute_handle;
                aService.end_group_handle   = ui_uint16;
                gatt_client_discover_characteristics_for_service(handle_gatt_client_event, handle, &aService);
                return 0;
            }
            case CENTRAL_W4_CHARACTERISTICS:
                printf("Discover Characteristics with UUID16 %04x\n", ui_uint16);
                gatt_client_discover_characteristics_for_handle_range_by_uuid16(handle_gatt_client_event, handle, 0x0001, 0xffff, ui_uint16);
                return 0;
            case CENTRAL_W4_DISCOVER_CHARACTERISTIC_DESCRIPTORS: {
                gatt_client_characteristic_t characteristic;
                characteristic.value_handle = ui_uint16 - 1;
                characteristic.end_handle = ui_uint16;
                gatt_client_discover_characteristic_descriptors(handle_gatt_client_event, handle, &characteristic);
                break;
            }
            case CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_HANDLE:
                printf("Read Characteristic Value with handle 0x%04x\n", ui_uint16);
                gatt_client_read_value_of_characteristic_using_value_handle(handle_gatt_client_event, handle, ui_uint16);
                return 0;
            case CENTRAL_ENTER_OFFSET_4_READ_LONG_CHARACTERISTIC_VALUE_BY_HANDLE:
                ui_attribute_handle = ui_uint16;
                ui_request_uint16("Please enter long value offset: ");
                central_state = CENTRAL_W4_READ_LONG_CHARACTERISTIC_VALUE_BY_HANDLE;
                return 0;
            case CENTRAL_W4_READ_LONG_CHARACTERISTIC_VALUE_BY_HANDLE:
                printf("Read Long Characteristic Value with handle 0x%04x, offset 0x%04x\n", ui_attribute_handle, ui_uint16);
                gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(handle_gatt_client_event, handle, ui_attribute_handle, ui_uint16);
                return 0;
            case CENTRAL_W4_READ_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE:
                printf("Read Characteristic Descriptor with handle 0x%04x\n", ui_uint16);
                gatt_client_read_characteristic_descriptor_using_descriptor_handle(handle_gatt_client_event, handle, ui_uint16);
                return 0;
            case CENTRAL_ENTER_OFFSET_4_READ_LONG_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE:
                ui_attribute_handle = ui_uint16;
                ui_request_uint16("Please enter long characteristic offset: ");
                central_state = CENTRAL_W4_READ_LONG_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE;
                return 0;
            case CENTRAL_W4_READ_LONG_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE:
                printf("Read Long Characteristic Descriptor with handle 0x%04x, offset 0x%04x\n", ui_attribute_handle, ui_uint16);
                gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(handle_gatt_client_event, handle, ui_attribute_handle, ui_uint16);
                return 0;
            case CENTRAL_ENTER_HANDLE_4_READ_CHARACTERISTIC_VALUE_BY_UUID:
                ui_uuid16 = ui_uint16;
                ui_request_uint16("Please enter start handle: ");
                central_state = CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_UUID;
                return 0;
            case CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_UUID:
                printf("Read Characteristic Value with UUID16 0x%04x\n", ui_uint16);
                gatt_client_read_value_of_characteristics_by_uuid16(handle_gatt_client_event, handle, ui_uint16, 0xffff, ui_uuid16);
                return 0;
            case CENTRAL_W4_READ_MULTIPLE_CHARACTERISTIC_VALUES:
                if (ui_uint16){
                    ui_handles[ui_handles_count++] = ui_uint16;
                    ui_request_uint16("Please enter handle: ");
                } else {
                    int i;                        
                    printf("Read multiple values, handles: ");
                    for (i=0;i<ui_handles_count;i++){
                        printf("0x%04x, ", ui_handles[i]);
                    }
                    printf("\n");
                    gatt_client_read_multiple_characteristic_values(handle_gatt_client_event, handle, ui_handles_count, ui_handles);
                }
                return 0;

            case CENTRAL_ENTER_HANDLE_4_WRITE_LONG_CHARACTERISTIC_VALUE:
                ui_attribute_handle = ui_uint16;
                ui_request_uint16("Please enter offset: ");
                central_state = CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_VALUE;
                return 0;
            case CENTRAL_ENTER_HANDLE_4_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR:
                ui_attribute_handle = ui_uint16;
                ui_request_uint16("Please enter offset: ");
                central_state = CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR;
                return 0;
            case CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_VALUE:
            case CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR:
                ui_attribute_offset = ui_uint16;
                ui_request_data("Please enter data: ");
                return 0;
            case CENTRAL_W4_WRITE_WITHOUT_RESPONSE:
            case CENTRAL_W4_WRITE_CHARACTERICISTIC_VALUE:
            case CENTRAL_W4_RELIABLE_WRITE:
            case CENTRAL_W4_WRITE_CHARACTERISTIC_DESCRIPTOR:
            case CENTRAL_W4_SIGNED_WRITE:
                ui_attribute_handle = ui_uint16;
                ui_request_data("Please enter data: ");
                return 0;
            case CENTRAL_W4_ENTER_OFFSET_4_PREPARE_WRITE:
                ui_attribute_offset = ui_uint16;
                ui_request_data("Please enter data: ");
                return 0;
            case CENTRAL_W4_ENTER_HANDLE_4_PREPARE_WRITE:
                ui_attribute_handle = ui_uint16;
                ui_request_uint16("Please enter offset: ");
                central_state = CENTRAL_W4_ENTER_OFFSET_4_PREPARE_WRITE;
                return 0;
            case CENTRAL_GPA_ENTER_START_HANDLE:
                ui_start_handle = ui_uint16;
                central_state = CENTRAL_GPA_ENTER_END_HANDLE;
                ui_request_uint16("Please enter end handle: ");
                return 0;
            case CENTRAL_GPA_ENTER_END_HANDLE:
                ui_end_handle = ui_uint16;
                central_state = CENTRAL_GPA_W4_RESPONSE;
                ui_request_uint16("Please enter uuid: ");
                return 0;
            case CENTRAL_GPA_W4_RESPONSE:
                ui_uuid16 = ui_uint16;
                printf("Read by type: range 0x%04x-0x%04x, uuid %04x\n", ui_start_handle, ui_end_handle, ui_uuid16);
                gatt_client_read_value_of_characteristics_by_uuid16(handle_gatt_client_event, handle, ui_start_handle, ui_end_handle, ui_uuid16);
                return 0;
            default:
                return 0;
        }
    }
    int hex = hexForChar(buffer);
    if (hex < 0){
        return 0;
    }
    printf("%c", buffer);
    fflush(stdout);
    ui_uint16 = ui_uint16 << 4 | hex;
    ui_uint16_pos++;
    return 0;    
}

static int uuid128_pos_starts_with_dash(int pos){
    switch(pos){
        case 8:
        case 12:
        case 16:
        case 20:
#ifdef PTS_UUID128_REPRESENTATION
        case 4:
        case 24:
#endif
            return 1;
        default:
            return 0;
    }
}

static int ui_process_uuid128_request(char buffer){
    if (buffer == '-') return 0;    // skip - 

    if (buffer == 0x7f || buffer == 0x08) {
        if (ui_uuid128_pos){
            if (uuid128_pos_starts_with_dash(ui_uuid128_pos)){
                printf("\b \b");
                fflush(stdout);
            }
            printf("\b \b");
            fflush(stdout);
            ui_uuid128_pos--;
        }
        return 0;
    }

    int hex = hexForChar(buffer);
    if (hex < 0){
        return 0;
    }
    printf("%c", buffer);
    fflush(stdout);
    if (ui_uuid128_pos & 1){
        ui_uuid128[ui_uuid128_pos >> 1] = (ui_uuid128[ui_uuid128_pos >> 1] & 0xf0) | hex;
    } else {
        ui_uuid128[ui_uuid128_pos >> 1] = hex << 4;
    }
    ui_uuid128_pos++;
    if (ui_uuid128_pos == 32){
        ui_uuid128_request = 0;
        printf("\n");
        switch (central_state){
            case CENTRAL_W4_PRIMARY_SERVICES:
                printf("Discover Primary Services with UUID128 %s\n", uuid128_to_str(ui_uuid128));
                gatt_client_discover_primary_services_by_uuid128(handle_gatt_client_event, handle, ui_uuid128);
                return 0;
            case CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_UUID:
                printf("Read Characteristic Value with UUID128 %s\n", uuid128_to_str(ui_uuid128));
                gatt_client_read_value_of_characteristics_by_uuid128(handle_gatt_client_event, handle, 0x0001, 0xffff, ui_uuid128);
                return 0;
            default:
                return 0;
        }
    }
    if (uuid128_pos_starts_with_dash(ui_uuid128_pos)){
        printf("-");
        fflush(stdout);
    }
    return 0;
}

static void ui_announce_write(const char * method){
    printf("Request: %s handle 0x%04x data: ", method, ui_uint16);
    printf_hexdump(ui_value_data, ui_value_pos >> 1);
    printf("\n");
}

static int ui_process_data_request(char buffer){
    if (buffer == 0x7f || buffer == 0x08) {
        if (ui_value_pos){
            if ((ui_value_pos & 1) == 0){
                printf("\b");
            }
            printf("\b \b");
            fflush(stdout);
            ui_value_pos--;
        }
        return 0;
    }
    if (buffer == '\n' || buffer == '\r'){
        ui_value_request = 0;
        printf("\n");
        uint16_t value_len = ui_value_pos >> 1;
        switch (central_state){
            case CENTRAL_W4_WRITE_WITHOUT_RESPONSE:
                ui_announce_write("Write without response");
                gatt_client_write_value_of_characteristic_without_response(handle, ui_attribute_handle, value_len, ui_value_data);
                break;
            case CENTRAL_W4_WRITE_CHARACTERICISTIC_VALUE:
                ui_announce_write("Write Characteristic Value");
                gatt_client_write_value_of_characteristic(handle_gatt_client_event, handle, ui_attribute_handle, value_len, ui_value_data);
                break;
            case CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_VALUE:
                ui_announce_write("Write Long Characteristic Value");
                gatt_client_write_long_value_of_characteristic_with_offset(handle_gatt_client_event, handle, ui_attribute_handle, ui_attribute_offset, value_len, ui_value_data);
                break;
            case CENTRAL_W4_RELIABLE_WRITE:
                ui_announce_write("Reliabe Write");
                gatt_client_reliable_write_long_value_of_characteristic(handle_gatt_client_event, handle, ui_attribute_handle, value_len, ui_value_data);
                break;
            case CENTRAL_W4_WRITE_CHARACTERISTIC_DESCRIPTOR:
                ui_announce_write("Write Characteristic Descriptor");
                gatt_client_write_characteristic_descriptor_using_descriptor_handle(handle_gatt_client_event, handle, ui_attribute_handle, value_len, ui_value_data);
                break;
            case CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR:
                ui_announce_write("Write Long Characteristic Descriptor");
                gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(handle_gatt_client_event, handle, ui_attribute_handle, ui_attribute_offset, value_len, ui_value_data);
                break;
            case CENTRAL_W4_SIGNED_WRITE:
                ui_announce_write("Signed Write");
                gatt_client_signed_write_without_response(handle_gatt_client_event, handle, ui_attribute_handle, value_len, ui_value_data);
                break;
            case CENTRAL_W4_ENTER_OFFSET_4_PREPARE_WRITE:
                ui_announce_write("Preprare Write");
                gatt_client_prepare_write(handle_gatt_client_event, handle, ui_attribute_handle, ui_attribute_offset, value_len, ui_value_data);
                break;
            default:
                break;
        }             
        return 0;   
    }

    // ignore spaces
    if (buffer == ' ') return 0;

    int hex = hexForChar(buffer);
    if (hex < 0){
        return 0;
    }

    printf("%c", buffer);

    if (ui_value_pos & 1){
        ui_value_data[ui_value_pos >> 1] = (ui_value_data[ui_value_pos >> 1] & 0xf0) | hex;
        printf(" ");
    } else {
        ui_value_data[ui_value_pos >> 1] = hex << 4;
    }
    ui_value_pos++;

    fflush(stdout);
    return 0;
}

static void ui_process_command(char buffer){
    int res;
    int i;
    switch (buffer){
        case '1':
            printf("Enabling non-resolvable private address\n");
            gap_random_address_set_mode(GAP_RANDOM_ADDRESS_NON_RESOLVABLE);
            gap_privacy = 1;
            update_advertisement_params();
            break;
        case '0':
            update_security_level(LEVEL_0);
            break;
        case '2':
            update_security_level(LEVEL_2);
            break;
        case '3':
            update_security_level(LEVEL_3);
            break;
        case '4':
            update_security_level(LEVEL_4);
            break;
        case '5':
            sm_io_capabilities = "IO_CAPABILITY_DISPLAY_YES_NO";
            sm_set_io_capabilities(IO_CAPABILITY_DISPLAY_YES_NO);
            printf("IO Capabilities: %s\n", sm_io_capabilities);
            break;
        case '6':
            sm_io_capabilities = "IO_CAPABILITY_NO_INPUT_NO_OUTPUT";
            sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
            printf("IO Capabilities: %s\n", sm_io_capabilities);
            break;
        case '7':
            sm_io_capabilities = "IO_CAPABILITY_KEYBOARD_ONLY";
            sm_set_io_capabilities(IO_CAPABILITY_KEYBOARD_ONLY);
            printf("IO Capabilities: %s\n", sm_io_capabilities);
            break;
        case '8':
            sm_io_capabilities = "IO_CAPABILITY_KEYBOARD_DISPLAY";
            sm_set_io_capabilities(IO_CAPABILITY_KEYBOARD_DISPLAY);
            printf("IO Capabilities: %s\n", sm_io_capabilities);
            break;
        case '9':
            printf("Creating HCI Classic Connection to %s\n", bd_addr_to_str(public_pts_address));
            hci_send_cmd(&hci_create_connection, public_pts_address, hci_usable_acl_packet_types(), 0, 0, 0, 1);
            break;
        case 'N':
            printf("Creating L2CAP Connection to PSM 0x1001\n");
            l2cap_create_channel(&app_packet_handler, public_pts_address, 0x1001, 100, NULL);
            break;
        case 'a':
            gap_advertisements_enable(1);
            printf("Advertisement enabled\n");
            break;
        case 'b':
            printf("Request pairing\n");
            sm_request_pairing(handle);
            break;
        case 'c':
            printf("Non-Connectable\n");
            gap_connectable = 0;
            update_advertisement_params();
            gap_connectable_control(gap_connectable);
            break;
        case 'C':
            printf("Connectable\n");
            gap_connectable = 1;
            update_advertisement_params();
            gap_connectable_control(gap_connectable);
            break;
        case 'd':
            printf("Non-Bondable\n");
            gap_bondable = 0;
            gap_set_bondable_mode(gap_bondable);
            update_auth_req();
            break;
        case 'D':
            printf("Bondable\n");
            gap_bondable = 1;
            gap_set_bondable_mode(gap_bondable);
            update_auth_req();
            break;
        case 'f':
            printf("PTS Address: ");
            fflush(stdout);
            ui_bd_addr_request = true;
            ui_uint16_pos = 0;
            ui_uint16 = 0;
            ui_value_pos = 0;
            break;
        case 'F':
            printf("Drop bonding information\n");
            for (i=0;i<le_device_db_max_count();i++){
                 le_device_db_remove(i);
            }
            gap_drop_link_key_for_bd_addr(public_pts_address);
            break;
        case 'H':
            printf("Switch to dynamic database\n");
            att_set_db(att_db_util_get_address());
            att_db_util_add_service_uuid16(ORG_BLUETOOTH_SERVICE_GENERIC_ATTRIBUTE);
            att_db_util_add_characteristic_uuid16(0xfff8, ATT_PROPERTY_AUTHENTICATED_SIGNED_WRITE | ATT_PROPERTY_DYNAMIC, ATT_SECURITY_AUTHENTICATED, ATT_SECURITY_AUTHENTICATED, NULL, 0);
            break;
        case 'm':
            printf("MITM protection off\n");
            sm_mitm_protection = 0;
            update_auth_req();
            break;
        case 'M':
            printf("MITM protection on\n");
            sm_mitm_protection = 1;
            update_auth_req();
            break;
        case 'j':
            printf("Secure connections OFF\n");
            sm_sc = 0;
            update_auth_req();
            break;
        case 'J':
            printf("Secure connections ON\n");
            sm_sc = 1;
            update_auth_req();
            break;
        case 'k':
            printf("Secure connections Only Mode: OFF\n");
            sm_sc_only_mode = 0;
            update_auth_req();
            break;
        case 'K':
            printf("Secure connections Only Mode: ON\n");
            sm_sc_only_mode = 1;
            update_auth_req();
            break;
        case 'l':
            gap_dedicated_bonding(current_pts_address, sm_mitm_protection);
            break;
        case 'g':
            central_state = CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_HANDLE;
            ui_request_uint16("Attribute Handle: ");
            break;
        case 'G':
            central_state = CENTRAL_W4_WRITE_CHARACTERICISTIC_VALUE;
            ui_request_uint16("Attribute Handle: ");
            break;
        case 'n':
            central_state = CENTRAL_W4_NAME_QUERY_COMPLETE;
            gatt_client_discover_characteristics_for_handle_range_by_uuid16(handle_gatt_client_event, handle, 1, 0xffff, GAP_DEVICE_NAME_UUID);
            break;
        case 'o':
            central_state = CENTRAL_W4_RECONNECTION_ADDRESS_QUERY_COMPLETE;
            gatt_client_discover_characteristics_for_handle_range_by_uuid16(handle_gatt_client_event, handle, 1, 0xffff, GAP_RECONNECTION_ADDRESS_UUID);
            break;
        case 'p':
            res = gap_auto_connection_start(current_pts_address_type, current_pts_address);
            printf("Auto Connection Establishment to type %u, addr %s -> %x\n", current_pts_address_type, bd_addr_to_str(current_pts_address), res);
            break;
#if 0
        case 'P':
            gap_connect(current_pts_address, current_pts_address_type);
            printf("Direct Connection Establishment to type %u, addr %s\n", current_pts_address_type, bd_addr_to_str(current_pts_address));
            break;
#endif
        case 'P':
            connect_to_resolvable_address = true;
            gap_start_scan();
            printf("Connect to PTS with resolvable address\n");
            break;
        case 's':
            if (scanning_active){
                gap_stop_scan();
                scanning_active = 0;
                break;
            }
            printf("Start passive scanning\n");
            gap_set_scan_parameters(0, 48, 48);
            gap_start_scan();
            scanning_active = 1;
            break;
        case 'S':
            if (scanning_active){
                printf("Stop scanning\n");
                gap_stop_scan();
                scanning_active = 0;
                break;
            }
            printf("Start active scanning\n");
            gap_set_scan_parameters(1, 48, 48);
            gap_start_scan();
            scanning_active = 1;
            break;
        case 't':
            printf("Terminating connection\n");
            hci_send_cmd(&hci_disconnect, handle, 0x13);
            gap_auto_connection_stop_all();
            gap_connect_cancel();
            break;
        case 'w':
            pts_privacy_flag = 2;
            central_state = CENTRAL_W4_SIGNED_WRITE_QUERY_COMPLETE;
            gatt_client_discover_characteristics_for_handle_range_by_uuid16(handle_gatt_client_event, handle, 1, 0xffff, pts_signed_write_characteristic_uuid);
            break;
        case 'W':
            // fetch csrk
            le_device_db_local_csrk_get(le_device_db_index, signing_csrk);
            // calc signature
            sm_cmac_signed_write_start(signing_csrk, ATT_SIGNED_WRITE_COMMAND, pts_signed_write_characteristic_handle, sizeof(signed_write_value), signed_write_value, 0, att_signed_write_handle_cmac_result);
            break;
        case 'x':
            sm_min_key_size = 7;
            sm_set_encryption_key_size_range(7, 16);
            show_usage();
            break;
        case 'X':
            sm_min_key_size = 16;
            sm_set_encryption_key_size_range(16, 16);
            show_usage();
            break; 
       case 'y':
            printf("Drop OOB data\n");
            sm_have_oob_data = 0;
            break;
        case 'Y':
            printf("Simulate OOB data\n");
            if (sm_have_oob_data){
                sm_have_oob_data = 3 - sm_have_oob_data;
            } else {
                sm_have_oob_data = 1;
            }
            break;
        case 'z':
            printf("Updating connection parameters via L2CAP\n");
            gap_request_connection_parameter_update(handle, 50, 120, 0, 550);
            break;
        case 'Z':
            printf("Updating connection parameters via LL\n");
            gap_update_connection_parameters(handle, 50, 120, 0, 550);
            break;

        // GATT commands
        case 'e':
            central_state = CENTRAL_W4_PRIMARY_SERVICES;
            gatt_client_discover_primary_services(handle_gatt_client_event, handle);
            break;
        case 'i':
            printf("BR/EDR General Inquiry\n");
            gap_inquiry_set_lap(GAP_IAC_GENERAL_INQUIRY);
            gap_inquiry_start(5);
            break;
        case 'I':
            printf("BR/EDR Limited Inquiry\n");
            gap_inquiry_set_lap(GAP_IAC_LIMITED_INQUIRY);
            gap_inquiry_start(5);
            break;
        case '\n':
        case '\r':
            break;
        default:
            show_usage();
            break;
    }
}

static void stdin_process(char c){
    if (ui_digits_for_passkey){
        ui_process_digits_for_passkey(c);
        return;
    }

    if (ui_uint16_request){
        ui_process_uint16_request(c);
        return;
    }

    if (ui_uuid128_request){
        ui_process_uuid128_request(c);
        return;
    }

    if (ui_value_request){
        ui_process_data_request(c);        
        return;
    }

    if (ui_bd_addr_request){
        ui_process_bd_addr_request(c);
        return;
    }

    ui_process_command(c);
}

static int get_oob_data_callback(uint8_t addres_type, bd_addr_t addr, uint8_t * oob_data){
    UNUSED(addres_type);
    (void)addr;
    switch(sm_have_oob_data){
        case 1:
            memcpy(oob_data, sm_oob_data_A, 16);
            return 1;
        case 2:
            memcpy(oob_data, sm_oob_data_B, 16);
            return 1;
        default:
            return 0;
    }
}

// ATT Client Read Callback for Dynamic Data
// - if buffer == NULL, don't copy data, just return size of value
// - if buffer != NULL, copy data and return number bytes copied
// @param offset defines start of attribute value
static uint16_t att_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    UNUSED(con_handle);
    UNUSED(attribute_handle);
    printf("READ Callback, handle %04x, offset %u, buffer size %u\n", handle, offset, buffer_size);
    uint16_t  att_value_len;

    uint16_t uuid16 = att_uuid_for_handle(handle);
    switch (uuid16){
        case 0x2a00:
            att_value_len = strlen(gap_device_name);
            if (buffer) {
                memcpy(buffer, gap_device_name, att_value_len);
            }
            return att_value_len;        
        default:
            break;
    }
    return 0;
}

static int att_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    UNUSED(con_handle);
    printf("WRITE Callback, handle %04x, mode %u, offset %u, data: ", attribute_handle, transaction_mode, offset);
    printf_hexdump(buffer, buffer_size);
    return 0;
}

int btstack_main(int argc, const char * argv[]);
int btstack_main(int argc, const char * argv[]){
    UNUSED(argc);
    (void)argv;
    printf("BTstack LE Peripheral starting up...\n");

    memset(rows, 0, sizeof(char *) * 100);
    memset(lines, 0, sizeof(char *) * 100);

    strcpy(gap_device_name, "BTstack");

    // register for HCI Events
    hci_event_callback_registration.callback = &app_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    hci_disable_l2cap_timeout_check();

    // gap_set_security_level(LEVEL_4);
    gap_ssp_set_authentication_requirement(0);
    gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);

    // register for SM events
    sm_event_callback_registration.callback = &app_packet_handler;
    sm_add_event_handler(&sm_event_callback_registration);

    // set up l2cap_le
    l2cap_init();
    
    // Setup SM: Display only
    sm_init();
    sm_register_oob_data_callback(get_oob_data_callback);
    sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
    sm_io_capabilities =  "IO_CAPABILITY_NO_INPUT_NO_OUTPUT";
    sm_set_authentication_requirements(SM_AUTHREQ_BONDING);

    sm_set_encryption_key_size_range(sm_min_key_size, 16);
    sm_test_set_irk(test_irk);
    sm_test_use_fixed_local_csrk();

    // setup GATT Client
    gatt_client_init();
    gatt_client_listen_for_characteristic_value_updates(&gatt_client_notification, &handle_gatt_client_event, GATT_CLIENT_ANY_CONNECTION, NULL);

    // Setup ATT/GATT Server
    att_server_init(profile_data, att_read_callback, att_write_callback);
    att_server_register_packet_handler(app_packet_handler);

    // prepare for dynamic db construction
    att_db_util_init();

    // Setup LE Device DB
    le_device_db_init();

    // set adv params
    update_advertisement_params();

    update_auth_req();

    update_security_level(LEVEL_2);

    memcpy(current_pts_address, public_pts_address, 6);
    current_pts_address_type = public_pts_address_type;

    // classic discoverable / connectable
    gap_connectable_control(0);
    gap_discoverable_control(1);

    // allow for terminal input
    btstack_stdin_setup(stdin_process);

    // turn on!
    hci_power_control(HCI_POWER_ON);
    
    return 0;
}
