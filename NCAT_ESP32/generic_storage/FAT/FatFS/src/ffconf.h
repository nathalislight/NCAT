/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file  R0.09b (C)ChaN, 2013
/----------------------------------------------------------------------------/
/
/ CAUTION! Do not forget to make clean the project after any changes to
/ the configuration options.
/
/----------------------------------------------------------------------------*/
#ifndef _FFCONF
#define _FFCONF 82786	/* Revision ID */

/* Use usb callbacks from c++ */
#define _USB_ 1

#ifndef _FS_LOCK
#define	_FS_LOCK	1	/* 0:Disable or >=1:Enable */
/* To enable file lock control feature, set _FS_LOCK to 1 or greater.
   The value defines how many files can be opened simultaneously. */
#endif

/* Set to 1 to do a proper full mount at mount time instead of deferred. */
#ifndef DO_FULL_MOUNT
#define DO_FULL_MOUNT 1
#endif

/*---------------------------------------------------------------------------/
/ Functions and Buffer Configurations
/----------------------------------------------------------------------------*/

#ifndef _FS_TINY
#define	_FS_TINY	1	/* 0:Normal or 1:Tiny */
#endif
/* When _FS_TINY is set to 1, FatFs uses the sector buffer in the file system
/  object instead of the sector buffer in the individual file object for file
/  data transfer. This reduces memory consumption 512 bytes each file object.
 */

#ifndef _USE_LFN
#define	_USE_LFN        3		/* 0 to 3 */
#endif
#define	_MAX_LFN	255		/* Maximum LFN length to handle (12 to 255) */
/* The _USE_LFN option switches the LFN support.
/
/   0: Disable LFN feature. _MAX_LFN and _LFN_UNICODE have no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT reentrant.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  The LFN working buffer occupies (_MAX_LFN + 1) * 2 bytes. To enable LFN,
/  Unicode handling functions ff_convert() and ff_wtoupper() must be added
/  to the project. When enable to use heap, memory control functions
/  ff_memalloc() and ff_memfree() must be added to the project.

 */

#define _FS_READONLY	0	/* 0:Read/Write or 1:Read only */
/* Setting _FS_READONLY to 1 defines read only configuration. This removes
/  writing functions, f_write, f_sync, f_unlink, f_mkdir, f_chmod, f_rename,
/  f_truncate and useless f_getfree. */


#define _FS_MINIMIZE	0	/* 0 to 3 */
/* The _FS_MINIMIZE option defines minimization level to remove some functions.
/
/   0: Full function.
/   1: f_stat, f_getfree, f_unlink, f_mkdir, f_chmod, f_truncate and f_rename
/      are removed.
/   2: f_opendir and f_readdir are removed in addition to 1.
/   3: f_lseek is removed in addition to 2. */


#define	_USE_STRFUNC	0	/* 0:Disable or 1-3:Enable */
/* To enable string functions, set _USE_STRFUNC to 1 or 2.
 * 1 enable and be sane like Linux
 * > 1 enables everything as text... lame!
 * 3 lame dos LF -> CRLF
 */


#define	_USE_MKFS	1	/* 0:Disable or 1:Enable */
/* To enable f_mkfs function, set _USE_MKFS to 1 and set _FS_READONLY to 0 */

#ifndef _USE_FASTSEEK
#define	_USE_FASTSEEK	0	/* 0:Disable or 1:Enable */
/* To enable fast seek feature, set _USE_FASTSEEK to 1. */
#endif

#define _USE_LABEL	1	/* 0:Disable or 1:Enable */
/* To enable volume label functions, set _USE_LABEL to 1 */


#define	_USE_FORWARD	0	/* 0:Disable or 1:Enable */
/* To enable f_forward function, set _USE_FORWARD to 1 and set _FS_TINY to 1. */


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/----------------------------------------------------------------------------*/

#ifndef _CODE_PAGE
#define _CODE_PAGE 437
/* The _CODE_PAGE specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   932  - Japanese Shift-JIS (DBCS, OEM, Windows)
/   936  - Simplified Chinese GBK (DBCS, OEM, Windows)
/   949  - Korean (DBCS, OEM, Windows)
/   950  - Traditional Chinese Big5 (DBCS, OEM, Windows)
/   1250 - Central Europe (Windows)
/   1251 - Cyrillic (Windows)
/   1252 - Latin 1 (Windows)
/   1253 - Greek (Windows)
/   1254 - Turkish (Windows)
/   1255 - Hebrew (Windows)
/   1256 - Arabic (Windows)
/   1257 - Baltic (Windows)
/   1258 - Vietnam (OEM, Windows)
/   437  - U.S. (OEM)
/   720  - Arabic (OEM)
/   737  - Greek (OEM)
/   775  - Baltic (OEM)
/   850  - Multilingual Latin 1 (OEM)
/   858  - Multilingual Latin 1 + Euro (OEM)
/   852  - Latin 2 (OEM)
/   855  - Cyrillic (OEM)
/   866  - Russian (OEM)
/   857  - Turkish (OEM)
/   862  - Hebrew (OEM)
/   874  - Thai (OEM, Windows)
/   1    - ASCII only (Valid for non LFN cfg.)
/ TO-DO: add 65001 - UTF-8
 */
#endif

#define	_LFN_UNICODE	0	/* 0:ANSI/OEM or 1:Unicode 2: UTF-8 */
/* To switch the character code set on FatFs API to Unicode,
/  enable LFN feature and set _LFN_UNICODE to 1.
 */


#define _FS_RPATH	2	/* 0 to 2 */
/* The _FS_RPATH option configures relative path feature.
/
/   0: Disable relative path feature and remove related functions.
/   1: Enable relative path. f_chdrive() and f_chdir() are available.
/   2: f_getcwd() is available in addition to 1.
/
/  Note that output of the f_readdir function is affected by this option, by
/  turning off returning '.' and '..' entries.
/
/  That's stupid. '.' and '..' are of huge use, I'll have to fix this.
/  For now I have to use the 2 setting.
/  Thankfully avr-ld will throw out unused code, so we are not affected --xxajk
/
 */


/*---------------------------------------------------------------------------/
/ Physical Drive Configurations
/----------------------------------------------------------------------------*/
#ifndef _VOLUMES
#define _VOLUMES	1
/* Number of volumes (logical drives) to be used. */
#endif

#ifndef _MAX_SS
#define	_MAX_SS		512U		/* 512, 1024, 2048 or 4096 */
/* Maximum sector size to be handled.
/  Always set 512 for memory card and hard disk but a larger value may be
/  required for on-board flash memory, floppy disk and optical disk.
/  When _MAX_SS is larger than 512, it configures FatFs to variable sector size
/  and GET_SECTOR_SIZE command must be implemented to the disk_ioctl function. */
#endif


#define	_MULTI_PARTITION	0	/* 0:Single partition, 1:Enable multiple partition */
/* When set to 0, each volume is bound to the same physical drive number and
/ it can mount only first primary partition. When it is set to 1, each volume
/ is tied to the partitions listed in VolToPart[]. */


#define	_USE_ERASE	0	/* 0:Disable or 1:Enable */
/* To enable sector erase feature, set _USE_ERASE to 1. CTRL_ERASE_SECTOR command
/  should be added to the disk_ioctl function. */



/*---------------------------------------------------------------------------/
/ System Configurations
/----------------------------------------------------------------------------*/
#if defined(__AVR__) || defined(memcpy_P)
///#include <avr/pgmspace.h>
#if !defined(ARDUINO_ARCH_SAM)
///#include <avr/io.h>
#else
// First, we define missing stuff that we need for demos.
// And then we fake the rest.
#if !defined(printf_P)
#define printf_P(...) printf(__VA_ARGS__)
#endif
#if !defined(fprintf_P)
#define fprintf_P(s, ...) ((s), __VA_ARGS__)
#endif
#endif

#define _TABLES_IN_PGMSPACE 1
#define GWSTR(X) pgm_read_word_near(&X)
#define GSTR(X) pgm_read_byte_near(&X)
#define GPROGMEM PROGMEM
#define GPSTR(X) PSTR(X)
#define GCONST const
#else
#define _TABLES_IN_PGMSPACE 0
#define GSTR
#define GWSTR
#define GPROGMEM
#define memcpy_P mem_cpy
#define GPSTR(X) X
#define GCONST const
#endif

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN
#define _WORD_ACCESS	1	/* 0 or 1 */
#endif

/* on most arduino, it's LE, 'cept a few... */
#ifndef _WORD_ACCESS
#if defined(__AVR__) || defined(__ARM__)
#define _WORD_ACCESS	1	/* 0 or 1 */
#else
#define _WORD_ACCESS	0	/* 0 or 1 */
#endif
#endif

/* Set 0 first and it is always compatible with all platforms. The _WORD_ACCESS
/  option defines which access method is used to the word data on the FAT volume.
/
/   0: Byte-by-byte access.
/   1: Word access. Do not choose this unless following condition is met.
/
/  When the byte order on the memory is big-endian or address miss-aligned word
/  access results incorrect behavior, the _WORD_ACCESS must be set to 0.
/  If it is not the case, the value can also be set to 1 to improve the
/  performance and code size.
 */


/* A header file that defines sync object types on the O/S, such as
/  windows.h, ucos_ii.h and semphr.h, must be included prior to ff.h. */

#define _FS_REENTRANT           0	/* 0:Disable or 1:Enable */
#define _FS_TIMEOUT		1000	/* Timeout period in unit of time ticks */
/* Stupid, this should use #ifdefs... */
#define	_SYNC_t			HANDLE	/* O/S dependent type of sync object. e.g. HANDLE, OS_EVENT*, ID and etc.. */

/* The _FS_REENTRANT option switches the reentrancy (thread safe) of the FatFs module.
/
/   0: Disable reentrancy. _SYNC_t and _FS_TIMEOUT have no effect.
/   1: Enable reentrancy. Also user provided synchronization handlers,
/      ff_req_grant, ff_rel_grant, ff_del_syncobj and ff_cre_syncobj
/      function must be added to the project. */


#endif /* _FFCONFIG */
