/*
 * File:   Storage.h
 * Author: root
 *
 * Created on February 19, 2013, 7:44 AM
 */

#ifndef STORAGE_H
#define	STORAGE_H
#include "FAT/FatFS/src/diskio.h"

#ifndef MAX_USB_MS_DRIVERS
#define MAX_USB_MS_DRIVERS 1 // must be 1 to 4
#endif

#ifndef FAT_MAX_ERROR_RETRIES
#define FAT_MAX_ERROR_RETRIES 10
#endif

/*
 * Notes:
 * In order to assist these calls, a pointer to the Storage struct is also passed,
 * so that your driver can get at its own private information, if used.
 * The private_data pointer can point to anything that you need for your driver,
 * or nothing at all if you do not need any. Nothing in the code here will use it.
 *
 * Also, "sector" is the Logical Block Address on the media, starting at ZERO.
 * C/H/S is not, and will not be used!
 * Any translations from the LBA must be done on the storage driver side, if needed.
 * Translations include any sector offsets, logical device numbers, etc.
 * The buffer size is guaranteed to be correct providing that you set SectorSize properly.
 *
 * Success from Read and Write should return ZERO,
 * Errors should return a non-Zero integer meaningful to the storage caller.
 * Negative One is returned by this layer to indicate some other error.
 *
 */


typedef struct Storage {
        int (*Reads)(uint32_t, uint8_t *, struct Storage *, uint8_t); // multiple sector read
        int (*Writes)(uint32_t, uint8_t *, struct Storage *, uint8_t); // multiple sector write
        bool (*Status)(struct Storage *);
        DSTATUS (*Initialize)(struct Storage *);
        uint8_t (*Commit)(struct Storage *);
        uint16_t SectorSize; // physical or translated size on the physical media
        uint32_t TotalSectors; // Total sector count. Used to guard against illegal access.
        void *private_data; // Anything you need, or nothing at all.
} storage_t;

#if defined(USE_MULTIPLE_APP_API)
#include <xmem.h>
#define STORAGE_HAS_MEDIA_CACHE 1
#else
#define STORAGE_HAS_MEDIA_CACHE 0
#endif

#ifdef _usb_h_

extern USB Usb;

extern BulkOnly *UHS_USB_BulkOnly[MAX_USB_MS_DRIVERS];

typedef struct Pvt {
        uint8_t lun; // which LUN
        int B; // which "BulkOnly" instance
} pvt_t;

bool UHS_USB_BulkOnly_Status(storage_t *sto);
DSTATUS UHS_USB_BulkOnly_Initialize(storage_t *sto);
int UHS_USB_BulkOnly_Read(uint32_t LBA, uint8_t *buf, storage_t *sto, uint8_t count);
int UHS_USB_BulkOnly_Write(uint32_t LBA, uint8_t *buf, storage_t *sto, uint8_t count);
uint8_t UHS_USB_BulkOnly_Commit(storage_t *sto);
#endif

// Your driver interface(s) go here...


// TO-DO: abstract to allow multiple interfaces at once, e.g. SD on SPI and USB

// TO-DO: cache reads.
#if STORAGE_HAS_MEDIA_CACHE

// This structure is used to cache writes.
// The idea is to coalesce writes so that media wear is lessened.
typedef struct storage_cache {
        uint8_t *buf; // One sector of data
        storage_t *sto; // NULL if free
        uint32_t LBA; // LBA
        //uint16_t cache_ticker;
} storage_cache_t;
#endif

// Initialize every sub-system
void Init_Generic_Storage(void);

#endif	/* STORAGE_H */
