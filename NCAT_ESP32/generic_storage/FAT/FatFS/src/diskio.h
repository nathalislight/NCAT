/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2013
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED


#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */

#include "integer.h"
#include "ffconf.h"
#ifdef __cplusplus
extern "C" {
#endif

        /* Status of Disk Functions */
        typedef FBYTE DSTATUS;
        /* Results of Disk Functions */
        typedef enum {
                RES_OK = 0, /* 0: Successful */
                RES_ERROR, /* 1: R/W Error */
                RES_WRPRT, /* 2: Write Protected */
                RES_NOTRDY, /* 3: Not Ready */
                RES_PARERR /* 4: Invalid Parameter */
        } /* __attribute__((packed)) */ DRESULT;


        /*---------------------------------------*/
        /* Prototypes for disk control functions */
#if _USB_ == 1
struct PFAT;
DSTATUS CPP_PFAT_disk_status(struct PFAT*);
DSTATUS CPP_PFAT_disk_initialize(struct PFAT*);
DRESULT CPP_PFAT_disk_read(struct PFAT*, FBYTE* buff, DWORD sector, FBYTE count);
DRESULT CPP_PFAT_disk_write(struct PFAT*, const FBYTE* buff, DWORD sector, FBYTE count);
DRESULT CPP_PFAT_disk_ioctl(struct PFAT*, FBYTE cmd, void* buff);
DWORD CPP_PFAT_get_fattime(struct PFAT*);

#define DISK_STATUS(a)          CPP_PFAT_disk_status(fs->pfat)
#define DISK_INITIALIZE(a)      CPP_PFAT_disk_initialize(fs->pfat)
#define DISK_READ(a,b,c,d)      CPP_PFAT_disk_read(fs->pfat,b,c,d)
#define DISK_WRITE(a,b,c,d)     CPP_PFAT_disk_write(fs->pfat,b,c,d)
#define DISK_IOCTL(a,b,c)       CPP_PFAT_disk_ioctl(fs->pfat,b,c)
#define GET_FATTIME()           CPP_PFAT_get_fattime(fs->pfat)
#endif

        /* Disk Status Bits (DSTATUS) */
#define STA_OK                  0x00    /* Drive OK, not write protected */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


        /* Command code for disk_ioctrl fucntion */

        /* Generic commands used by FatFs */
#define CTRL_SYNC		0x00	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	0x01	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		0x02	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define CTRL_COMMIT             0x03    /* Commit media caches */

        /* Generic command (not used by FatFs) */
#define CTRL_POWER              0x10	/* Get/Set power status */
#define CTRL_LOCK               0x11	/* Lock/Unlock media removal */
#define CTRL_EJECT              0x12	/* Eject(unmount)/Load(mount) media */
#define CTRL_FORMAT             0x13	/* Create physical format on the media */

        /* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE            0x20	/* Get card type */
#define MMC_GET_CSD             0x21	/* Get CSD */
#define MMC_GET_CID             0x22	/* Get CID */
#define MMC_GET_OCR             0x23	/* Get OCR */
#define MMC_GET_SDSTAT          0x24	/* Get SD status */
#define GET_BLOCK_SIZE		0x2E	/* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR	0x2F	/* Force erased a block of sectors (for only _USE_ERASE) */

        /* ATA/CF specific ioctl command */
#define ATA_GET_REV             0x30	/* Get F/W revision */
#define ATA_GET_MODEL           0x31	/* Get model name */
#define ATA_GET_SN              0x32	/* Get serial number */


        /* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC                  0x01		/* MMC ver 3 */
#define CT_SD1                  0x02		/* SD ver 1 */
#define CT_SD2                  0x04		/* SD ver 2 */
#define CT_SDC                  (CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK                0x08		/* Block addressing */


#ifdef __cplusplus
}
#endif

#endif
