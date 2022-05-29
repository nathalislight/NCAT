/*
 * Copyright (C) 2018 BlueKitchen GmbH
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
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BLUEKITCHEN
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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

#define BTSTACK_FILE__ "btstack_audio_esp32.c"

/*
 *  btstack_audio_esp32.c
 *
 *  Implementation of btstack_audio.h using polling ESP32 I2S driver
 *
 */

#include "../btstack/src/btstack_config.h"
#include "../btstack/src/btstack_debug.h"
#include "../btstack/src/btstack_audio.h"
#include "../btstack/src/btstack_run_loop.h"
#include "hal_audio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/i2s.h"
#include <string.h>

#ifdef CONFIG_ESP_LYRAT_V4_3_BOARD
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "es8388.h"
#define IIC_DATA                    (GPIO_NUM_18)
#define IIC_CLK                     (GPIO_NUM_23)
#endif

// prototypes
static void btstack_audio_esp32_sink_fill_buffer(void);
static void btstack_audio_esp32_source_process_buffer(void);

///#define BTSTACK_AUDIO_I2S_NUM  (I2S_NUM_0)
#define BTSTACK_AUDIO_I2S_NUM  AUDIO_I2SPORT

#define DRIVER_POLL_INTERVAL_MS          5
#define DMA_BUFFER_COUNT                 2
#define DMA_BUFFER_SAMPLES               512
#define BYTES_PER_SAMPLE_STEREO          4


typedef enum {
    BTSTACK_AUDIO_ESP32_OFF = 0,
    BTSTACK_AUDIO_ESP32_INITIALIZED,
    BTSTACK_AUDIO_ESP32_STREAMING
} btstack_audio_esp32_state_t;

static bool btstack_audio_esp32_i2s_installed;
static bool btstack_audio_esp32_i2s_streaming;
static uint32_t btstack_audio_esp32_i2s_samplerate;

// timer to fill output ring buffer
static btstack_timer_source_t  btstack_audio_esp32_driver_timer;

static uint8_t  btstack_audio_esp32_sink_num_channels;
static uint32_t btstack_audio_esp32_sink_samplerate;

static uint8_t  btstack_audio_esp32_source_num_channels;
static uint32_t btstack_audio_esp32_source_samplerate;

static btstack_audio_esp32_state_t btstack_audio_esp32_sink_state;
static btstack_audio_esp32_state_t btstack_audio_esp32_source_state;

// client
static void (*btstack_audio_esp32_sink_playback_callback)(int16_t * buffer, uint16_t num_samples);
static void (*btstack_audio_esp32_source_recording_callback)(const int16_t * buffer, uint16_t num_samples);

// queue for RX/TX done events
///static QueueHandle_t btstack_audio_esp32_i2s_event_queue;

#ifdef CONFIG_ESP_LYRAT_V4_3_BOARD

static bool btstack_audio_esp32_es8388_initialized;

static es8388_config_t es8388_i2c_cfg = AUDIO_CODEC_ES8388_DEFAULT();

static void btstack_audio_esp32_set_i2s0_mclk(void)
{
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    WRITE_PERI_REG(PIN_CTRL, 0xFFF0);
}

void btstack_audio_esp32_es8388_init(void){
    if (btstack_audio_esp32_es8388_initialized) return;
    btstack_audio_esp32_es8388_initialized = true;

    es8388_init(&es8388_i2c_cfg);
    es8388_config_fmt(ES_MODULE_ADC_DAC, ES_I2S_NORMAL);
    es8388_set_bits_per_sample(ES_MODULE_ADC_DAC, BIT_LENGTH_16BITS);
    es8388_start(ES_MODULE_ADC_DAC);
    es8388_set_volume(70);
    es8388_set_mute(false);
}
#endif

static void btstack_audio_esp32_driver_timer_handler(btstack_timer_source_t * ts){
/*#if I2SAUDIO_ENABLED ///
    // read max 2 events from i2s event queue
    i2s_event_t i2s_event;
    int i;
    for (i=0;i<2;i++){
        ///if( xQueueReceive( btstack_audio_esp32_i2s_event_queue, &i2s_event, 0) == false) break;
        if( xQueueReceive( i2s_event_queue, &i2s_event, 0) == false) break; ///
        switch (i2s_event.type){
            case I2S_EVENT_TX_DONE:
                btstack_audio_esp32_sink_fill_buffer();
                break;
            case I2S_EVENT_RX_DONE:
                btstack_audio_esp32_source_process_buffer();
                break;
            default:
                break;
        }
    }
#else ///
*/
    btstack_audio_esp32_sink_fill_buffer(); ///
///#endif ///

    // re-set timer
    btstack_run_loop_set_timer(ts, DRIVER_POLL_INTERVAL_MS);
    btstack_run_loop_add_timer(ts);
}

static void btstack_audio_esp32_stream_start(void){
    if (btstack_audio_esp32_i2s_streaming) return;

    // start i2s
    log_info("i2s stream start");
#if I2SAUDIO_ENABLED ///
    i2s_start((i2s_port_t)BTSTACK_AUDIO_I2S_NUM);
#endif    

    // start timer
    btstack_run_loop_set_timer_handler(&btstack_audio_esp32_driver_timer, &btstack_audio_esp32_driver_timer_handler);
    btstack_run_loop_set_timer(&btstack_audio_esp32_driver_timer, DRIVER_POLL_INTERVAL_MS);
    btstack_run_loop_add_timer(&btstack_audio_esp32_driver_timer);

    btstack_audio_esp32_i2s_streaming = true;
}

static void btstack_audio_esp32_stream_stop(void){
    if (btstack_audio_esp32_i2s_streaming == false) return;

    // check if still needed
    bool still_needed = (btstack_audio_esp32_sink_state   == BTSTACK_AUDIO_ESP32_STREAMING)
                     || (btstack_audio_esp32_source_state == BTSTACK_AUDIO_ESP32_STREAMING);
    if (still_needed) return;

    // stop timer
    btstack_run_loop_remove_timer(&btstack_audio_esp32_driver_timer);

    // stop i2s
    log_info("i2s stream stop");
#if I2SAUDIO_ENABLED ///
    i2s_stop((i2s_port_t)BTSTACK_AUDIO_I2S_NUM);
#endif    

    btstack_audio_esp32_i2s_streaming = false;
}

static void btstack_audio_esp32_init(void){

    // de-register driver if already installed
    if (btstack_audio_esp32_i2s_installed){
        ///i2s_driver_uninstall(BTSTACK_AUDIO_I2S_NUM);
    }

    // set i2s mode, sample rate and pins based on sink / source config
    i2s_mode_t i2s_mode  = I2S_MODE_MASTER;
    int i2s_data_out_pin = I2S_PIN_NO_CHANGE;
    int i2s_data_in_pin  = I2S_PIN_NO_CHANGE;
    btstack_audio_esp32_i2s_samplerate = 0;

    if (btstack_audio_esp32_sink_state != BTSTACK_AUDIO_ESP32_OFF){
        i2s_mode = (i2s_mode_t)( (int)i2s_mode |  I2S_MODE_TX); // playback ///
        i2s_data_out_pin = GPIO_NUM_26;
        if (btstack_audio_esp32_i2s_samplerate != 0){
            btstack_assert(btstack_audio_esp32_i2s_samplerate == btstack_audio_esp32_sink_samplerate);
        }
        btstack_audio_esp32_i2s_samplerate = btstack_audio_esp32_sink_samplerate;
    }

    if (btstack_audio_esp32_source_state != BTSTACK_AUDIO_ESP32_OFF){
        i2s_mode = (i2s_mode_t)( (int)i2s_mode |  I2S_MODE_RX); // recording ///
        i2s_data_in_pin = GPIO_NUM_35;
        if (btstack_audio_esp32_i2s_samplerate != 0){
            btstack_assert(btstack_audio_esp32_i2s_samplerate == btstack_audio_esp32_source_samplerate);
        }
        btstack_audio_esp32_i2s_samplerate = btstack_audio_esp32_source_samplerate;
    }

    i2s_config_t config =
    {
/*
        .mode                 = i2s_mode,
        .sample_rate          = btstack_audio_esp32_i2s_samplerate,
        .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count        = DMA_BUFFER_COUNT,              // Number of DMA buffers. Max 128.
        .dma_buf_len          = DMA_BUFFER_SAMPLES,            // Size of each DMA buffer in samples. Max 1024.
        .use_apll             = true,
        .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1
*/        
    };
    ///
        config.mode                 = i2s_mode;
        config.sample_rate          = btstack_audio_esp32_i2s_samplerate;
        config.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
        config.channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT;
        config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        config.dma_buf_count        = DMA_BUFFER_COUNT; 
        config.dma_buf_len          = DMA_BUFFER_SAMPLES;
        config.use_apll             = true;
        config.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1;

    i2s_pin_config_t pins =
    {
/*
        .bck_io_num           = GPIO_NUM_5,
        .ws_io_num            = GPIO_NUM_25,
        .data_out_num         = i2s_data_out_pin,
        .data_in_num          = i2s_data_in_pin
*/        
    };
    ///
    pins.bck_io_num           = GPIO_NUM_5;
    pins.ws_io_num            = GPIO_NUM_25;
    pins.data_out_num         = i2s_data_out_pin;
    pins.data_in_num          = i2s_data_in_pin;

#ifdef CONFIG_ESP_LYRAT_V4_3_BOARD
    btstack_audio_esp32_set_i2s0_mclk();
#endif

    log_info("i2s init mode 0x%02x, samplerate %u", i2s_mode, btstack_audio_esp32_sink_samplerate);

    AUDIO_SET_PARAMS(btstack_audio_esp32_i2s_samplerate, 16, 2, 1); ///


///    i2s_driver_install(BTSTACK_AUDIO_I2S_NUM, &config, DMA_BUFFER_COUNT, &btstack_audio_esp32_i2s_event_queue);
///    i2s_set_pin(BTSTACK_AUDIO_I2S_NUM, &pins);

#ifdef CONFIG_ESP_LYRAT_V4_3_BOARD
    btstack_audio_esp32_es8388_init();
#endif

    btstack_audio_esp32_i2s_installed = true;
}

static void btstack_audio_esp32_deinit(void){
    if  (btstack_audio_esp32_i2s_installed == false) return;

    // check if still needed
    bool still_needed = (btstack_audio_esp32_sink_state   != BTSTACK_AUDIO_ESP32_OFF)
                    ||  (btstack_audio_esp32_source_state != BTSTACK_AUDIO_ESP32_OFF);
    if (still_needed) return;

    // uninstall driver
    log_info("i2s close");
///    i2s_driver_uninstall(BTSTACK_AUDIO_I2S_NUM);

    btstack_audio_esp32_i2s_installed = false;
}

// SINK Implementation

uint8_t* btstack_audio_buffer; ///


static void btstack_audio_esp32_sink_fill_buffer(void){
    size_t bytes_written;
///    uint8_t buffer[DMA_BUFFER_SAMPLES * BYTES_PER_SAMPLE_STEREO];
    if (btstack_audio_esp32_sink_state == BTSTACK_AUDIO_ESP32_STREAMING){
///        (*btstack_audio_esp32_sink_playback_callback)((int16_t *) buffer, DMA_BUFFER_SAMPLES);
        (*btstack_audio_esp32_sink_playback_callback)((int16_t *) btstack_audio_buffer, DMA_BUFFER_SAMPLES); ///
        // duplicate samples for mono
        if (btstack_audio_esp32_sink_num_channels == 1){
            int16_t i;
            ///int16_t * buffer16 = (int16_t *) buffer;
            int16_t * buffer16 = (int16_t *) btstack_audio_buffer; ///
            for (i=DMA_BUFFER_SAMPLES-1;i >= 0; i--){
                buffer16[2*i  ] = buffer16[i];
                buffer16[2*i+1] = buffer16[i];
            }
        }
    } else {
        ///memset(buffer, 0, sizeof(buffer));
        memset(btstack_audio_buffer, 0, DMA_BUFFER_SAMPLES * BYTES_PER_SAMPLE_STEREO); ///
    }
    ///i2s_write(BTSTACK_AUDIO_I2S_NUM, buffer, DMA_BUFFER_SAMPLES * BYTES_PER_SAMPLE_STEREO, &bytes_written, portMAX_DELAY);
    bytes_written = AUDIO_WRITE(btstack_audio_buffer,DMA_BUFFER_SAMPLES * BYTES_PER_SAMPLE_STEREO,portMAX_DELAY); ///
    ///bytes_written = DMA_BUFFER_SAMPLES * BYTES_PER_SAMPLE_STEREO;

}

static int btstack_audio_esp32_sink_init(
    uint8_t channels,
    uint32_t samplerate, 
    void (*playback)(int16_t * buffer, uint16_t num_samples)){

    btstack_assert(playback != NULL);
    btstack_assert(channels == 2);

    // store config
    btstack_audio_esp32_sink_playback_callback  = playback;
    btstack_audio_esp32_sink_num_channels       = channels;
    btstack_audio_esp32_sink_samplerate         = samplerate;

    btstack_audio_esp32_sink_state = BTSTACK_AUDIO_ESP32_INITIALIZED;

    // init i2s and codec
    btstack_audio_esp32_init();

    return 0;
}

static void btstack_audio_esp32_sink_set_volume(uint8_t gain) {
#ifdef CONFIG_ESP_LYRAT_V4_3_BOARD
    uint8_t volume_0_100 = (uint8_t) ((((uint16_t) gain) * 100) / 128);
    es8388_set_volume( volume_0_100 );
#else
    UNUSED(gain);
#endif
}

static void btstack_audio_esp32_sink_start_stream(void){

    if (btstack_audio_esp32_sink_state != BTSTACK_AUDIO_ESP32_INITIALIZED) return;

    // validate samplerate
    btstack_assert(btstack_audio_esp32_sink_samplerate == btstack_audio_esp32_i2s_samplerate);

    // state
    btstack_audio_esp32_sink_state = BTSTACK_AUDIO_ESP32_STREAMING;

    // pre-fill HAL buffers
    uint16_t i;
    for (i=0;i<DMA_BUFFER_COUNT;i++){
        btstack_audio_esp32_sink_fill_buffer();
    }

    btstack_audio_esp32_stream_start();
}

static void btstack_audio_esp32_sink_stop_stream(void){

    if (btstack_audio_esp32_sink_state != BTSTACK_AUDIO_ESP32_STREAMING) return;

    // state
    btstack_audio_esp32_sink_state = BTSTACK_AUDIO_ESP32_INITIALIZED;

    btstack_audio_esp32_stream_stop();
}

static void btstack_audio_esp32_sink_close(void){

    if (btstack_audio_esp32_sink_state == BTSTACK_AUDIO_ESP32_STREAMING) {
        btstack_audio_esp32_sink_stop_stream();
    }

    // state
    btstack_audio_esp32_sink_state = BTSTACK_AUDIO_ESP32_OFF;

    btstack_audio_esp32_deinit();
}

static const btstack_audio_sink_t btstack_audio_esp32_sink = {
    /* int (*init)(..);*/                                       &btstack_audio_esp32_sink_init,
    /* void (*set_volume)(uint8_t gain); */                     &btstack_audio_esp32_sink_set_volume,
    /* void (*start_stream(void));*/                            &btstack_audio_esp32_sink_start_stream,
    /* void (*stop_stream)(void)  */                            &btstack_audio_esp32_sink_stop_stream,
    /* void (*close)(void); */                                  &btstack_audio_esp32_sink_close
};

const btstack_audio_sink_t * btstack_audio_esp32_sink_get_instance(void){
    return &btstack_audio_esp32_sink;
}

// SOURCE Implementation

static void btstack_audio_esp32_source_process_buffer(void){
    size_t bytes_read;
    uint8_t buffer[DMA_BUFFER_SAMPLES * BYTES_PER_SAMPLE_STEREO];
///    i2s_read(BTSTACK_AUDIO_I2S_NUM, buffer, DMA_BUFFER_SAMPLES * BYTES_PER_SAMPLE_STEREO, &bytes_read, portMAX_DELAY);
    int16_t * buffer16 = (int16_t *) buffer;
    if (btstack_audio_esp32_source_state == BTSTACK_AUDIO_ESP32_STREAMING) {
        // drop second channel if configured for mono
        if (btstack_audio_esp32_source_num_channels == 1){
            uint16_t i;
            for (i=0;i<DMA_BUFFER_SAMPLES;i++){
                buffer16[i] = buffer16[2*i];
            }
        }
        (*btstack_audio_esp32_source_recording_callback)(buffer16, DMA_BUFFER_SAMPLES);
    }
}

static int btstack_audio_esp32_source_init(
    uint8_t channels,
    uint32_t samplerate, 
    void (*recording)(const int16_t * buffer, uint16_t num_samples)
){
    btstack_assert(recording != NULL);

    // store config
    btstack_audio_esp32_source_recording_callback = recording;
    btstack_audio_esp32_source_num_channels       = channels;
    btstack_audio_esp32_source_samplerate         = samplerate;

    btstack_audio_esp32_source_state = BTSTACK_AUDIO_ESP32_INITIALIZED;

    // init i2s and codec
    btstack_audio_esp32_init();
    return 0;
}

static void btstack_audio_esp32_source_set_gain(uint8_t gain) {
#ifdef CONFIG_ESP_LYRAT_V4_3_BOARD
    // ES8388 supports 0..24 dB gain in 3 dB steps
    uint8_t gain_db = (uint8_t) ( ((uint16_t) gain) * 24 / 127);
    es8388_set_mic_gain( (es_codec_mic_gain_t) gain_db );
#else
    UNUSED(gain);
#endif
}

static void btstack_audio_esp32_source_start_stream(void){

    if (btstack_audio_esp32_source_state != BTSTACK_AUDIO_ESP32_INITIALIZED) return;

    // validate samplerate
    btstack_assert(btstack_audio_esp32_source_samplerate == btstack_audio_esp32_i2s_samplerate);

    // state
    btstack_audio_esp32_source_state = BTSTACK_AUDIO_ESP32_STREAMING;

    btstack_audio_esp32_stream_start();
}

static void btstack_audio_esp32_source_stop_stream(void){

    if (btstack_audio_esp32_source_state != BTSTACK_AUDIO_ESP32_STREAMING) return;

    // state
    btstack_audio_esp32_source_state = BTSTACK_AUDIO_ESP32_INITIALIZED;

    btstack_audio_esp32_stream_stop();
}

static void btstack_audio_esp32_source_close(void){

    if (btstack_audio_esp32_source_state == BTSTACK_AUDIO_ESP32_STREAMING) {
        btstack_audio_esp32_source_stop_stream();
    }

    // state
    btstack_audio_esp32_source_state = BTSTACK_AUDIO_ESP32_OFF;

    btstack_audio_esp32_deinit();
}

static const btstack_audio_source_t btstack_audio_esp32_source = {
    /* int (*init)(..);*/                                       &btstack_audio_esp32_source_init,
    /* void (*set_gain)(uint8_t gain); */                       &btstack_audio_esp32_source_set_gain,
    /* void (*start_stream(void));*/                            &btstack_audio_esp32_source_start_stream,
    /* void (*stop_stream)(void)  */                            &btstack_audio_esp32_source_stop_stream,
    /* void (*close)(void); */                                  &btstack_audio_esp32_source_close
};

const btstack_audio_source_t * btstack_audio_esp32_source_get_instance(void){
    return &btstack_audio_esp32_source;
}
