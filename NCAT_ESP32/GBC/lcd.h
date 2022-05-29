#ifndef __LCD_H__
#define __LCD_H__

#include "defs.h"

struct vissprite
{
	short pat;
	short x;
	short v;
	byte pal;
	byte pri;
	//byte pad[2]; //6
};

typedef struct scan_s ///
{
	int bg[64];
	int wnd[64];
	byte buf[256];
	un16 pal2[64];
	byte pri[256];
	struct vissprite vs[16];
	int ns, l, x, y, s, t, u, v, wx, wy, wt, wv;
} scan_t;
scan_t *GBC_scan;

struct obj
{
	byte y;
	byte x;
	byte pat;
	byte flags;
};


uint8_t* lcd_vbank[2];

typedef struct lcd_s
{
	///byte vbank[2][8192];
  ///byte* vbank[2];
 
	union
	{
		byte mem[256];
		struct obj obj[40];
	} oam;
	byte pal[128];
} lcd_t;
lcd_t *GBC_lcd;

///extern struct lcd lcd;
///extern struct scan scan;


void lcd_begin();
void lcd_refreshline();
void pal_write(int i, byte b);
void pal_write_dmg(int i, int mapnum, byte d);
void vram_write(int a, byte b);
void pal_dirty();
void vram_dirty();
void lcd_reset();
//void bg_scan_color();
void updatepatpix();

/* lcdc.c */
void lcdc_trans();
void lcdc_change(byte b);
void stat_write(byte b);
void stat_trigger();


#endif
