/*
 * (CopyLeft) 2013 Andrew J. Kroll
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
///#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FAT.h"
#include "../Storage.h"
#include "FatFS/src/diskio.h"
///#include <RTClib.h>

uint32_t RTClock() {
        ///return RTCnow().FatPacked();
        return(0); ///NCX:
}

PFAT::PFAT(storage_t *sto, uint8_t lv) {
        Create(sto, lv, (uint32_t)0);
}

/* Identify the FAT type. */
PFAT::PFAT(storage_t *sto, uint8_t lv, uint32_t first) {
        Create(sto, lv, first);
}

void PFAT::Create(storage_t *sto, uint8_t lv, uint32_t first) {
        label = NULL;
        ffs = NULL;

        set_clock_call((void*)&RTClock);
        uint8_t buf[sto->SectorSize];
        TCHAR lb[256];
        lb[0] = 0x00;
        int i = 0;
        if(lv > _VOLUMES) return;
        st = (int)(sto->Reads)(first, buf, sto, 1);
        if(!st) {
                fat_boot_t *BR_ = (fat_boot_t *)buf;
                // verify that the sig is OK.
                if(BR_->bootSectorSig0 != 0x55 || BR_->bootSectorSig1 != 0xAA) {
                        //printf_P(PSTR("Bad sig? %02x %02x\r\n"), BR_->bootSectorSig0, BR_->bootSectorSig1);
                        st = -1;
                } else {
                        Offset = first;
                        storage = sto;
                        ffs = new FATFS;
                        ffs->pfat = this;
                        volmap = lv;
                        st = 0xff & f_mount(volmap, ffs);
                        if(!st) {
                                if(label != NULL) {
                                        delete label;
                                        label = NULL;
                                }
                                TCHAR path[4];
                                path[0] = '0' + lv;
                                path[1] = ':';
                                path[2] = '/';
                                path[3] = 0x00;
                                DWORD sn;
                                FRESULT t = f_getlabel(path, lb, &sn);
                                label = (uint8_t *)(operator new[] (13));
                                label[0] = '/';
                                label[1] = 0x00;
                                if(!t) {
                                        for(i = 0; lb[i] != 0x00 && i < 12; i++)
                                                label[i + 1] = lb[i];
                                        label[i + 1] = 0x00;
                                        // We will need to convert 'wide' chars, etc? yuck!
                                        // Life would be a whole lot easier if everything was just UTF-8!
                                }
                                //printf_P(PSTR("VOLUME %i @ '%s'\r\n"), volmap, &label[0]);
                        } else {
                                // Unmount it
                                f_mount(volmap, NULL);
                                //printf_P(PSTR("Mount failed %i(%x)\r\n"), st, st);
                        }
                }
        }
}

int PFAT::MountStatus() {
        return st;
}

/*
    disk_initialize - Initialize disk drive (no need for most devices!)
    disk_status - Get disk status
    disk_read - Read sector(s)
    disk_write - Write sector(s)
    disk_ioctl - Control device dependent features
    get_fattime - Get current time
 */
DSTATUS PFAT::disk_initialize(void) {
        return (storage->Initialize(storage));
}

DSTATUS PFAT::disk_status(void) {
        bool rc = storage->Status(storage);
        if(rc) return STA_OK;
        return STA_PROTECT;
}

DRESULT PFAT::disk_read(FBYTE *buff, DWORD sector, FBYTE count) {
        int rc = storage->Reads(sector, (uint8_t*)buff, storage, count);
        if(rc == 0) return RES_OK;
        return RES_ERROR;
}

DRESULT PFAT::disk_write(const FBYTE *buff, DWORD sector, FBYTE count) {
        int rc = storage->Writes(sector, (uint8_t*)buff, storage, count);
        if(rc == 0) return RES_OK;
        return RES_ERROR;
}

DRESULT PFAT::disk_ioctl(FBYTE cmd, void* buff) {
        switch(cmd) {
                case CTRL_SYNC:
                        break;
                case GET_SECTOR_COUNT:
                        *(DWORD*)buff = storage->TotalSectors;
                        break;
                case GET_SECTOR_SIZE:
                        *(WORD*)buff = storage->SectorSize;
                        break;
                case CTRL_COMMIT:
                        if(!storage->Status(storage)) return RES_WRPRT;
                        if(storage->Commit(storage)) return RES_ERROR;
                        break;
                case CTRL_EJECT:
                        switch(*(int*)buff) {
                                case 0:

                                        if(ffs != NULL) {
                                                if(!storage->Status(storage)) {
                                                   storage->Commit(storage);
                                                }
                                                f_mount(volmap, NULL);
                                                delete ffs;
                                                ffs = NULL;
                                        }
                                        if(label != NULL) {
                                                delete label;
                                                label = NULL;
                                        }
                                        break;
                                default:
                                        // mount
                                        if(ffs != NULL) break;
                                        Create(storage, volmap, Offset);
                                        break;
                        }
                        break;
                default:
                        return RES_PARERR;
        }
        return RES_OK;
}

uint32_t PFAT::get_fattime() {

        return ((clock_call)());
}

void PFAT::set_clock_call(void *call) {

        clock_call = (uint32_t(*)())call;
}

/* This function is defined for only project compatibility               */

void disk_timerproc(void) {
        /* Nothing to do */
}

int PFAT::ReadSectors(uint32_t sector, uint8_t *buf, uint8_t count) {

        return storage->Reads(Offset + sector, buf, storage, count);
}

int PFAT::WriteSectors(uint32_t sector, uint8_t *buf, uint8_t count) {

        return storage->Writes(Offset + sector, buf, storage, count);
}

PFAT::~PFAT() {
        if(ffs != NULL) {
                f_mount(volmap, NULL);
                delete ffs;
                ffs = NULL;
        }

        if(label != NULL) {
                delete label;
                label = NULL;
        }
}

// Allow callbacks from C to C++ class methods.
extern "C" {

        DSTATUS CPP_PFAT_disk_initialize(PFAT *pfat) {
                return pfat->disk_initialize();
        }

        DSTATUS CPP_PFAT_disk_status(PFAT *pfat) {
                return pfat->disk_status();
        }

        DRESULT CPP_PFAT_disk_read(PFAT *pfat, FBYTE *buff, DWORD sector, FBYTE count) {
                return pfat->disk_read(buff, sector, count);
        }

        DRESULT CPP_PFAT_disk_write(PFAT *pfat, const FBYTE *buff, DWORD sector, FBYTE count) {
                return pfat->disk_write(buff, sector, count);
        }

        DRESULT CPP_PFAT_disk_ioctl(PFAT *pfat, FBYTE cmd, void *buff) {
                return pfat->disk_ioctl(cmd, buff);
        }

        uint32_t CPP_PFAT_get_fattime(PFAT *pfat) {
                return pfat->get_fattime();
        }
}
