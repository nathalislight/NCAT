/*
 *
 * Parts from
 *
 * Arduino SdFat Library
 * Copyright (C) 2012 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
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
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef PARTITION_H
#define	PARTITION_H

#include "../Storage.h"

/**
 * PC Partition structures
 */
/*
 * mostly from Microsoft document fatgen103.doc
 * http://www.microsoft.com/whdc/system/platform/firmware/fatgen.mspx
 */
//------------------------------------------------------------------------------
/** Value for byte 510 of boot block or MBR */
uint8_t const BOOTSIG0 = 0X55;
/** Value for byte 511 of boot block or MBR */
uint8_t const BOOTSIG1 = 0XAA;
/** Value for bootSignature field int FAT/FAT32 boot sector */
uint8_t const EXTENDED_BOOT_SIG = 0X29;
//------------------------------------------------------------------------------

/**
 * \struct partitionTable
 * \brief MBR partition table entry
 *
 * A partition table entry for a MBR formatted storage device.
 * The MBR partition table has four entries.
 */
struct partitionTable {
        /**
         * Boot Indicator . Indicates whether the volume is the active
         * partition.  Legal values include: 0X00. Do not use for booting.
         * 0X80 Active partition.
         */
        uint8_t boot;
        /**
         * Head part of Cylinder-head-sector address of the first block in
         * the partition. Legal values are 0-255. Only used in old PC BIOS.
         */
        uint8_t beginHead;
        /**
         * Sector part of Cylinder-head-sector address of the first block in
         * the partition. Legal values are 1-63. Only used in old PC BIOS.
         */
        unsigned beginSector : 6;
        /** High bits cylinder for first block in partition. */
        unsigned beginCylinderHigh : 2;
        /**
         * Combine beginCylinderLow with beginCylinderHigh. Legal values
         * are 0-1023.  Only used in old PC BIOS.
         */
        uint8_t beginCylinderLow;
        /**
         * Partition type. See defines that begin with PART_TYPE_ for
         * some Microsoft partition types.
         */
        uint8_t type;
        /**
         * head part of cylinder-head-sector address of the last sector in the
         * partition.  Legal values are 0-255. Only used in old PC BIOS.
         */
        uint8_t endHead;
        /**
         * Sector part of cylinder-head-sector address of the last sector in
         * the partition.  Legal values are 1-63. Only used in old PC BIOS.
         */
        unsigned endSector : 6;
        /** High bits of end cylinder */
        unsigned endCylinderHigh : 2;
        /**
         * Combine endCylinderLow with endCylinderHigh. Legal values
         * are 0-1023.  Only used in old PC BIOS.
         */
        uint8_t endCylinderLow;
        /** Logical block address of the first block in the partition. */
        uint32_t firstSector;
        /** Length of the partition, in blocks. */
        uint32_t totalSectors;
} __attribute__((packed));
/** Type name for partitionTable */
typedef struct partitionTable part_t;
//------------------------------------------------------------------------------

/**
 * \struct masterBootRecord
 *
 * \brief Master Boot Record
 *
 * The first block of a storage device that is formatted with a MBR.
 */
struct masterBootRecord {
        /** Code Area for master boot program. */
        uint8_t codeArea[440];
        /** Optional Windows NT disk signature. May contain boot code. */
        uint32_t diskSignature;
        /** Usually zero but may be more boot code. */
        uint16_t usuallyZero;
        /** Partition tables. */
        part_t part[4];
        /** First MBR signature byte. Must be 0X55 */
        uint8_t mbrSig0;
        /** Second MBR signature byte. Must be 0XAA */
        uint8_t mbrSig1;
} __attribute__((packed));
/** Type name for masterBootRecord */
typedef struct masterBootRecord mbr_t;

class PCPartition {
public:
        // Array of partitions.
        PCPartition();
        int Init(storage_t *sto);
        virtual ~PCPartition();

        // NULL is returned if there was a problem
        part_t * GetPart(int number);
private:
        // ZERO if all went well.
        int st;
        part_t part[4];
};

#endif	/* PARTITION_H */

