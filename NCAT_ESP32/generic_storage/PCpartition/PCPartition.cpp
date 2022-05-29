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
///#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "PCPartition.h"
#include "../FAT/FatFS/src/ffconf.h"

PCPartition::PCPartition() {
}

/* Identify partition types, if any. If no partition exists, default to super mode. */
int PCPartition::Init(storage_t *sto) {
        //printf("Sector size = %i\r\n", sto->SectorSize);

        st = -1;
        if (sto->SectorSize <= _MAX_SS) {

#if 0
                uint8_t *buf = (uint8_t *)malloc(sto->SectorSize);
#else
                // WARNING, CAN FAIL! this requires _MAX_SS stack space.
                uint8_t buf[sto->SectorSize];
#endif
                st = (int)((sto->Reads)(0, buf, sto,1));
                if (!st) {
                        mbr_t *MBR = (mbr_t *)buf;
                        // verify that the partition sig is OK.
                        // Anything else needs to be checked by the file system.
                        if (MBR->mbrSig0 != 0x55 || MBR->mbrSig1 != 0xAA) {
                                //printf_P(PSTR("Bad sig? %02x %02x\r\n"), MBR->mbrSig0, MBR->mbrSig1);
                                st = -1;
                        } else {
                                // Problem... partition seg and MBR sig are identical?!
                                // look for 46  41  54 (FAT) @ 0x36
                                if (buf[0x36] == 0x46 && buf[0x37] == 0x41 && buf[0x38] == 0x54) st = -1;
                                if (!st && buf[0x52] == 0x46 && buf[0x53] == 0x41 && buf[0x54] == 0x54) st = -1;
                                if (!st) {
                                        for (int i = 0; i < 4; i++) {
                                                part[i] = MBR->part[i]; // Wow! This acts like memcpy?!
                                                if (part[i].type != 0x00) {
                                                        if (part[i].boot != 0x80 && part[i].boot != 0x00) st = -1;
                                                }
                                        }
                                }
                        }
                }
#if 0
                free(buf);
#endif
        }
        return st;
}

part_t * PCPartition::GetPart(int number) {
        if (st) return NULL;
        return &(part[number]);
}

PCPartition::~PCPartition() {
}
