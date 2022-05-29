#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "defs.h"
#include "regs.h"
#include "mem.h"
#include "hw.h"
#include "rtc.h"
#include "rc.h"
#include "lcd.h"
#include "save.h"
#include "sound.h"
#include "sys.h"

static const int mbc_table[256] =
{
	0, 1, 1, 1, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	3, 3, 3, 3, 0, 0, 0, 0, 0, 5, 5, 5, MBC_RUMBLE, MBC_RUMBLE, MBC_RUMBLE, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MBC_HUC3, MBC_HUC1
};

static const int rtc_table[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0
};

static const int batt_table[256] =
{
	0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0,
	1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
	0
};

static const int romsize_table[256] =
{
	2, 4, 8, 16, 32, 64, 128, 256, 512,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 128, 128, 128
	/* 0, 0, 72, 80, 96  -- actual values but bad to use these! */
};

static const int ramsize_table[256] =
{
	1, 1, 1, 4, 16,
	4 /* FIXME - what value should this be?! */
};


static char *romfile;
static char *sramfile;
static char *rtcfile;
static char *saveprefix;

static char *savename;
static char *savedir;

static int saveslot;

static int forcebatt, nobatt;
static int forcedmg, gbamode;

static int memfill = -1, memrand = -1;


static void initmem(void *mem, int size)
{
	char *p = (char*)mem;
	if (memrand >= 0)
	{
		srand(memrand ? memrand : time(0));
		while(size--) *(p++) = rand();
	}
	else if (memfill >= 0)
		memset(p, memfill, size);
}

static byte* decompress(byte *data, int *len)
{
	unsigned long pos = 0;
	if (data[0] != 0x1f || data[1] != 0x8b)
	{
		printf("not compressed\n");
		return data;
	}

	die("compressed roms not supported\n");
}


int rom_load(const byte * data, int len)
{
	byte c, *header;

///	if (!ram)	ram = calloc(sizeof *ram, 1);
///	if (!rom)	rom = calloc(sizeof *rom, 1);

///  if (!ram) ram = ps_calloc(sizeof *ram, 1);
///  if (!rom) rom = ps_calloc(sizeof *rom, 1);

/// if (!ram || !rom) die("ram=%p rom=%p\n", ram, rom);

///	printf("%s: ram=%p rom=%p\n", __func__, ram, rom);


///	header = data = decompress((const byte*)data, &len);

if (GBC_IN_HIMEM) {
	/*GBC_HIMEM_TMPBANK0_BUFFER=(uint8_t*)ps_malloc(16384);
	GBC_HIMEM_TMPBANK1_BUFFER=(uint8_t*)ps_malloc(16384);
	GBC_HIMEM_TMPBANK2_BUFFER=(uint8_t*)ps_malloc(16384);
	GBC_HIMEM_TMPBANK3_BUFFER=(uint8_t*)ps_malloc(16384);
	GBC_HIMEM_TMPBANK4_BUFFER=(uint8_t*)ps_malloc(16384);
	GBC_HIMEM_TMPBANK5_BUFFER=(uint8_t*)ps_malloc(16384);
	GBC_HIMEM_TMPBANK6_BUFFER=(uint8_t*)ps_malloc(16384);
	GBC_HIMEM_TMPBANK7_BUFFER=(uint8_t*)ps_malloc(16384);*/

  for (uint8_t tmp=0;tmp<64;tmp++) { //check if we have mirrored ROM BANK
     GBC_HIMEM_TMPBANK_BUFFER[tmp]=(uint8_t*)ps_malloc(16384);
	 GBC_HIMEM_TMPBANK_SELECTED[tmp]=256;	 
  }


    ///getRomBank(0);
    ///memcpy(GBC_HIMEM_BANK0_BUFFER,GBC_HIMEM_BANK_BUFFER,16384);


    header = getRomBank(0); 
} else {
    header = (byte*)data; ///
}

	
	memcpy(rom.name, header+0x0134, 16);
	if (rom.name[14] & 0x80) rom.name[14] = 0;
	if (rom.name[15] & 0x80) rom.name[15] = 0;
	rom.name[16] = 0;

	c = header[0x0147];
	mbc.type = mbc_table[c];
	mbc.batt = (batt_table[c] && !nobatt) || forcebatt;
	rtc.batt = rtc_table[c];
	mbc.romsize = romsize_table[header[0x0148]];
	mbc.ramsize = ramsize_table[header[0x0149]];

	printf("ROM name: '%s'\n", rom.name);
	printf("ROM size: %d\n", mbc.romsize);
	printf("RAM size: %d\n", mbc.ramsize);

	if (!mbc.romsize) die("unknown ROM size %02X\n", header[0x0148]);
	if (!mbc.ramsize) die("unknown SRAM size %02X\n", header[0x0149]);

	const int rlen = 16384 * mbc.romsize;
	if (rlen != len) die("ROM len %d != image len %d\n", rlen, len);

	rom.bank = (byte (*)[16384])data; ///
	ram.sbank =  (byte (*)[8192])ps_malloc(8192 * mbc.ramsize); ///
	printf("%s: ram.sbank=%p\n", __func__, ram.sbank);
	if (!ram.sbank) die("RAM allocate %d failed\n", 8192 * mbc.ramsize);

	initmem(ram.sbank, 8192 * mbc.ramsize);
	initmem(ram_ibank, 4096 * 8);

	mbc.rombank = 1;
	mbc.rambank = 0;

	c = header[0x0143];
	hw.cgb = ((c == 0x80) || (c == 0xc0)) && !forcedmg;
	hw.gba = (hw.cgb && gbamode);
//--------------------------------------------------------------------------------
	if (hw.cgb==0 && bootromLoaded==1) {  //no bios boot at DMG
		romBankUnloadBootrom();
	}
//--------------------------------------------------------------------------------

	return 0;
}

///extern uint8_t DRAW;
///extern uint8_t AUDIO;
///extern uint8_t * fb_ram;

void loader_unload()
{
   ///DRAW=0;
   ///AUDIO=0;

///        sram_save();
///        rtc_save();
///        if (romfile) free(romfile);
///        if (sramfile) free(sramfile);
///        if (rtcfile) free(rtcfile);
///        if (saveprefix) free(saveprefix);

///        if (rom.bank) free(rom.bank); ///can not free part of romdata!!!
        if (ram.sbank) free(ram.sbank);
        romfile = sramfile = saveprefix = rtcfile = 0;
        rom.bank = 0;
        ram.sbank = 0;
        mbc.type = mbc.romsize = mbc.ramsize = mbc.batt = 0;

      
        
        lcd_vbank[1]=NULL;
        /*if (lcd_vbank[0])*/ free(lcd_vbank[0]); ///memory free

        ram_ibank[1]=NULL;
        ram_ibank[2]=NULL;
        ram_ibank[3]=NULL;
        ram_ibank[4]=NULL;
        ram_ibank[5]=NULL;
        ram_ibank[6]=NULL;
        ram_ibank[7]=NULL;
        free(ram_ibank[0]); ///memory free

        ///free(fb_ram);
//printf("CHECK1:\n");
		if (GBC_lcd) free(GBC_lcd);
		if (GBC_scan) free(GBC_scan);
		GBC_lcd=NULL; ///must be NULLED!
		GBC_scan=NULL;
//printf("CHECK2:\n");

if (GBC_IN_HIMEM) {
    free(GBC_HIMEM_BANK0_BUFFER);
	GBC_HIMEM_BANK0_BUFFER_LOADED=0;

/*	free(GBC_HIMEM_TMPBANK0_BUFFER);
	free(GBC_HIMEM_TMPBANK1_BUFFER);
	free(GBC_HIMEM_TMPBANK2_BUFFER);
	free(GBC_HIMEM_TMPBANK3_BUFFER);
	free(GBC_HIMEM_TMPBANK4_BUFFER);
	free(GBC_HIMEM_TMPBANK5_BUFFER);
	free(GBC_HIMEM_TMPBANK6_BUFFER);
	free(GBC_HIMEM_TMPBANK7_BUFFER);*/

  for (uint8_t tmp=0;tmp<64;tmp++) { //check if we have mirrored ROM BANK
     free(GBC_HIMEM_TMPBANK_BUFFER[tmp]);
	 GBC_HIMEM_TMPBANK_SELECTED[tmp]=256;
  }
  
  if (GBC_HIMEM_BANK_BUFFER_ALLOCATED) {
	  free (GBC_HIMEM_BANK_BUFFER);
	  GBC_HIMEM_BANK_BUFFER=NULL;
	  GBC_HIMEM_BANK_BUFFER_ALLOCATED=0;
  }	  

  GBC_HIMEM_TMPBANK_UPDATE_POSITION=0;


	
}	


}