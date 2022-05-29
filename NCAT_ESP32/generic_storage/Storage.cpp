#include "../src/USB_Host_Shield_2.0/Usb.h"
#include "../src/USB_Host_Shield_2.0/masstorage.h"
#include "Storage.h"

#ifdef _usb_h_
BulkOnly *UHS_USB_BulkOnly[MAX_USB_MS_DRIVERS];
#if STORAGE_HAS_MEDIA_CACHE
#define _CACHE_MAX (32768U/_MAX_SS)
uint8_t cache_bank;
storage_cache_t WriteCache[_CACHE_MAX];
uint8_t copyarea[_MAX_SS];
#endif

/**
 * This must be called before using UHS USB Mass Storage. This works around a G++ bug.
 * Thanks to Lei Shi for the heads up.
 */
static void UHS_USB_BulkOnly_Init(void) {
        for(int i = 0; i < MAX_USB_MS_DRIVERS; i++) {
                UHS_USB_BulkOnly[i] = new BulkOnly(&Usb);
        }
#if STORAGE_HAS_MEDIA_CACHE
        //cache_ticker = 0;
        cache_bank = xmem::AllocateExtraBank();
        xmem::Sleep(500);
        for(int i = 0; i < _CACHE_MAX; i++) {
                WriteCache[i].buf = reinterpret_cast<uint8_t *>(0x8000 + (_MAX_SS * i));
                WriteCache[i].sto = NULL;
        }
#endif
}

// On mass storage, there is nothing to do. Just call the status function.

DSTATUS UHS_USB_BulkOnly_Initialize(storage_t *sto) {
        if((UHS_USB_BulkOnly[((pvt_t *)sto->private_data)->B]->WriteProtected(((pvt_t *)sto->private_data)->lun))) {
                return STA_PROTECT;
        } else {
                return STA_OK;
        }
}

bool UHS_USB_BulkOnly_Status(storage_t *sto) {
        return (UHS_USB_BulkOnly[((pvt_t *)sto->private_data)->B]->WriteProtected(((pvt_t *)sto->private_data)->lun));
}


// TO-DO: read cache?

int UHS_USB_BulkOnly_Read(uint32_t LBA, uint8_t *buf, storage_t *sto, uint8_t count) {
        uint8_t x = 0;
        while(count) {
                int tries = FAT_MAX_ERROR_RETRIES;
#if STORAGE_HAS_MEDIA_CACHE
                // Try to read from the cache first.
                for(int i = 0; (i < _CACHE_MAX) && tries; i++) {
                        if(WriteCache[i].LBA == LBA && WriteCache[i].sto == sto) {
                                // Matched!
                                tries = 0;
                                xmem::copy_from_task(buf, WriteCache[i].buf, (sto->SectorSize), cache_bank);
                        }
                }
#endif

                while(tries) {
                        tries--;
                        x = (UHS_USB_BulkOnly[((pvt_t *)sto->private_data)->B]->Read(((pvt_t *)sto->private_data)->lun, LBA, (sto->SectorSize), 1, buf));
                        if(!x) break;
                }
                if(x) break;
                // TO-DO: READ caching?
                count--;
                buf += (sto->SectorSize);
                LBA++;
        }
        int y = x;
        return y;
}

int UHS_USB_BulkOnly_Write(uint32_t LBA, uint8_t *buf, storage_t *sto, uint8_t count) {
        uint8_t x = 0;
        while(count && !x) {
                int tries = FAT_MAX_ERROR_RETRIES;
#if STORAGE_HAS_MEDIA_CACHE
                for(int i = 0; (i < _CACHE_MAX) && tries; i++) {
                        if(WriteCache[i].LBA == LBA && WriteCache[i].sto == sto) {
                                // Matched!
                                xmem::copy_to_task(WriteCache[i].buf, buf, (sto->SectorSize), cache_bank);
                                tries = 0;
                        }
                }
                if(tries) {
                        // No match, anything free?
                        for(int i = 0; (i < _CACHE_MAX) && tries; i++) {
                                if(WriteCache[i].sto == NULL) {
                                        // FREE!
                                        WriteCache[i].sto = sto;
                                        WriteCache[i].LBA = LBA;
                                        xmem::copy_to_task(WriteCache[i].buf, buf, (sto->SectorSize), cache_bank);
                                        tries = 0;
                                }
                        }
                        if(tries) {
                                // Nothing free, commit some of the cache
                                int bigindex[_CACHE_MAX / 4];
                                for(int j = 0; j < _CACHE_MAX / 4; j++) {
                                        bigindex[j] = 0;
                                }
                                for(int i = 0; i < _CACHE_MAX; i++) {
                                        for(int j=0; j<(_CACHE_MAX / 4); j++) {
                                                if(WriteCache[i].LBA > WriteCache[bigindex[j]].LBA) {
                                                        bigindex[j]=i;
                                                        break;
                                                }
                                        }
                                }
                                for(int j = 0; (j < _CACHE_MAX / 4) && !x; j++) {
                                        tries = FAT_MAX_ERROR_RETRIES;
                                        xmem::copy_from_task(copyarea, WriteCache[j].buf, (WriteCache[j].sto->SectorSize), cache_bank);
                                        while(tries) {
                                                tries--;
                                                x = (UHS_USB_BulkOnly[((pvt_t *)WriteCache[j].sto->private_data)->B]->Write(((pvt_t *)WriteCache[j].sto->private_data)->lun, WriteCache[j].LBA, (WriteCache[j].sto->SectorSize), 1, copyarea));
                                                if(x == MASS_ERR_WRITE_PROTECTED) break;
                                                if(!x) break;
                                        }
                                        if(!x) {
                                                WriteCache[j].sto = NULL;
                                        }
                                }
                                if(!x) {
                                        xmem::copy_to_task(WriteCache[bigindex[0]].buf, buf, (sto->SectorSize), cache_bank);
                                        WriteCache[bigindex[0]].sto = sto;
                                        WriteCache[bigindex[0]].LBA = LBA;
                                }
                        }
                }
#else
                while(tries) {
                        tries--;
                        x = (UHS_USB_BulkOnly[((pvt_t *)sto->private_data)->B]->Write(((pvt_t *)sto->private_data)->lun, LBA, (sto->SectorSize), 1, buf));
                        if(x == MASS_ERR_WRITE_PROTECTED) break;
                        if(!x) break;
                }
#endif
                count--;
                buf += (sto->SectorSize);
                LBA++;
        }
        int y = x;
        return y;
}

uint8_t UHS_USB_BulkOnly_Commit(storage_t *sto) {
        uint8_t x = 0;
#if STORAGE_HAS_MEDIA_CACHE
        for(int j = 0; j < _CACHE_MAX; j++) {
                if(WriteCache[j].sto == sto) {
                        // Matched!
                        xmem::copy_from_task(copyarea, WriteCache[j].buf, (WriteCache[j].sto->SectorSize), cache_bank);
                        int tries = FAT_MAX_ERROR_RETRIES;
                        while(tries) {
                                tries--;
                                x = (UHS_USB_BulkOnly[((pvt_t *)WriteCache[j].sto->private_data)->B]->Write(((pvt_t *)WriteCache[j].sto->private_data)->lun, WriteCache[j].LBA, (WriteCache[j].sto->SectorSize), 1, copyarea));
                                if(x == MASS_ERR_WRITE_PROTECTED) break;
                                if(!x) break;
                        }
                        if(!x) {
                                WriteCache[j].sto = NULL;
                        } else {
                                j=_CACHE_MAX;
                        }
                }
        }
#endif
        return x;
}
#endif


// YOUR DRIVERS HERE








// Allow init to happen only once in the case of multiple external inits.
static bool inited = false;

/**
 * This must be called before using generic_storage. Calling more than once is harmless.
 */
void Init_Generic_Storage(void) {
        if(!inited) {
                inited = true;
#ifdef _usb_h_
                UHS_USB_BulkOnly_Init();
#endif

                // YOUR INIT HERE

        }
        return;
}

#if 0

void PollStorage(void) {
#ifdef _usb_h_
        UHS_USB_BulkOnly_Poll();
#endif
        // YOUR POLLING HERE

}
#endif
