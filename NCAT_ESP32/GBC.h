#if GBC_EMULATOR


uint8_t GBC_PAL[256]={0};
uint32_t gbc_frame_time=0;
uint32_t deltatime=0;
uint32_t framenum=0;
uint8_t skipped=0;

#define IS_LITTLE_ENDIAN

//--------------------------------------------------------------------------------
//GNUBOY.h
//--------------------------------------------------------------------------------

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define MESSAGE_ERROR(x, ...) printf("!! %s: " x, __func__, ## __VA_ARGS__)
#define MESSAGE_INFO(x, ...) printf("%s: " x, __func__, ## __VA_ARGS__)
//#define MESSAGE_DEBUG(x, ...) printf("> %s: " x, __func__, ## __VA_ARGS__)
#define MESSAGE_DEBUG(x...) {}

typedef uint8_t byte;
typedef uint8_t un8;
typedef uint16_t un16;
typedef uint32_t un32;
typedef int8_t n8;
typedef int16_t n16;
typedef int32_t n32;
//typedef uint16_t uint16_t;
typedef uint16_t addr_t; // Most efficient but at least 16 bits

int  gnuboy_init(void);
int  gnuboy_load_bios(const char *file);
void gnuboy_free_bios(void);
int  gnuboy_load_rom(const char *file);
void gnuboy_free_rom(void);
void gnuboy_reset(bool hard);
void gnuboy_run(bool draw);
void gnuboy_die(const char *fmt, ...);

void gnuboy_get_time(int *day, int *hour, int *minute, int *second);
void gnuboy_set_time(int day, int hour, int minute, int second);

void gnuboy_load_bank(int);


//--------------------------------------------------------------------------------
//CPU.h
//--------------------------------------------------------------------------------

#pragma once

// Quick access CPU registers 

//#ifdef IS_LITTLE_ENDIAN
#define GBC_LB(r) ((r).b[0])
#define GBC_HB(r) ((r).b[1])
//#else
//#define GBC_LB(r) ((r).b[1])
//#define GBC_HB(r) ((r).b[0])
//#endif

#define GBC_W(r) ((r).w)



#define GBC_A GBC_HB(GBC_cpu.af)
#define GBC_F GBC_LB(GBC_cpu.af)
#define GBC_B GBC_HB(GBC_cpu.bc)
#define GBC_C GBC_LB(GBC_cpu.bc)
#define GBC_D GBC_HB(GBC_cpu.de)
#define GBC_E GBC_LB(GBC_cpu.de)
#define GBC_H GBC_HB(GBC_cpu.hl)
#define GBC_L GBC_LB(GBC_cpu.hl)

#define AF GBC_W(GBC_cpu.af)
#define BC GBC_W(GBC_cpu.bc)
#define DE GBC_W(GBC_cpu.de)
#define HL GBC_W(GBC_cpu.hl)

#define GBC_PC GBC_W(GBC_cpu.pc)
#define GBC_SP GBC_W(GBC_cpu.sp)

#define IMA GBC_cpu.ima
#define IME GBC_cpu.ime

#define FZ 0x80
#define FN 0x40
#define FH 0x20
#define FC 0x10
#define FL 0x0F // low unused portion of flags 

typedef union
{
  byte b[2];
  uint16_t w;
} cpu_reg_t;

typedef struct
{
  cpu_reg_t pc, sp, bc, de, hl, af;

  un32 timer, div;
  un32 ime, ima;
  un32 halted;
  un32 double_speed;
  un32 disassemble;
} cpu_t;

extern cpu_t GBC_cpu;

void cpu_reset(bool hard);
int  cpu_emulate(int cycles);
void cpu_burn(int cycles);
void cpu_disassemble(addr_t a, int c);

//--------------------------------------------------------------------------------
// HW.h
//--------------------------------------------------------------------------------

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define PAD_RIGHT  0x01
#define PAD_LEFT   0x02
#define PAD_UP     0x04
#define PAD_DOWN   0x08
#define PAD_A      0x10
#define PAD_B      0x20
#define PAD_SELECT 0x40
#define PAD_START  0x80

#define IF_VBLANK 0x01
#define IF_STAT   0x02
#define IF_TIMER  0x04
#define IF_SERIAL 0x08
#define IF_PAD    0x10
#define IF_MASK   0x1F

// General internal/io stuff 
#define RI_P1    0x00
#define RI_SB    0x01
#define RI_SC    0x02
#define RI_DIV   0x04
#define RI_TIMA  0x05
#define RI_TMA   0x06
#define RI_TAC   0x07
#define RI_KEY1  0x4D
#define RI_RP    0x56
#define RI_SVBK  0x70

// Interrupts flags 
#define RI_IF    0x0F
#define RI_IE    0xFF

// LCDC 
#define RI_LCDC  0x40
#define RI_STAT  0x41
#define RI_SCY   0x42
#define RI_SCX   0x43
#define RI_LY    0x44
#define RI_LYC   0x45
#define RI_DMA   0x46
#define RI_BGP   0x47
#define RI_OBP0  0x48
#define RI_OBP1  0x49
#define RI_WY    0x4A
#define RI_WX    0x4B
#define RI_VBK   0x4F

#define RI_BIOS  0x50

#define RI_HDMA1 0x51
#define RI_HDMA2 0x52
#define RI_HDMA3 0x53
#define RI_HDMA4 0x54
#define RI_HDMA5 0x55

#define RI_BCPS  0x68
#define RI_BCPD  0x69
#define RI_OCPS  0x6A
#define RI_OCPD  0x6B

// Sound 
#define RI_NR10 0x10
#define RI_NR11 0x11
#define RI_NR12 0x12
#define RI_NR13 0x13
#define RI_NR14 0x14
#define RI_NR21 0x16
#define RI_NR22 0x17
#define RI_NR23 0x18
#define RI_NR24 0x19
#define RI_NR30 0x1A
#define RI_NR31 0x1B
#define RI_NR32 0x1C
#define RI_NR33 0x1D
#define RI_NR34 0x1E
#define RI_NR41 0x20
#define RI_NR42 0x21
#define RI_NR43 0x22
#define RI_NR44 0x23
#define RI_NR50 0x24
#define RI_NR51 0x25
#define RI_NR52 0x26

#define REG(n) hw.ioregs[(n)]

// General internal/io stuff 
#define R_P1    REG(RI_P1)
#define R_SB    REG(RI_SB)
#define R_SC    REG(RI_SC)
#define R_DIV   REG(RI_DIV)
#define R_TIMA  REG(RI_TIMA)
#define R_TMA   REG(RI_TMA)
#define R_TAC   REG(RI_TAC)
#define R_KEY1  REG(RI_KEY1)
#define R_RP    REG(RI_RP)
#define R_SVBK  REG(RI_SVBK)

// Interrupts flags 
#define R_IF    REG(RI_IF)
#define R_IE    REG(RI_IE)

// LCDC 
#define R_LCDC  REG(RI_LCDC)
#define R_STAT  REG(RI_STAT)
#define R_SCY   REG(RI_SCY)
#define R_SCX   REG(RI_SCX)
#define R_LY    REG(RI_LY)
#define R_LYC   REG(RI_LYC)
#define R_DMA   REG(RI_DMA)
#define R_BGP   REG(RI_BGP)
#define R_OBP0  REG(RI_OBP0)
#define R_OBP1  REG(RI_OBP1)
#define R_WY    REG(RI_WY)
#define R_WX    REG(RI_WX)
#define R_VBK   REG(RI_VBK)

#define R_BIOS  REG(RI_BIOS)

#define R_HDMA1 REG(RI_HDMA1)
#define R_HDMA2 REG(RI_HDMA2)
#define R_HDMA3 REG(RI_HDMA3)
#define R_HDMA4 REG(RI_HDMA4)
#define R_HDMA5 REG(RI_HDMA5)

#define R_BCPS  REG(RI_BCPS)
#define R_BCPD  REG(RI_BCPD)
#define R_OCPS  REG(RI_OCPS)
#define R_OCPD  REG(RI_OCPD)

// Sound 
#define R_NR10 REG(RI_NR10)
#define R_NR11 REG(RI_NR11)
#define R_NR12 REG(RI_NR12)
#define R_NR13 REG(RI_NR13)
#define R_NR14 REG(RI_NR14)
#define R_NR21 REG(RI_NR21)
#define R_NR22 REG(RI_NR22)
#define R_NR23 REG(RI_NR23)
#define R_NR24 REG(RI_NR24)
#define R_NR30 REG(RI_NR30)
#define R_NR31 REG(RI_NR31)
#define R_NR32 REG(RI_NR32)
#define R_NR33 REG(RI_NR33)
#define R_NR34 REG(RI_NR34)
#define R_NR41 REG(RI_NR41)
#define R_NR42 REG(RI_NR42)
#define R_NR43 REG(RI_NR43)
#define R_NR44 REG(RI_NR44)
#define R_NR50 REG(RI_NR50)
#define R_NR51 REG(RI_NR51)
#define R_NR52 REG(RI_NR52)

enum {
  MBC_NONE = 0,
  MBC_MBC1,
  MBC_MBC2,
  MBC_MBC3,
  MBC_MBC5,
  MBC_MBC6,
  MBC_MBC7,
  MBC_HUC1,
  MBC_HUC3,
  MBC_MMM01,
};

byte* gb_hw_rambanks[8]; 

typedef struct
{
  //byte rambanks[8][4096];
  byte ioregs[256];
  byte *rmap[0x10];
  byte *wmap[0x10];
  un32 ilines;
  un32 pad;
  un32 cgb;
  n32 hdma;
  n32 serial;
  un8 *bios;
  n32 frames;
} gb_hw_t;

typedef struct
{
  // Memory
  byte *rombanks[512];
  byte (*rambanks)[8192];
  un32 sram_dirty;

  un32 romsize;
  un32 ramsize;

  // Extra hardware
  bool has_rumble;
  bool has_sensor;
  bool has_battery;
  bool has_rtc;

  // MBC stuff
  int mbc;
  int bankmode;
  int enableram;
  int rombank;
  int rambank;

  un16 colorize;
  un16 checksum;
  char name[20];
} gb_cart_t;

extern gb_cart_t GBC_cart;
extern gb_hw_t hw;

void hw_reset(bool hard);
void hw_setpad(byte btn, int set);
void hw_interrupt(byte i, int level);
void hw_hdma(void);
void hw_updatemap(void);
void hw_write(addr_t a, byte b);
byte hw_read(addr_t a);
void hw_vblank(void);
void pad_set(byte btn, int set);


static inline byte readb(addr_t a)
{
  byte *p = hw.rmap[a >> 12];
  if (p) return p[a];
  return hw_read(a);
}

static inline void writeb(addr_t a, byte b)
{
  byte *p = hw.wmap[a >> 12];
  if (p) p[a] = b;
  else hw_write(a, b);
}

static inline uint16_t readw(addr_t a)
{
#ifdef IS_LITTLE_ENDIAN
  if ((a & 0xFFF) == 0xFFF)
#endif
  {
    return readb(a) | (readb(a + 1) << 8);
  }
  byte *p = hw.rmap[a >> 12];
  if (p)
  {
    return *(uint16_t *)(p + a);
  }
  return hw_read(a) | (hw_read(a + 1) << 8);
}

static inline void writew(addr_t a, uint16_t w)
{
#ifdef IS_LITTLE_ENDIAN
  if ((a & 0xFFF) == 0xFFF)
#endif
  {
    writeb(a, w);
    writeb(a + 1, w >> 8);
    return;
  }
  byte *p = hw.wmap[a >> 12];
  if (p)
  {
    *(uint16_t *)(p + a) = w;
    return;
  }
  hw_write(a, w);
  hw_write(a + 1, w >> 8);
}


//--------------------------------------------------------------------------------
// LCD.h
//--------------------------------------------------------------------------------
#pragma once

#define GB_WIDTH (160)
#define GB_HEIGHT (144)

typedef struct
{
  byte y;
  byte x;
  byte pat;
  byte flags;
} gb_obj_t;

typedef struct
{
  int pat, x, v, pal, pri;
} gb_vs_t;

byte* gb_lcd_vbank[2]; // 2 * 8192 bytes

typedef struct
{
  //byte vbank[2][8192];
  union {
    byte mem[256];
    gb_obj_t obj[40];
  } oam;
  byte pal[128];

  int BG[64];
  int WND[64];
  byte BUF[0x100];
  byte PRI[0x100];
  gb_vs_t VS[16];

  int S, GBC_T, U, V;
  int WX, WY, WT, WV;

  int cycles;

  // Fix for Fushigi no Dungeon - Fuurai no Shiren GB2 and Donkey Kong
  int enable_window_offset_hack;

  struct {
    byte *buffer;

    un16 cgb_pal[64];
    un16 dmg_pal[4][4];
    int colorize;
    int format;
    int enabled;
    void (*blit_func)();
  } out;
} gb_lcd_t;

enum
{
  GB_PIXEL_PALETTED,
  GB_PIXEL_565_LE,
  GB_PIXEL_565_BE,
};

enum
{
  GB_PALETTE_GBCBIOS,
  GB_PALETTE_DEFAULT,
  GB_PALETTE_2BGRAYS,
  GB_PALETTE_LINKSAW,
  GB_PALETTE_NSUPRGB,
  GB_PALETTE_NGBARNE,
  GB_PALETTE_GRAPEFR,
  GB_PALETTE_MEGAMAN,
  GB_PALETTE_POKEMON,
  GB_PALETTE_DMGREEN,
  GB_PALETTE_COUNT,
};


extern gb_lcd_t lcd;

void lcd_reset(bool hard);
void lcd_emulate(void);
void lcd_rebuildpal(void);
void lcd_stat_trigger(void);
void lcd_lcdc_change(byte b);

void pal_write_cgb(byte i, byte b);
void pal_write_dmg(byte i, byte mapnum, byte d);


//--------------------------------------------------------------------------------
// RTC.h
//--------------------------------------------------------------------------------
#ifndef __RTC_H__
#define __RTC_H__

#include <stdio.h>

typedef struct
{
  n32 sel, flags, latch, dirty;
  n32 ticks; // Ticks (60 = +1s)
  n32 d, h, m, s; // Current time
  n32 regs[5]; // Latched time
} gb_rtc_t;

extern gb_rtc_t rtc;

void rtc_latch(byte b);
void rtc_write(byte b);
int rtc_save(const char *file);
int rtc_load(const char *file);
void rtc_tick();
void rtc_reset(bool hard);

#endif

//--------------------------------------------------------------------------------
// NOISE.h
//--------------------------------------------------------------------------------

static const byte noise7[] =
{
  0xfb, 0xe7, 0xae, 0x1b, 0xa6, 0x2b, 0x05, 0xe3,
  0xb6, 0x4a, 0x42, 0x72, 0xd1, 0x19, 0xaa, 0x03,
};

static const byte noise15[] =
{
  0xff, 0xfb, 0xff, 0xe7, 0xff, 0xaf, 0xfe, 0x1f,
  0xfb, 0xbf, 0xe6, 0x7f, 0xaa, 0xfe, 0x01, 0xfb,
  0xfb, 0xe7, 0xe7, 0xaf, 0xae, 0x1e, 0x1b, 0xbb,
  0xa6, 0x66, 0x2a, 0xab, 0x00, 0x05, 0xff, 0xe3,
  0xff, 0xb7, 0xfe, 0x4f, 0xfa, 0x5f, 0xe2, 0x3f,
  0xb3, 0x7e, 0x54, 0xfa, 0x05, 0xe3, 0xe3, 0xb7,
  0xb6, 0x4e, 0x4a, 0x5a, 0x42, 0x22, 0x73, 0x32,
  0xd5, 0x51, 0x00, 0x19, 0xff, 0xab, 0xfe, 0x07,
  0xfb, 0xef, 0xe7, 0x9f, 0xae, 0xbe, 0x18, 0x7b,
  0xae, 0xe6, 0x19, 0xab, 0xaa, 0x06, 0x03, 0xeb,
  0xf7, 0x87, 0xce, 0xef, 0x59, 0x9c, 0x2a, 0xb7,
  0x00, 0x4d, 0xfe, 0x53, 0xfa, 0x17, 0xe3, 0x8f,
  0xb6, 0xde, 0x49, 0x3a, 0x49, 0x62, 0x48, 0xb2,
  0x4c, 0x52, 0x56, 0x12, 0x0b, 0x93, 0xc6, 0x97,
  0x68, 0x8c, 0x8c, 0xd4, 0xd5, 0x05, 0x01, 0xe1,
  0xfb, 0xbb, 0xe6, 0x67, 0xaa, 0xae, 0x00, 0x1b,
  0xff, 0xa7, 0xfe, 0x2f, 0xfb, 0x1f, 0xe5, 0xbf,
  0xa2, 0x7e, 0x32, 0xfb, 0x51, 0xe4, 0x1b, 0xa7,
  0xa6, 0x2e, 0x2b, 0x1b, 0x05, 0xa5, 0xe2, 0x23,
  0xb3, 0x36, 0x55, 0x4a, 0x00, 0x43, 0xfe, 0x77,
  0xfa, 0xcf, 0xe1, 0x5f, 0xb8, 0x3e, 0x6f, 0x7a,
  0x9c, 0xe0, 0xb5, 0xbc, 0x42, 0x76, 0x72, 0xca,
  0xd1, 0x41, 0x18, 0x79, 0xae, 0xea, 0x19, 0x83,
  0xaa, 0xf6, 0x01, 0xcb, 0xfb, 0x47, 0xe4, 0x6f,
  0xa6, 0x9e, 0x28, 0xbb, 0x0c, 0x65, 0xd6, 0xa3,
  0x08, 0x35, 0xcf, 0x43, 0x5c, 0x74, 0x36, 0xc7,
  0x49, 0x6c, 0x48, 0x96, 0x4c, 0x8a, 0x54, 0xc2,
  0x05, 0x73, 0xe0, 0xd7, 0xbd, 0x0e, 0x71, 0xda,
  0xdb, 0x21, 0x25, 0x39, 0x21, 0x69, 0x38, 0x89,
  0x6c, 0xc8, 0x95, 0x4c, 0x80, 0x54, 0xfe, 0x05,
  0xfb, 0xe3, 0xe7, 0xb7, 0xae, 0x4e, 0x1a, 0x5b,
  0xa2, 0x26, 0x33, 0x2b, 0x55, 0x04, 0x01, 0xe7,
  0xfb, 0xaf, 0xe6, 0x1f, 0xab, 0xbe, 0x06, 0x7b,
  0xea, 0xe7, 0x81, 0xae, 0xfa, 0x19, 0xe3, 0xab,
  0xb6, 0x06, 0x4b, 0xea, 0x47, 0x82, 0x6e, 0xf2,
  0x99, 0xd0, 0xab, 0x1c, 0x05, 0xb7, 0xe2, 0x4f,
  0xb2, 0x5e, 0x52, 0x3a, 0x13, 0x63, 0x94, 0xb6,
  0x84, 0x48, 0xe6, 0x4d, 0xaa, 0x52, 0x02, 0x13,
  0xf3, 0x97, 0xd6, 0x8f, 0x08, 0xdd, 0xcd, 0x33,
  0x51, 0x54, 0x18, 0x07, 0xaf, 0xee, 0x1f, 0x9b,
  0xbe, 0xa6, 0x78, 0x2a, 0xef, 0x01, 0x9d, 0xfa,
  0xb3, 0xe0, 0x57, 0xbe, 0x0e, 0x7b, 0xda, 0xe7,
  0x21, 0xad, 0x3a, 0x11, 0x63, 0x98, 0xb6, 0xac,
  0x48, 0x16, 0x4f, 0x8a, 0x5e, 0xc2, 0x39, 0x73,
  0x68, 0xd4, 0x8d, 0x04, 0xd1, 0xe5, 0x1b, 0xa1,
  0xa6, 0x3a, 0x2b, 0x63, 0x04, 0xb5, 0xe4, 0x43,
  0xa6, 0x76, 0x2a, 0xcb, 0x01, 0x45, 0xf8, 0x63,
  0xee, 0xb7, 0x98, 0x4e, 0xae, 0x58, 0x1a, 0x2f,
  0xa3, 0x1e, 0x35, 0xbb, 0x42, 0x64, 0x72, 0xa6,
  0xd0, 0x29, 0x1f, 0x09, 0xbd, 0xca, 0x73, 0x42,
  0xd4, 0x71, 0x06, 0xd9, 0xe9, 0x2b, 0x89, 0x06,
  0xc9, 0xe9, 0x4b, 0x88, 0x46, 0xce, 0x69, 0x5a,
  0x88, 0x20, 0xcf, 0x3d, 0x5d, 0x70, 0x30, 0xdf,
  0x5d, 0x3c, 0x31, 0x77, 0x58, 0xcc, 0x2d, 0x57,
  0x10, 0x0d, 0x9f, 0xd2, 0xbf, 0x10, 0x7d, 0x9e,
  0xf2, 0xb9, 0xd0, 0x6b, 0x1e, 0x85, 0xb8, 0xe2,
  0x6d, 0xb2, 0x92, 0x50, 0x92, 0x1c, 0x93, 0xb4,
  0x96, 0x44, 0x8a, 0x64, 0xc2, 0xa5, 0x70, 0x20,
  0xdf, 0x3d, 0x3d, 0x71, 0x70, 0xd8, 0xdd, 0x2d,
  0x31, 0x11, 0x59, 0x98, 0x2a, 0xaf, 0x00, 0x1d,
  0xff, 0xb3, 0xfe, 0x57, 0xfa, 0x0f, 0xe3, 0xdf,
  0xb7, 0x3e, 0x4d, 0x7a, 0x50, 0xe2, 0x1d, 0xb3,
  0xb2, 0x56, 0x52, 0x0a, 0x13, 0xc3, 0x97, 0x76,
  0x8c, 0xc8, 0xd5, 0x4d, 0x00, 0x51, 0xfe, 0x1b,
  0xfb, 0xa7, 0xe6, 0x2f, 0xab, 0x1e, 0x05, 0xbb,
  0xe2, 0x67, 0xb2, 0xae, 0x50, 0x1a, 0x1f, 0xa3,
  0xbe, 0x36, 0x7b, 0x4a, 0xe4, 0x41, 0xa6, 0x7a,
  0x2a, 0xe3, 0x01, 0xb5, 0xfa, 0x43, 0xe2, 0x77,
  0xb2, 0xce, 0x51, 0x5a, 0x18, 0x23, 0xaf, 0x36,
  0x1d, 0x4b, 0xb0, 0x46, 0x5e, 0x6a, 0x3a, 0x83,
  0x60, 0xf4, 0xbd, 0xc4, 0x73, 0x66, 0xd4, 0xa9,
  0x04, 0x09, 0xe7, 0xcb, 0xaf, 0x46, 0x1c, 0x6b,
  0xb6, 0x86, 0x48, 0xea, 0x4d, 0x82, 0x52, 0xf2,
  0x11, 0xd3, 0x9b, 0x16, 0xa5, 0x88, 0x22, 0xcf,
  0x31, 0x5d, 0x58, 0x30, 0x2f, 0x5f, 0x1c, 0x3d,
  0xb7, 0x72, 0x4c, 0xd2, 0x55, 0x12, 0x01, 0x93,
  0xfa, 0x97, 0xe0, 0x8f, 0xbc, 0xde, 0x75, 0x3a,
  0xc1, 0x61, 0x78, 0xb8, 0xec, 0x6d, 0x96, 0x92,
  0x88, 0x90, 0xcc, 0x9d, 0x54, 0xb0, 0x04, 0x5f,
  0xe6, 0x3f, 0xab, 0x7e, 0x04, 0xfb, 0xe5, 0xe7,
  0xa3, 0xae, 0x36, 0x1b, 0x4b, 0xa4, 0x46, 0x26,
  0x6b, 0x2a, 0x85, 0x00, 0xe1, 0xfd, 0xbb, 0xf2,
  0x67, 0xd2, 0xaf, 0x10, 0x1d, 0x9f, 0xb2, 0xbe,
  0x50, 0x7a, 0x1e, 0xe3, 0xb9, 0xb6, 0x6a, 0x4a,
  0x82, 0x40, 0xf2, 0x7d, 0xd2, 0xf3, 0x11, 0xd5,
  0x9b, 0x02, 0xa5, 0xf0, 0x23, 0xdf, 0x37, 0x3d,
  0x4d, 0x70, 0x50, 0xde, 0x1d, 0x3b, 0xb1, 0x66,
  0x58, 0xaa, 0x2c, 0x03, 0x17, 0xf5, 0x8f, 0xc2,
  0xdf, 0x71, 0x3c, 0xd9, 0x75, 0x28, 0xc1, 0x0d,
  0x79, 0xd0, 0xeb, 0x1d, 0x85, 0xb2, 0xe2, 0x51,
  0xb2, 0x1a, 0x53, 0xa2, 0x16, 0x33, 0x8b, 0x56,
  0xc4, 0x09, 0x67, 0xc8, 0xaf, 0x4c, 0x1c, 0x57,
  0xb6, 0x0e, 0x4b, 0xda, 0x47, 0x22, 0x6d, 0x32,
  0x91, 0x50, 0x98, 0x1c, 0xaf, 0xb4, 0x1e, 0x47,
  0xba, 0x6e, 0x62, 0x9a, 0xb0, 0xa0, 0x5c, 0x3e,
  0x37, 0x7b, 0x4c, 0xe4, 0x55, 0xa6, 0x02, 0x2b,
  0xf3, 0x07, 0xd5, 0xef, 0x03, 0x9d, 0xf6, 0xb3,
  0xc8, 0x57, 0x4e, 0x0c, 0x5b, 0xd6, 0x27, 0x0b,
  0x2d, 0xc5, 0x13, 0x61, 0x94, 0xba, 0x84, 0x60,
  0xe6, 0xbd, 0xa8, 0x72, 0x0e, 0xd3, 0xd9, 0x17,
  0x29, 0x8d, 0x0a, 0xd1, 0xc1, 0x1b, 0x79, 0xa4,
  0xea, 0x25, 0x83, 0x22, 0xf5, 0x31, 0xc1, 0x5b,
  0x78, 0x24, 0xef, 0x25, 0x9d, 0x22, 0xb1, 0x30,
  0x59, 0x5e, 0x28, 0x3b, 0x0f, 0x65, 0xdc, 0xa3,
  0x34, 0x35, 0x47, 0x40, 0x6c, 0x7e, 0x96, 0xf8,
  0x89, 0xec, 0xcb, 0x95, 0x46, 0x80, 0x68, 0xfe,
  0x8d, 0xf8, 0xd3, 0xed, 0x17, 0x91, 0x8e, 0x9a,
  0xd8, 0xa1, 0x2c, 0x39, 0x17, 0x69, 0x8c, 0x8a,
  0xd4, 0xc1, 0x05, 0x79, 0xe0, 0xeb, 0xbd, 0x86,
  0x72, 0xea, 0xd1, 0x81, 0x1a, 0xf9, 0xa1, 0xea,
  0x3b, 0x83, 0x66, 0xf4, 0xa9, 0xc4, 0x0b, 0x67,
  0xc4, 0xaf, 0x64, 0x1c, 0xa7, 0xb4, 0x2e, 0x47,
  0x1a, 0x6d, 0xa2, 0x92, 0x30, 0x93, 0x5c, 0x94,
  0x34, 0x87, 0x44, 0xec, 0x65, 0x96, 0xa2, 0x88,
  0x30, 0xcf, 0x5d, 0x5c, 0x30, 0x37, 0x5f, 0x4c,
  0x3c, 0x57, 0x76, 0x0c, 0xcb, 0xd5, 0x47, 0x00,
  0x6d, 0xfe, 0x93, 0xf8, 0x97, 0xec, 0x8f, 0x94,
  0xde, 0x85, 0x38, 0xe1, 0x6d, 0xb8, 0x92, 0x6c,
  0x92, 0x94, 0x90, 0x84, 0x9c, 0xe4, 0xb5, 0xa4,
  0x42, 0x26, 0x73, 0x2a, 0xd5, 0x01, 0x01, 0xf9,
  0xfb, 0xeb, 0xe7, 0x87, 0xae, 0xee, 0x19, 0x9b,
  0xaa, 0xa6, 0x00, 0x2b, 0xff, 0x07, 0xfd, 0xef,
  0xf3, 0x9f, 0xd6, 0xbf, 0x08, 0x7d, 0xce, 0xf3,
  0x59, 0xd4, 0x2b, 0x07, 0x05, 0xed, 0xe3, 0x93,
  0xb6, 0x96, 0x48, 0x8a, 0x4c, 0xc2, 0x55, 0x72,
  0x00, 0xd3, 0xfd, 0x17, 0xf1, 0x8f, 0xda, 0xdf,
  0x21, 0x3d, 0x39, 0x71, 0x68, 0xd8, 0x8d, 0x2c,
  0xd1, 0x15, 0x19, 0x81, 0xaa, 0xfa, 0x01, 0xe3,
  0xfb, 0xb7, 0xe6, 0x4f, 0xaa, 0x5e, 0x02, 0x3b,
  0xf3, 0x67, 0xd4, 0xaf, 0x04, 0x1d, 0xe7, 0xb3,
  0xae, 0x56, 0x1a, 0x0b, 0xa3, 0xc6, 0x37, 0x6b,
  0x4c, 0x84, 0x54, 0xe6, 0x05, 0xab, 0xe2, 0x07,
  0xb3, 0xee, 0x57, 0x9a, 0x0e, 0xa3, 0xd8, 0x37,
  0x2f, 0x4d, 0x1c, 0x51, 0xb6, 0x1a, 0x4b, 0xa2,
  0x46, 0x32, 0x6b, 0x52, 0x84, 0x10, 0xe7, 0x9d,
  0xae, 0xb2, 0x18, 0x53, 0xae, 0x16, 0x1b, 0x8b,
  0xa6, 0xc6, 0x29, 0x6b, 0x08, 0x85, 0xcc, 0xe3,
  0x55, 0xb4, 0x02, 0x47, 0xf2, 0x6f, 0xd2, 0x9f,
  0x10, 0xbd, 0x9c, 0x72, 0xb6, 0xd0, 0x49, 0x1e,
  0x49, 0xba, 0x4a, 0x62, 0x42, 0xb2, 0x70, 0x52,
  0xde, 0x11, 0x3b, 0x99, 0x66, 0xa8, 0xa8, 0x0c,
  0x0f, 0xd7, 0xdf, 0x0f, 0x3d, 0xdd, 0x73, 0x30,
  0xd5, 0x5d, 0x00, 0x31, 0xff, 0x5b, 0xfc, 0x27,
  0xf7, 0x2f, 0xcd, 0x1f, 0x51, 0xbc, 0x1a, 0x77,
  0xa2, 0xce, 0x31, 0x5b, 0x58, 0x24, 0x2f, 0x27,
  0x1d, 0x2d, 0xb1, 0x12, 0x59, 0x92, 0x2a, 0x93,
  0x00, 0x95, 0xfc, 0x83, 0xf4, 0xf7, 0xc5, 0xcf,
  0x63, 0x5c, 0xb4, 0x34, 0x47, 0x46, 0x6c, 0x6a,
  0x96, 0x80, 0x88, 0xfc, 0xcd, 0xf5, 0x53, 0xc0,
  0x17, 0x7f, 0x8c, 0xfe, 0xd5, 0xf9, 0x03, 0xe9,
  0xf7, 0x8b, 0xce, 0xc7, 0x59, 0x6c, 0x28, 0x97,
  0x0c, 0x8d, 0xd4, 0xd3, 0x05, 0x15, 0xe1, 0x83,
  0xba, 0xf6, 0x61, 0xca, 0xbb, 0x40, 0x64, 0x7e,
  0xa6, 0xf8, 0x29, 0xef, 0x0b, 0x9d, 0xc6, 0xb3,
  0x68, 0x54, 0x8e, 0x04, 0xdb, 0xe5, 0x27, 0xa1,
  0x2e, 0x39, 0x1b, 0x69, 0xa4, 0x8a, 0x24, 0xc3,
  0x25, 0x75, 0x20, 0xc1, 0x3d, 0x79, 0x70, 0xe8,
  0xdd, 0x8d, 0x32, 0xd1, 0x51, 0x18, 0x19, 0xaf,
  0xaa, 0x1e, 0x03, 0xbb, 0xf6, 0x67, 0xca, 0xaf,
  0x40, 0x1c, 0x7f, 0xb6, 0xfe, 0x49, 0xfa, 0x4b,
  0xe2, 0x47, 0xb2, 0x6e, 0x52, 0x9a, 0x10, 0xa3,
  0x9c, 0x36, 0xb7, 0x48, 0x4c, 0x4e, 0x56, 0x5a,
  0x0a, 0x23, 0xc3, 0x37, 0x75, 0x4c, 0xc0, 0x55,
  0x7e, 0x00, 0xfb, 0xfd, 0xe7, 0xf3, 0xaf, 0xd6,
  0x1f, 0x0b, 0xbd, 0xc6, 0x73, 0x6a, 0xd4, 0x81,
  0x04, 0xf9, 0xe5, 0xeb, 0xa3, 0x86, 0x36, 0xeb,
  0x49, 0x84, 0x4a, 0xe6, 0x41, 0xaa, 0x7a, 0x02,
  0xe3, 0xf1, 0xb7, 0xda, 0x4f, 0x22, 0x5d, 0x32,
  0x31, 0x53, 0x58, 0x14, 0x2f, 0x87, 0x1e, 0xed,
  0xb9, 0x92, 0x6a, 0x92, 0x80, 0x90, 0xfc, 0x9d,
  0xf4, 0xb3, 0xc4, 0x57, 0x66, 0x0c, 0xab, 0xd4,
  0x07, 0x07, 0xed, 0xef, 0x93, 0x9e, 0x96, 0xb8,
  0x88, 0x6c, 0xce, 0x95, 0x58, 0x80, 0x2c, 0xff,
  0x15, 0xfd, 0x83, 0xf2, 0xf7, 0xd1, 0xcf, 0x1b,
  0x5d, 0xa4, 0x32, 0x27, 0x53, 0x2c, 0x15, 0x17,
  0x81, 0x8e, 0xfa, 0xd9, 0xe1, 0x2b, 0xb9, 0x06,
  0x69, 0xea, 0x8b, 0x80, 0xc6, 0xfd, 0x69, 0xf0,
  0x8b, 0xdc, 0xc7, 0x35, 0x6d, 0x40, 0x90, 0x7c,
  0x9e, 0xf4, 0xb9, 0xc4, 0x6b, 0x66, 0x84, 0xa8,
  0xe4, 0x0d, 0xa7, 0xd2, 0x2f, 0x13, 0x1d, 0x95,
  0xb2, 0x82, 0x50, 0xf2, 0x1d, 0xd3, 0xb3, 0x16,
  0x55, 0x8a, 0x02, 0xc3, 0xf1, 0x77, 0xd8, 0xcf,
  0x2d, 0x5d, 0x10, 0x31, 0x9f, 0x5a, 0xbc, 0x20,
  0x77, 0x3e, 0xcd, 0x79, 0x50, 0xe8, 0x1d, 0x8f,
  0xb2, 0xde, 0x51, 0x3a, 0x19, 0x63, 0xa8, 0xb6,
  0x0c, 0x4b, 0xd6, 0x47, 0x0a, 0x6d, 0xc2, 0x93,
  0x70, 0x94, 0xdc, 0x85, 0x34, 0xe1, 0x45, 0xb8,
  0x62, 0x6e, 0xb2, 0x98, 0x50, 0xae, 0x1c, 0x1b,
  0xb7, 0xa6, 0x4e, 0x2a, 0x5b, 0x02, 0x25, 0xf3,
  0x23, 0xd5, 0x37, 0x01, 0x4d, 0xf8, 0x53, 0xee,
  0x17, 0x9b, 0x8e, 0xa6, 0xd8, 0x29, 0x2f, 0x09,
  0x1d, 0xc9, 0xb3, 0x4a, 0x54, 0x42, 0x06, 0x73,
  0xea, 0xd7, 0x81, 0x0e, 0xf9, 0xd9, 0xeb, 0x2b,
  0x85, 0x06, 0xe1, 0xe9, 0xbb, 0x8a, 0x66, 0xc2,
  0xa9, 0x70, 0x08, 0xdf, 0xcd, 0x3f, 0x51, 0x7c,
  0x18, 0xf7, 0xad, 0xce, 0x13, 0x5b, 0x94, 0x26,
  0x87, 0x28, 0xed, 0x0d, 0x91, 0xd2, 0x9b, 0x10,
  0xa5, 0x9c, 0x22, 0xb7, 0x30, 0x4d, 0x5e, 0x50,
  0x3a, 0x1f, 0x63, 0xbc, 0xb6, 0x74, 0x4a, 0xc6,
  0x41, 0x6a, 0x78, 0x82, 0xec, 0xf1, 0x95, 0xda,
  0x83, 0x20, 0xf5, 0x3d, 0xc1, 0x73, 0x78, 0xd4,
  0xed, 0x05, 0x91, 0xe2, 0x9b, 0xb0, 0xa6, 0x5c,
  0x2a, 0x37, 0x03, 0x4d, 0xf4, 0x53, 0xc6, 0x17,
  0x6b, 0x8c, 0x86, 0xd4, 0xe9, 0x05, 0x89, 0xe2,
  0xcb, 0xb1, 0x46, 0x58, 0x6a, 0x2e, 0x83, 0x18,
  0xf5, 0xad, 0xc2, 0x13, 0x73, 0x94, 0xd6, 0x85,
  0x08, 0xe1, 0xcd, 0xbb, 0x52, 0x64, 0x12, 0xa7,
  0x90, 0x2e, 0x9f, 0x18, 0xbd, 0xac, 0x72, 0x16,
  0xd3, 0x89, 0x16, 0xc9, 0x89, 0x4a, 0xc8, 0x41,
  0x4e, 0x78, 0x5a, 0xee, 0x21, 0x9b, 0x3a, 0xa5,
  0x60, 0x20, 0xbf, 0x3c, 0x7d, 0x76, 0xf0, 0xc9,
  0xdd, 0x4b, 0x30, 0x45, 0x5e, 0x60, 0x3a, 0xbf,
  0x60, 0x7c, 0xbe, 0xf4, 0x79, 0xc6, 0xeb, 0x69,
  0x84, 0x8a, 0xe4, 0xc1, 0xa5, 0x7a, 0x20, 0xe3,
  0x3d, 0xb5, 0x72, 0x40, 0xd2, 0x7d, 0x12, 0xf1,
  0x91, 0xda, 0x9b, 0x20, 0xa5, 0x3c, 0x21, 0x77,
  0x38, 0xcd, 0x6d, 0x50, 0x90, 0x1c, 0x9f, 0xb4,
  0xbe, 0x44, 0x7a, 0x66, 0xe2, 0xa9, 0xb0, 0x0a,
  0x5f, 0xc2, 0x3f, 0x73, 0x7c, 0xd4, 0xf5, 0x05,
  0xc1, 0xe3, 0x7b, 0xb4, 0xe6, 0x45, 0xaa, 0x62,
  0x02, 0xb3, 0xf0, 0x57, 0xde, 0x0f, 0x3b, 0xdd,
  0x67, 0x30, 0xad, 0x5c, 0x10, 0x37, 0x9f, 0x4e,
  0xbc, 0x58, 0x76, 0x2e, 0xcb, 0x19, 0x45, 0xa8,
  0x62, 0x0e, 0xb3, 0xd8, 0x57, 0x2e, 0x0d, 0x1b,
  0xd1, 0xa7, 0x1a, 0x2d, 0xa3, 0x12, 0x35, 0x93,
  0x42, 0x94, 0x70, 0x86, 0xdc, 0xe9, 0x35, 0x89,
  0x42, 0xc8, 0x71, 0x4e, 0xd8, 0x59, 0x2e, 0x29,
  0x1b, 0x09, 0xa5, 0xca, 0x23, 0x43, 0x34, 0x75,
  0x46, 0xc0, 0x69, 0x7e, 0x88, 0xf8, 0xcd, 0xed,
  0x53, 0x90, 0x16, 0x9f, 0x88, 0xbe, 0xcc, 0x79,
  0x56, 0xe8, 0x09, 0x8f, 0xca, 0xdf, 0x41, 0x3c,
  0x79, 0x76, 0xe8, 0xc9, 0x8d, 0x4a, 0xd0, 0x41,
  0x1e, 0x79, 0xba, 0xea, 0x61, 0x82, 0xba, 0xf0,
  0x61, 0xde, 0xbb, 0x38, 0x65, 0x6e, 0xa0, 0x98,
  0x3c, 0xaf, 0x74, 0x1c, 0xc7, 0xb5, 0x6e, 0x40,
  0x9a, 0x7c, 0xa2, 0xf4, 0x31, 0xc7, 0x5b, 0x6c,
  0x24, 0x97, 0x24, 0x8d, 0x24, 0xd1, 0x25, 0x19,
  0x21, 0xa9, 0x3a, 0x09, 0x63, 0xc8, 0xb7, 0x4c,
  0x4c, 0x56, 0x56, 0x0a, 0x0b, 0xc3, 0xc7, 0x77,
  0x6c, 0xcc, 0x95, 0x54, 0x80, 0x04, 0xff, 0xe5,
  0xff, 0xa3, 0xfe, 0x37, 0xfb, 0x4f, 0xe4, 0x5f,
  0xa6, 0x3e, 0x2b, 0x7b, 0x04, 0xe5, 0xe5, 0xa3,
  0xa2, 0x36, 0x33, 0x4b, 0x54, 0x44, 0x06, 0x67,
  0xea, 0xaf, 0x80, 0x1e, 0xff, 0xb9, 0xfe, 0x6b,
  0xfa, 0x87, 0xe0, 0xef, 0xbd, 0x9e, 0x72, 0xba,
  0xd0, 0x61, 0x1e, 0xb9, 0xb8, 0x6a, 0x6e, 0x82,
  0x98, 0xf0, 0xad, 0xdc, 0x13, 0x37, 0x95, 0x4e,
  0x80, 0x58, 0xfe, 0x2d, 0xfb, 0x13, 0xe5, 0x97,
  0xa2, 0x8e, 0x30, 0xdb, 0x5d, 0x24, 0x31, 0x27,
  0x59, 0x2c, 0x29, 0x17, 0x09, 0x8d, 0xca, 0xd3,
  0x41, 0x14, 0x79, 0x86, 0xea, 0xe9, 0x81, 0x8a,
  0xfa, 0xc1, 0xe1, 0x7b, 0xb8, 0xe6, 0x6d, 0xaa,
  0x92, 0x00, 0x93, 0xfc, 0x97, 0xf4, 0x8f, 0xc4,
  0xdf, 0x65, 0x3c, 0xa1, 0x74, 0x38, 0xc7, 0x6d,
  0x6c, 0x90, 0x94, 0x9c, 0x84, 0xb4, 0xe4, 0x45,
  0xa6, 0x62, 0x2a, 0xb3, 0x00, 0x55, 0xfe, 0x03,
  0xfb, 0xf7, 0xe7, 0xcf, 0xaf, 0x5e, 0x1c, 0x3b,
  0xb7, 0x66, 0x4c, 0xaa, 0x54, 0x02, 0x07, 0xf3,
  0xef, 0xd7, 0x9f, 0x0e, 0xbd, 0xd8, 0x73, 0x2e,
  0xd5, 0x19, 0x01, 0xa9, 0xfa, 0x0b, 0xe3, 0xc7,
  0xb7, 0x6e, 0x4c, 0x9a, 0x54, 0xa2, 0x04, 0x33,
  0xe7, 0x57, 0xac, 0x0e, 0x17, 0xdb, 0x8f, 0x26,
  0xdd, 0x29, 0x31, 0x09, 0x59, 0xc8, 0x2b, 0x4f,
  0x04, 0x5d, 0xe6, 0x33, 0xab, 0x56, 0x04, 0x0b,
  0xe7, 0xc7, 0xaf, 0x6e, 0x1c, 0x9b, 0xb4, 0xa6,
  0x44, 0x2a, 0x67, 0x02, 0xad, 0xf0, 0x13, 0xdf,
  0x97, 0x3e, 0x8d, 0x78, 0xd0, 0xed, 0x1d, 0x91,
  0xb2, 0x9a, 0x50, 0xa2, 0x1c, 0x33, 0xb7, 0x56,
  0x4c, 0x0a, 0x57, 0xc2, 0x0f, 0x73, 0xdc, 0xd7,
  0x35, 0x0d, 0x41, 0xd0, 0x7b, 0x1e, 0xe5, 0xb9,
  0xa2, 0x6a, 0x32, 0x83, 0x50, 0xf4, 0x1d, 0xc7,
  0xb3, 0x6e, 0x54, 0x9a, 0x04, 0xa3, 0xe4, 0x37,
  0xa7, 0x4e, 0x2c, 0x5b, 0x16, 0x25, 0x8b, 0x22,
  0xc5, 0x31, 0x61, 0x58, 0xb8, 0x2c, 0x6f, 0x16,
  0x9d, 0x88, 0xb2, 0xcc, 0x51, 0x56, 0x18, 0x0b,
  0xaf, 0xc6, 0x1f, 0x6b, 0xbc, 0x86, 0x74, 0xea,
  0xc5, 0x81, 0x62, 0xf8, 0xb1, 0xec, 0x5b, 0x96,
  0x26, 0x8b, 0x28, 0xc5, 0x0d, 0x61, 0xd0, 0xbb,
  0x1c, 0x65, 0xb6, 0xa2, 0x48, 0x32, 0x4f, 0x52,
  0x5c, 0x12, 0x37, 0x93, 0x4e, 0x94, 0x58, 0x86,
  0x2c, 0xeb, 0x15, 0x85, 0x82, 0xe2, 0xf1, 0xb1,
  0xda, 0x5b, 0x22, 0x25, 0x33, 0x21, 0x55, 0x38,
  0x01, 0x6f, 0xf8, 0x9f, 0xec, 0xbf, 0x94, 0x7e,
  0x86, 0xf8, 0xe9, 0xed, 0x8b, 0x92, 0xc6, 0x91,
  0x68, 0x98, 0x8c, 0xac, 0xd4, 0x15, 0x07, 0x81,
  0xee, 0xfb, 0x99, 0xe6, 0xab, 0xa8, 0x06, 0x0f,
  0xeb, 0xdf, 0x87, 0x3e, 0xed, 0x79, 0x90, 0xea,
  0x9d, 0x80, 0xb2, 0xfc, 0x51, 0xf6, 0x1b, 0xcb,
  0xa7, 0x46, 0x2c, 0x6b, 0x16, 0x85, 0x88, 0xe2,
  0xcd, 0xb1, 0x52, 0x58, 0x12, 0x2f, 0x93, 0x1e,
  0x95, 0xb8, 0x82, 0x6c, 0xf2, 0x95, 0xd0, 0x83,
  0x1c, 0xf5, 0xb5, 0xc2, 0x43, 0x72, 0x74, 0xd2,
  0xc5, 0x11, 0x61, 0x98, 0xba, 0xac, 0x60, 0x16,
  0xbf, 0x88, 0x7e, 0xce, 0xf9, 0x59, 0xe8, 0x2b,
  0x8f, 0x06, 0xdd, 0xe9, 0x33, 0x89, 0x56, 0xc8,
  0x09, 0x4f, 0xc8, 0x5f, 0x4e, 0x3c, 0x5b, 0x76,
  0x24, 0xcb, 0x25, 0x45, 0x20, 0x61, 0x3e, 0xb9,
  0x78, 0x68, 0xee, 0x8d, 0x98, 0xd2, 0xad, 0x10,
  0x11, 0x9f, 0x9a, 0xbe, 0xa0, 0x78, 0x3e, 0xef,
  0x79, 0x9c, 0xea, 0xb5, 0x80, 0x42, 0xfe, 0x71,
  0xfa, 0xdb, 0xe1, 0x27, 0xb9, 0x2e, 0x69, 0x1a,
  0x89, 0xa0, 0xca, 0x3d, 0x43, 0x70, 0x74, 0xde,
  0xc5, 0x39, 0x61, 0x68, 0xb8, 0x8c, 0x6c, 0xd6,
  0x95, 0x08, 0x81, 0xcc, 0xfb, 0x55, 0xe4, 0x03,
  0xa7, 0xf6, 0x2f, 0xcb, 0x1f, 0x45, 0xbc, 0x62,
  0x76, 0xb2, 0xc8, 0x51, 0x4e, 0x18, 0x5b, 0xae,
  0x26, 0x1b, 0x2b, 0xa5, 0x06, 0x21, 0xeb, 0x3b,
  0x85, 0x66, 0xe0, 0xa9, 0xbc, 0x0a, 0x77, 0xc2,
  0xcf, 0x71, 0x5c, 0xd8, 0x35, 0x2f, 0x41, 0x1c,
  0x79, 0xb6, 0xea, 0x49, 0x82, 0x4a, 0xf2, 0x41,
  0xd2, 0x7b, 0x12, 0xe5, 0x91, 0xa2, 0x9a, 0x30,
  0xa3, 0x5c, 0x34, 0x37, 0x47, 0x4c, 0x6c, 0x56,
  0x96, 0x08, 0x8b, 0xcc, 0xc7, 0x55, 0x6c, 0x00,
  0x97, 0xfc, 0x8f, 0xf4, 0xdf, 0xc5, 0x3f, 0x61,
  0x7c, 0xb8, 0xf4, 0x6d, 0xc6, 0x93, 0x68, 0x94,
  0x8c, 0x84, 0xd4, 0xe5, 0x05, 0xa1, 0xe2, 0x3b,
  0xb3, 0x66, 0x54, 0xaa, 0x04, 0x03, 0xe7, 0xf7,
  0xaf, 0xce, 0x1f, 0x5b, 0xbc, 0x26, 0x77, 0x2a,
  0xcd, 0x01, 0x51, 0xf8, 0x1b, 0xef, 0xa7, 0x9e,
  0x2e, 0xbb, 0x18, 0x65, 0xae, 0xa2, 0x18, 0x33,
  0xaf, 0x56, 0x1c, 0x0b, 0xb7, 0xc6, 0x4f, 0x6a,
  0x5c, 0x82, 0x34, 0xf3, 0x45, 0xd4, 0x63, 0x06,
  0xb5, 0xe8, 0x43, 0x8e, 0x76, 0xda, 0xc9, 0x21,
  0x49, 0x38, 0x49, 0x6e, 0x48, 0x9a, 0x4c, 0xa2,
  0x54, 0x32, 0x07, 0x53, 0xec, 0x17, 0x97, 0x8e,
  0x8e, 0xd8, 0xd9, 0x2d, 0x29, 0x11, 0x09, 0x99,
  0xca, 0xab, 0x40, 0x04, 0x7f, 0xe6, 0xff, 0xa9,
  0xfe, 0x0b, 0xfb, 0xc7, 0xe7, 0x6f, 0xac, 0x9e,
  0x14, 0xbb, 0x84, 0x66, 0xe6, 0xa9, 0xa8, 0x0a,
  0x0f, 0xc3, 0xdf, 0x77, 0x3c, 0xcd, 0x75, 0x50,
  0xc0, 0x1d, 0x7f, 0xb0, 0xfe, 0x5d, 0xfa, 0x33,
  0xe3, 0x57, 0xb4, 0x0e, 0x47, 0xda, 0x6f, 0x22,
  0x9d, 0x30, 0xb1, 0x5c, 0x58, 0x36, 0x2f, 0x4b,
  0x1c, 0x45, 0xb6, 0x62, 0x4a, 0xb2, 0x40, 0x52,
  0x7e, 0x12, 0xfb, 0x91, 0xe6, 0x9b, 0xa8, 0xa6,
  0x0c, 0x2b, 0xd7, 0x07, 0x0d, 0xed, 0xd3, 0x93,
  0x16, 0x95, 0x88, 0x82, 0xcc, 0xf1, 0x55, 0xd8,
  0x03, 0x2f, 0xf5, 0x1f, 0xc1, 0xbf, 0x7a, 0x7c,
  0xe2, 0xf5, 0xb1, 0xc2, 0x5b, 0x72, 0x24, 0xd3,
  0x25, 0x15, 0x21, 0x81, 0x3a, 0xf9, 0x61, 0xe8,
  0xbb, 0x8c, 0x66, 0xd6, 0xa9, 0x08, 0x09, 0xcf,
  0xcb, 0x5f, 0x44, 0x3c, 0x67, 0x76, 0xac, 0xc8,
  0x15, 0x4f, 0x80, 0x5e, 0xfe, 0x39, 0xfb, 0x6b,
  0xe4, 0x87, 0xa4, 0xee, 0x25, 0x9b, 0x22, 0xa5,
  0x30, 0x21, 0x5f, 0x38, 0x3d, 0x6f, 0x70, 0x9c,
  0xdc, 0xb5, 0x34, 0x41, 0x46, 0x78, 0x6a, 0xee,
  0x81, 0x98, 0xfa, 0xad, 0xe0, 0x13, 0xbf, 0x96,
  0x7e, 0x8a, 0xf8, 0xc1, 0xed, 0x7b, 0x90, 0xe6,
  0x9d, 0xa8, 0xb2, 0x0c, 0x53, 0xd6, 0x17, 0x0b,
  0x8d, 0xc6, 0xd3, 0x69, 0x14, 0x89, 0x84, 0xca,
  0xe5, 0x41, 0xa0, 0x7a, 0x3e, 0xe3, 0x79, 0xb4,
  0xea, 0x45, 0x82, 0x62, 0xf2, 0xb1, 0xd0, 0x5b,
  0x1e, 0x25, 0xbb, 0x22, 0x65, 0x32, 0xa1, 0x50,
  0x38, 0x1f, 0x6f, 0xbc, 0x9e, 0x74, 0xba, 0xc4,
  0x61, 0x66, 0xb8, 0xa8, 0x6c, 0x0e, 0x97, 0xd8,
  0x8f, 0x2c, 0xdd, 0x15, 0x31, 0x81, 0x5a, 0xf8,
  0x21, 0xef, 0x3b, 0x9d, 0x66, 0xb0, 0xa8, 0x5c,
  0x0e, 0x37, 0xdb, 0x4f, 0x24, 0x5d, 0x26, 0x31,
  0x2b, 0x59, 0x04, 0x29, 0xe7, 0x0b, 0xad, 0xc6,
  0x13, 0x6b, 0x94, 0x86, 0x84, 0xe8, 0xe5, 0x8d,
  0xa2, 0xd2, 0x31, 0x13, 0x59, 0x94, 0x2a, 0x87,
  0x00, 0xed, 0xfd, 0x93, 0xf2, 0x97, 0xd0, 0x8f,
  0x1c, 0xdd, 0xb5, 0x32, 0x41, 0x52, 0x78, 0x12,
  0xef, 0x91, 0x9e, 0x9a, 0xb8, 0xa0, 0x6c, 0x3e,
  0x97, 0x78, 0x8c, 0xec, 0xd5, 0x95, 0x02, 0x81,
  0xf0, 0xfb, 0xdd, 0xe7, 0x33, 0xad, 0x56, 0x10,
  0x0b, 0x9f, 0xc6, 0xbf, 0x68, 0x7c, 0x8e, 0xf4,
  0xd9, 0xc5, 0x2b, 0x61, 0x04, 0xb9, 0xe4, 0x6b,
  0xa6, 0x86, 0x28, 0xeb, 0x0d, 0x85, 0xd2, 0xe3,
  0x11, 0xb5, 0x9a, 0x42, 0xa2, 0x70, 0x32, 0xdf,
  0x51, 0x3c, 0x19, 0x77, 0xa8, 0xce, 0x0d, 0x5b,
  0xd0, 0x27, 0x1f, 0x2d, 0xbd, 0x12, 0x71, 0x92,
  0xda, 0x91, 0x20, 0x99, 0x3c, 0xa9, 0x74, 0x08,
  0xc7, 0xcd, 0x6f, 0x50, 0x9c, 0x1c, 0xb7, 0xb4,
  0x4e, 0x46, 0x5a, 0x6a, 0x22, 0x83, 0x30, 0xf5,
  0x5d, 0xc0, 0x33, 0x7f, 0x54, 0xfc, 0x05, 0xf7,
  0xe3, 0xcf, 0xb7, 0x5e, 0x4c, 0x3a, 0x57, 0x62,
  0x0c, 0xb3, 0xd4, 0x57, 0x06, 0x0d, 0xeb, 0xd3,
  0x87, 0x16, 0xed, 0x89, 0x92, 0xca, 0x91, 0x40,
  0x98, 0x7c, 0xae, 0xf4, 0x19, 0xc7, 0xab, 0x6e,
  0x04, 0x9b, 0xe4, 0xa7, 0xa4, 0x2e, 0x27, 0x1b,
  0x2d, 0xa5, 0x12, 0x21, 0x93, 0x3a, 0x95, 0x60,
  0x80, 0xbc, 0xfc, 0x75, 0xf6, 0xc3, 0xc9, 0x77,
  0x48, 0xcc, 0x4d, 0x56, 0x50, 0x0a, 0x1f, 0xc3,
  0xbf, 0x76, 0x7c, 0xca, 0xf5, 0x41, 0xc0, 0x7b,
  0x7e, 0xe4, 0xf9, 0xa5, 0xea, 0x23, 0x83, 0x36,
  0xf5, 0x49, 0xc0, 0x4b, 0x7e, 0x44, 0xfa, 0x65,
  0xe2, 0xa3, 0xb0, 0x36, 0x5f, 0x4a, 0x3c, 0x43,
  0x76, 0x74, 0xca, 0xc5, 0x41, 0x60, 0x78, 0xbe,
  0xec, 0x79, 0x96, 0xea, 0x89, 0x80, 0xca, 0xfd,
  0x41, 0xf0, 0x7b, 0xde, 0xe7, 0x39, 0xad, 0x6a,
  0x10, 0x83, 0x9c, 0xf6, 0xb5, 0xc8, 0x43, 0x4e,
  0x74, 0x5a, 0xc6, 0x21, 0x6b, 0x38, 0x85, 0x6c,
  0xe0, 0x95, 0xbc, 0x82, 0x74, 0xf2, 0xc5, 0xd1,
  0x63, 0x18, 0xb5, 0xac, 0x42, 0x16, 0x73, 0x8a,
  0xd6, 0xc1, 0x09, 0x79, 0xc8, 0xeb, 0x4d, 0x84,
  0x52, 0xe6, 0x11, 0xab, 0x9a, 0x06, 0xa3, 0xe8,
  0x37, 0x8f, 0x4e, 0xdc, 0x59, 0x36, 0x29, 0x4b,
  0x08, 0x45, 0xce, 0x63, 0x5a, 0xb4, 0x20, 0x47,
  0x3e, 0x6d, 0x7a, 0x90, 0xe0, 0x9d, 0xbc, 0xb2,
  0x74, 0x52, 0xc6, 0x11, 0x6b, 0x98, 0x86, 0xac,
  0xe8, 0x15, 0x8f, 0x82, 0xde, 0xf1, 0x39, 0xd9,
  0x6b, 0x28, 0x85, 0x0c, 0xe1, 0xd5, 0xbb, 0x02,
  0x65, 0xf2, 0xa3, 0xd0, 0x37, 0x1f, 0x4d, 0xbc,
  0x52, 0x76, 0x12, 0xcb, 0x91, 0x46, 0x98, 0x68,
  0xae, 0x8c, 0x18, 0xd7, 0xad, 0x0e, 0x11, 0xdb,
  0x9b, 0x26, 0xa5, 0x28, 0x21, 0x0f, 0x39, 0xdd,
  0x6b, 0x30, 0x85, 0x5c, 0xe0, 0x35, 0xbf, 0x42,
  0x7c, 0x72, 0xf6, 0xd1, 0xc9, 0x1b, 0x49, 0xa4,
  0x4a, 0x26, 0x43, 0x2a, 0x75, 0x02, 0xc1, 0xf1,
  0x7b, 0xd8, 0xe7, 0x2d, 0xad, 0x12, 0x11, 0x93,
  0x9a, 0x96, 0xa0, 0x88, 0x3c, 0xcf, 0x75, 0x5c,
  0xc0, 0x35, 0x7f, 0x40, 0xfc, 0x7d, 0xf6, 0xf3,
  0xc9, 0xd7, 0x4b, 0x0c, 0x45, 0xd6, 0x63, 0x0a,
  0xb5, 0xc0, 0x43, 0x7e, 0x74, 0xfa, 0xc5, 0xe1,
  0x63, 0xb8, 0xb6, 0x6c, 0x4a, 0x96, 0x40, 0x8a,
  0x7c, 0xc2, 0xf5, 0x71, 0xc0, 0xdb, 0x7d, 0x24,
  0xf1, 0x25, 0xd9, 0x23, 0x29, 0x35, 0x09, 0x41,
  0xc8, 0x7b, 0x4e, 0xe4, 0x59, 0xa6, 0x2a, 0x2b,
  0x03, 0x05, 0xf5, 0xe3, 0xc3, 0xb7, 0x76, 0x4c,
  0xca, 0x55, 0x42, 0x00, 0x73, 0xfe, 0xd7, 0xf9,
  0x0f, 0xe9, 0xdf, 0x8b, 0x3e, 0xc5, 0x79, 0x60,
  0xe8, 0xbd, 0x8c, 0x72, 0xd6, 0xd1, 0x09, 0x19,
  0xc9, 0xab, 0x4a, 0x04, 0x43, 0xe6, 0x77, 0xaa,
  0xce, 0x01, 0x5b, 0xf8, 0x27, 0xef, 0x2f, 0x9d,
  0x1e, 0xb1, 0xb8, 0x5a, 0x6e, 0x22, 0x9b, 0x30,
  0xa5, 0x5c, 0x20, 0x37, 0x3f, 0x4d, 0x7c, 0x50,
  0xf6, 0x1d, 0xcb, 0xb3, 0x46, 0x54, 0x6a, 0x06,
  0x83, 0xe8, 0xf7, 0x8d, 0xce, 0xd3, 0x59, 0x14,
  0x29, 0x87, 0x0a, 0xed, 0xc1, 0x93, 0x7a, 0x94,
  0xe0, 0x85, 0xbc, 0xe2, 0x75, 0xb2, 0xc2, 0x51,
  0x72, 0x18, 0xd3, 0xad, 0x16, 0x11, 0x8b, 0x9a,
  0xc6, 0xa1, 0x68, 0x38, 0x8f, 0x6c, 0xdc, 0x95,
  0x34, 0x81, 0x44, 0xf8, 0x65, 0xee, 0xa3, 0x98,
  0x36, 0xaf, 0x48, 0x1c, 0x4f, 0xb6, 0x5e, 0x4a,
  0x3a, 0x43, 0x62, 0x74, 0xb2, 0xc4, 0x51, 0x66,
  0x18, 0xab, 0xac, 0x06, 0x17, 0xeb, 0x8f, 0x86,
  0xde, 0xe9, 0x39, 0x89, 0x6a, 0xc8, 0x81, 0x4c,
  0xf8, 0x55, 0xee, 0x03, 0x9b, 0xf6, 0xa7, 0xc8,
  0x2f, 0x4f, 0x1c, 0x5d, 0xb6, 0x32, 0x4b, 0x52,
  0x44, 0x12, 0x67, 0x92, 0xae, 0x90, 0x18, 0x9f,
  0xac, 0xbe, 0x14, 0x7b, 0x86, 0xe6, 0xe9, 0xa9,
  0x8a, 0x0a, 0xc3, 0xc1, 0x77, 0x78, 0xcc, 0xed,
  0x55, 0x90, 0x02, 0x9f, 0xf0, 0xbf, 0xdc, 0x7f,
  0x36, 0xfd, 0x49, 0xf0, 0x4b, 0xde, 0x47, 0x3a,
  0x6d, 0x62, 0x90, 0xb0, 0x9c, 0x5c, 0xb6, 0x34,
  0x4b, 0x46, 0x44, 0x6a, 0x66, 0x82, 0xa8, 0xf0,
  0x0d, 0xdf, 0xd3, 0x3f, 0x15, 0x7d, 0x80, 0xf2,
  0xfd, 0xd1, 0xf3, 0x1b, 0xd5, 0xa7, 0x02, 0x2d,
  0xf3, 0x13, 0xd5, 0x97, 0x02, 0x8d, 0xf0, 0xd3,
  0xdd, 0x17, 0x31, 0x8d, 0x5a, 0xd0, 0x21, 0x1f,
  0x39, 0xbd, 0x6a, 0x70, 0x82, 0xdc, 0xf1, 0x35,
  0xd9, 0x43, 0x28, 0x75, 0x0e, 0xc1, 0xd9, 0x7b,
  0x28, 0xe5, 0x0d, 0xa1, 0xd2, 0x3b, 0x13, 0x65,
  0x94, 0xa2, 0x84, 0x30, 0xe7, 0x5d, 0xac, 0x32,
  0x17, 0x53, 0x8c, 0x16, 0xd7, 0x89, 0x0e, 0xc9,
  0xd9, 0x4b, 0x28, 0x45, 0x0e, 0x61, 0xda, 0xbb,
  0x20, 0x65, 0x3e, 0xa1, 0x78, 0x38, 0xef, 0x6d,
  0x9c, 0x92, 0xb4, 0x90, 0x44, 0x9e, 0x64, 0xba,
  0xa4, 0x60, 0x26, 0xbf, 0x28, 0x7d, 0x0e, 0xf1,
  0xd9, 0xdb, 0x2b, 0x25, 0x05, 0x21, 0xe1, 0x3b,
  0xb9, 0x66, 0x68, 0xaa, 0x8c, 0x00, 0xd7, 0xfd,
  0x0f, 0xf1, 0xdf, 0xdb, 0x3f, 0x25, 0x7d, 0x20,
  0xf1, 0x3d, 0xd9, 0x73, 0x28, 0xd5, 0x0d, 0x01,
  0xd1, 0xfb, 0x1b, 0xe5, 0xa7, 0xa2, 0x2e, 0x33,
  0x1b, 0x55, 0xa4, 0x02, 0x27, 0xf3, 0x2f, 0xd5,
  0x1f, 0x01, 0xbd, 0xfa, 0x73, 0xe2, 0xd7, 0xb1,
  0x0e, 0x59, 0xda, 0x2b, 0x23, 0x05, 0x35, 0xe1,
  0x43, 0xb8, 0x76, 0x6e, 0xca, 0x99, 0x40, 0xa8,
  0x7c, 0x0e, 0xf7, 0xd9, 0xcf, 0x2b, 0x5d, 0x04,
  0x31, 0xe7, 0x5b, 0xac, 0x26, 0x17, 0x2b, 0x8d,
  0x06, 0xd1, 0xe9, 0x1b, 0x89, 0xa6, 0xca, 0x29,
  0x43, 0x08, 0x75, 0xce, 0xc3, 0x59, 0x74, 0x28,
  0xc7, 0x0d, 0x6d, 0xd0, 0x93, 0x1c, 0x95, 0xb4,
  0x82, 0x44, 0xf2, 0x65, 0xd2, 0xa3, 0x10, 0x35,
  0x9f, 0x42, 0xbc, 0x70, 0x76, 0xde, 0xc9, 0x39,
  0x49, 0x68, 0x48, 0x8e, 0x4c, 0xda, 0x55, 0x22,
  0x01, 0x33, 0xf9, 0x57, 0xe8, 0x0f, 0x8f, 0xde,
  0xdf, 0x39, 0x3d, 0x69, 0x70, 0x88, 0xdc, 0xcd,
  0x35, 0x51, 0x40, 0x18, 0x7f, 0xae, 0xfe, 0x19,
  0xfb, 0xab, 0xe6, 0x07, 0xab, 0xee, 0x07, 0x9b,
  0xee, 0xa7, 0x98, 0x2e, 0xaf, 0x18, 0x1d, 0xaf,
  0xb2, 0x1e, 0x53, 0xba, 0x16, 0x63, 0x8a, 0xb6,
  0xc0, 0x49, 0x7e, 0x48, 0xfa, 0x4d, 0xe2, 0x53,
  0xb2, 0x16, 0x53, 0x8a, 0x16, 0xc3, 0x89, 0x76,
  0xc8, 0xc9, 0x4d, 0x48, 0x50, 0x4e, 0x1e, 0x5b,
  0xba, 0x26, 0x63, 0x2a, 0xb5, 0x00, 0x41, 0xfe,
  0x7b, 0xfa, 0xe7, 0xe1, 0xaf, 0xba, 0x1e, 0x63,
  0xba, 0xb6, 0x60, 0x4a, 0xbe, 0x40, 0x7a, 0x7e,
  0xe2, 0xf9, 0xb1, 0xea, 0x5b, 0x82, 0x26, 0xf3,
  0x29, 0xd5, 0x0b, 0x01, 0xc5, 0xfb, 0x63, 0xe4,
  0xb7, 0xa4, 0x4e, 0x26, 0x5b, 0x2a, 0x25, 0x03,
  0x21, 0xf5, 0x3b, 0xc1, 0x67, 0x78, 0xac, 0xec,
  0x15, 0x97, 0x82, 0x8e, 0xf0, 0xd9, 0xdd, 0x2b,
  0x31, 0x05, 0x59, 0xe0, 0x2b, 0xbf, 0x06, 0x7d,
  0xea, 0xf3, 0x81, 0xd6, 0xfb, 0x09, 0xe5, 0xcb,
  0xa3, 0x46, 0x34, 0x6b, 0x46, 0x84, 0x68, 0xe6,
  0x8d, 0xa8, 0xd2, 0x0d, 0x13, 0xd1, 0x97, 0x1a,
  0x8d, 0xa0, 0xd2, 0x3d, 0x13, 0x71, 0x94, 0xda,
  0x85, 0x20, 0xe1, 0x3d, 0xb9, 0x72, 0x68, 0xd2,
  0x8d, 0x10, 0xd1, 0x9d, 0x1a, 0xb1, 0xa0, 0x5a,
  0x3e, 0x23, 0x7b, 0x34, 0xe5, 0x45, 0xa0, 0x62,
  0x3e, 0xb3, 0x78, 0x54, 0xee, 0x05, 0x9b, 0xe2,
  0xa7, 0xb0, 0x2e, 0x5f, 0x1a, 0x3d, 0xa3, 0x72,
  0x34, 0xd3, 0x45, 0x14, 0x61, 0x86, 0xba, 0xe8,
  0x61, 0x8e, 0xba, 0xd8, 0x61, 0x2e, 0xb9, 0x18,
  0x69, 0xae, 0x8a, 0x18, 0xc3, 0xad, 0x76, 0x10,
  0xcb, 0x9d, 0x46, 0xb0, 0x68, 0x5e, 0x8e, 0x38,
  0xdb, 0x6d, 0x24, 0x91, 0x24, 0x99, 0x24, 0xa9,
  0x24, 0x09, 0x27, 0xc9, 0x2f, 0x49, 0x1c, 0x49,
  0xb6, 0x4a, 0x4a, 0x42, 0x42, 0x72, 0x72, 0xd2,
  0xd1, 0x11, 0x19, 0x99, 0xaa, 0xaa, 0x00, 0x03,
};


//--------------------------------------------------------------------------------
// SOUND.h
//--------------------------------------------------------------------------------

#pragma once

typedef struct
{
  int rate, cycles;
  byte wave[16];
  struct {
    int on;
    unsigned pos;
    int cnt, encnt, swcnt;
    int len, enlen, swlen;
    int swfreq;
    int freq;
    int envol, endir;
  } ch[4];
} gb_snd_t;

typedef struct
{
  int hz, len;
  int stereo;
  n16* buf;
  int pos;  
} gb_pcm_t;

extern gb_pcm_t pcm;
extern gb_snd_t snd;

void sound_write(byte r, byte b);
byte sound_read(byte r);
void sound_dirty();
void sound_reset(bool hard);
void sound_mix();


//--------------------------------------------------------------------------------
// SAVE.h
//--------------------------------------------------------------------------------

#pragma once

int sram_load(const char *file);
int sram_save(const char *file);
int sram_update(const char *file);

//--------------------------------------------------------------------------------
// CPU.c
//--------------------------------------------------------------------------------

// For cycle accurate emulation this needs to be 1
// Anything above 10 have diminishing returns
#define COUNTERS_TICK_PERIOD 8

static const byte cycles_table[256] =
{
  1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,
  1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
  3, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
  3, 3, 2, 2, 3, 3, 3, 1, 3, 2, 2, 2, 1, 1, 2, 1,

  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
  2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,

  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
  1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,

  5, 3, 4, 4, 6, 4, 2, 4, 5, 4, 4, 1, 6, 6, 2, 4,
  5, 3, 4, 0, 6, 4, 2, 4, 5, 4, 4, 0, 6, 0, 2, 4,
  3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,
  3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4,
};

static const byte cb_cycles_table[256] =
{
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,

  2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
  2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
  2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
  2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,

  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,

  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
};

static const byte incflag_table[256] =
{
  FZ | FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  FH, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const byte decflag_table[256] =
{
  FZ | FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH,
  FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN, FN | FH
};

static const byte swap_table[256] =
{
  0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0,
  0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x81, 0x91, 0xA1, 0xB1, 0xC1, 0xD1, 0xE1, 0xF1,
  0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 0x82, 0x92, 0xA2, 0xB2, 0xC2, 0xD2, 0xE2, 0xF2,
  0x03, 0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73, 0x83, 0x93, 0xA3, 0xB3, 0xC3, 0xD3, 0xE3, 0xF3,
  0x04, 0x14, 0x24, 0x34, 0x44, 0x54, 0x64, 0x74, 0x84, 0x94, 0xA4, 0xB4, 0xC4, 0xD4, 0xE4, 0xF4,
  0x05, 0x15, 0x25, 0x35, 0x45, 0x55, 0x65, 0x75, 0x85, 0x95, 0xA5, 0xB5, 0xC5, 0xD5, 0xE5, 0xF5,
  0x06, 0x16, 0x26, 0x36, 0x46, 0x56, 0x66, 0x76, 0x86, 0x96, 0xA6, 0xB6, 0xC6, 0xD6, 0xE6, 0xF6,
  0x07, 0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87, 0x97, 0xA7, 0xB7, 0xC7, 0xD7, 0xE7, 0xF7,
  0x08, 0x18, 0x28, 0x38, 0x48, 0x58, 0x68, 0x78, 0x88, 0x98, 0xA8, 0xB8, 0xC8, 0xD8, 0xE8, 0xF8,
  0x09, 0x19, 0x29, 0x39, 0x49, 0x59, 0x69, 0x79, 0x89, 0x99, 0xA9, 0xB9, 0xC9, 0xD9, 0xE9, 0xF9,
  0x0A, 0x1A, 0x2A, 0x3A, 0x4A, 0x5A, 0x6A, 0x7A, 0x8A, 0x9A, 0xAA, 0xBA, 0xCA, 0xDA, 0xEA, 0xFA,
  0x0B, 0x1B, 0x2B, 0x3B, 0x4B, 0x5B, 0x6B, 0x7B, 0x8B, 0x9B, 0xAB, 0xBB, 0xCB, 0xDB, 0xEB, 0xFB,
  0x0C, 0x1C, 0x2C, 0x3C, 0x4C, 0x5C, 0x6C, 0x7C, 0x8C, 0x9C, 0xAC, 0xBC, 0xCC, 0xDC, 0xEC, 0xFC,
  0x0D, 0x1D, 0x2D, 0x3D, 0x4D, 0x5D, 0x6D, 0x7D, 0x8D, 0x9D, 0xAD, 0xBD, 0xCD, 0xDD, 0xED, 0xFD,
  0x0E, 0x1E, 0x2E, 0x3E, 0x4E, 0x5E, 0x6E, 0x7E, 0x8E, 0x9E, 0xAE, 0xBE, 0xCE, 0xDE, 0xEE, 0xFE,
  0x0F, 0x1F, 0x2F, 0x3F, 0x4F, 0x5F, 0x6F, 0x7F, 0x8F, 0x9F, 0xAF, 0xBF, 0xCF, 0xDF, 0xEF, 0xFF,
};

#define ZFLAG(n) ( (n) ? 0 : FZ )
#define HFLAG(n) ( (n) ? 0 : FH )
#define CFLAG(n) ( (n) ? 0 : FC )

#define PUSH(w) ( (GBC_SP -= 2), (writew(GBC_SP, (w))) )
#define POP(w)  ( ((w) = readw(GBC_SP)), (GBC_SP += 2) )

#define FETCH (readb(GBC_PC++))

#define GBC_INC(r) { ((r)++); GBC_F = (GBC_F & (FL|FC)) | incflag_table[(r)]; }
#define GBC_DEC(r) { ((r)--); GBC_F = (GBC_F & (FL|FC)) | decflag_table[(r)]; }

#define INCW(r) ( (r)++ )
#define DECW(r) ( (r)-- )

#define ADD(n) { \
    GBC_W(acc) = (un16)GBC_A + (un16)(n); \
    GBC_F = (ZFLAG(GBC_LB(acc))) | (FH & ((GBC_A ^ (n) ^ GBC_LB(acc)) << 1)) | (GBC_HB(acc) << 4); \
    GBC_A = GBC_LB(acc); }

#define ADC(n) { \
    GBC_W(acc) = (un16)GBC_A + (un16)(n) + (un16)((GBC_F&FC)>>4); \
    GBC_F = (ZFLAG(GBC_LB(acc))) | (FH & ((GBC_A ^ (n) ^ GBC_LB(acc)) << 1)) | (GBC_HB(acc) << 4); \
    GBC_A = GBC_LB(acc); }

#define ADDW(n) { \
    temp = (un32)HL + (un32)(n); \
    GBC_F &= FZ; \
    if (0xFFFF - (n) < HL) GBC_F |= FC; \
    if ((HL & 0xFFF) + ((n) & 0xFFF) > 0xFFF) GBC_F |= FH; \
    HL = temp; }

#define CP(n) { \
    GBC_W(acc) = (un16)GBC_A - (un16)(n); \
    GBC_F = FN \
        | (ZFLAG(GBC_LB(acc))) \
        | (FH & ((GBC_A ^ (n) ^ GBC_LB(acc)) << 1)) \
        | ((un8)(-(n8)GBC_HB(acc)) << 4); }

#define SUB(n) { CP((n)); GBC_A = GBC_LB(acc); }

#define SBC(n) { \
    GBC_W(acc) = (un16)GBC_A - (un16)(n) - (un16)((GBC_F&FC)>>4); \
    GBC_F = FN \
        | (ZFLAG((n8)GBC_LB(acc))) \
        | (FH & ((GBC_A ^ (n) ^ GBC_LB(acc)) << 1)) \
        | ((un8)(-(n8)GBC_HB(acc)) << 4); \
    GBC_A = GBC_LB(acc); }

#define AND(n) { GBC_A &= (n); GBC_F = ZFLAG(GBC_A) | FH; }
#define XOR(n) { GBC_A ^= (n); GBC_F = ZFLAG(GBC_A); }
#define OR(n)  { GBC_A |= (n); GBC_F = ZFLAG(GBC_A); }

#define RLCA(r) { (r) = ((r)>>7) | ((r)<<1); GBC_F = (((r)&0x01)<<4); }
#define RRCA(r) { (r) = ((r)<<7) | ((r)>>1); GBC_F = (((r)&0x80)>>3); }
#define RLA(r) { GBC_LB(acc) = (((r)&0x80)>>3); (r) = ((r)<<1) | ((GBC_F&FC)>>4); GBC_F = GBC_LB(acc); }
#define RRA(r) { GBC_LB(acc) = (((r)&0x01)<<4); (r) = ((r)>>1) | ((GBC_F&FC)<<3); GBC_F = GBC_LB(acc); }

#define RLC(r) { RLCA(r); GBC_F |= ZFLAG(r); }
#define RRC(r) { RRCA(r); GBC_F |= ZFLAG(r); }
#define RL(r)  { RLA(r); GBC_F |= ZFLAG(r); }
#define RR(r)  { RRA(r); GBC_F |= ZFLAG(r); }

#define SLA(r) { GBC_LB(acc) = (((r)&0x80)>>3); (r) <<= 1; GBC_F = ZFLAG((r)) | GBC_LB(acc); }
#define SRA(r) { GBC_LB(acc) = (((r)&0x01)<<4); (r) = (un8)(((n8)(r))>>1); GBC_F = ZFLAG((r)) | GBC_LB(acc); }
#define SRL(r) { GBC_LB(acc) = (((r)&0x01)<<4); (r) >>= 1; GBC_F = ZFLAG((r)) | GBC_LB(acc); }

#define CPL(r) { (r) = ~(r); GBC_F |= (FH|FN); }

#define SCF { GBC_F = (GBC_F & (FZ)) | FC; }
#define CCF { GBC_F = (GBC_F & (FZ|FC)) ^ FC; }

#define SWAP(r) { (r) = swap_table[(r)]; GBC_F = ZFLAG((r)); }

#define BIT(n,r) { GBC_F = (GBC_F & FC) | ZFLAG(((r) & (1 << (n)))) | FH; }
#define RES(n,r) { (r) &= ~(1 << (n)); }
#define SET(n,r) { (r) |= (1 << (n)); }

#define JR ( GBC_PC += 1+(n8)readb(GBC_PC) )
#define JP ( GBC_PC = readw(GBC_PC) )

#define NOJR   ( clen--,  GBC_PC++ )
#define NOJP   ( clen--,  GBC_PC+=2 )
#define NOCALL ( clen-=3, GBC_PC+=2 )
#define NORET  ( clen-=3 )

#define RST(n) { PUSH(GBC_PC); GBC_PC = (n); }

#define CALL ( PUSH(GBC_PC+2), JP )
#define RET ( POP(GBC_PC) )

#define EI ( IMA = 1 )
#define DI ( GBC_cpu.halted = IMA = IME = 0 )

#define COND_EXEC_INT(i, n) if (temp & i) { DI; PUSH(GBC_PC); R_IF &= ~i; GBC_PC = 0x40+((n)<<3); clen = 5; goto _skip; }

#define CB_REG_CASES(r, n) \
  case 0x00|(n): RLC(r); break; \
  case 0x08|(n): RRC(r); break; \
  case 0x10|(n): RL(r); break; \
  case 0x18|(n): RR(r); break; \
  case 0x20|(n): SLA(r); break; \
  case 0x28|(n): SRA(r); break; \
  case 0x30|(n): SWAP(r); break; \
  case 0x38|(n): SRL(r); break; \
  case 0x40|(n): BIT(0, r); break; \
  case 0x48|(n): BIT(1, r); break; \
  case 0x50|(n): BIT(2, r); break; \
  case 0x58|(n): BIT(3, r); break; \
  case 0x60|(n): BIT(4, r); break; \
  case 0x68|(n): BIT(5, r); break; \
  case 0x70|(n): BIT(6, r); break; \
  case 0x78|(n): BIT(7, r); break; \
  case 0x80|(n): RES(0, r); break; \
  case 0x88|(n): RES(1, r); break; \
  case 0x90|(n): RES(2, r); break; \
  case 0x98|(n): RES(3, r); break; \
  case 0xA0|(n): RES(4, r); break; \
  case 0xA8|(n): RES(5, r); break; \
  case 0xB0|(n): RES(6, r); break; \
  case 0xB8|(n): RES(7, r); break; \
  case 0xC0|(n): SET(0, r); break; \
  case 0xC8|(n): SET(1, r); break; \
  case 0xD0|(n): SET(2, r); break; \
  case 0xD8|(n): SET(3, r); break; \
  case 0xE0|(n): SET(4, r); break; \
  case 0xE8|(n): SET(5, r); break; \
  case 0xF0|(n): SET(6, r); break; \
  case 0xF8|(n): SET(7, r); break;


#define ALU_CASES(base, imm, op, label) \
  case (imm): b = FETCH; goto label; \
  case (base): b = GBC_B; goto label; \
  case (base)+1: b = GBC_C; goto label; \
  case (base)+2: b = GBC_D; goto label; \
  case (base)+3: b = GBC_E; goto label; \
  case (base)+4: b = GBC_H; goto label; \
  case (base)+5: b = GBC_L; goto label; \
  case (base)+6: b = readb(HL); goto label; \
  case (base)+7: b = GBC_A; \
  label: op(b); break;


cpu_t GBC_cpu;


void cpu_reset(bool hard) {
  GBC_cpu.double_speed = 0;
  GBC_cpu.halted = 0;
  GBC_cpu.div = 0;
  GBC_cpu.timer = 0;
  // set lcdc ahead of GBC_cpu by 19us; see GBC_A
  // Set lcdc ahead of GBC_cpu by 19us (matches minimal hblank duration according
  // to some docs). Value from lcd.cycles (when positive) is used to drive CPU,
  // setting some ahead-time at startup is necessary to begin emulation.
  // FIXME: leave value at 0, use lcd_emulate() to actually send lcdc ahead
  
  lcd.cycles = 40;

  IME = 0;
  IMA = 0;

  GBC_PC = hw.bios ? 0x0000 : 0x0100;
  GBC_SP = 0xFFFE;
  AF = hw.cgb ? 0x11B0 : 0x01B0;
  BC = 0x0013;
  DE = 0x00D8;
  HL = 0x014D;
}

// cnt - time to emulate, expressed in real clock cycles 
static inline void timer_advance(int cycles) {
  GBC_cpu.div += (cycles << 2);

  R_DIV += (GBC_cpu.div >> 8);

  GBC_cpu.div &= 0xff;

  if (R_TAC & 0x04)
  {
    GBC_cpu.timer += (cycles << ((((-R_TAC) & 3) << 1) + 1));

    if (GBC_cpu.timer >= 512)
    {
      int tima = R_TIMA + (GBC_cpu.timer >> 9);
      GBC_cpu.timer &= 0x1ff;
      if (tima >= 256)
      {
        hw_interrupt(IF_TIMER, 1);
        hw_interrupt(IF_TIMER, 0);
        tima = R_TMA;
      }
      R_TIMA = tima;
    }
  }
}

// cnt - time to emulate, expressed in real clock cycles 
static inline void serial_advance(int cycles) {
  if (hw.serial > 0)
  {
    hw.serial -= cycles << 1;
    if (hw.serial <= 0)
    {
      R_SB = 0xFF;
      R_SC &= 0x7f;
      hw.serial = 0;
      hw_interrupt(IF_SERIAL, 1);
      hw_interrupt(IF_SERIAL, 0);
    }
  }
}

// cnt - time to emulate, expressed in double-speed cycles
// Will call lcd_emulate() if CPU emulation caught up or
// went ahead of LCDC, so that lcd never falls behind

static inline void lcdc_advance(int cycles)
{
  lcd.cycles -= cycles;
  if (lcd.cycles <= 0) lcd_emulate();
}

// cnt - time to emulate, expressed in double-speed cycles 
static inline void sound_advance(int cycles)
{
  snd.cycles += cycles;
}

// burn GBC_cpu cycles without running any instructions 
void cpu_burn(int cycles)
{

}

// cpu_emulate()
//  Emulate CPU for time no less than specified
//
//  cycles - time to emulate, expressed in double-speed cycles
//  returns number of cycles emulated
//
//  Might emulate up to cycles+(11) time units (longest op takes 12
//  cycles in single-speed mode)


#if BOOST_BY_IRAM 
IRAM_ATTR
#endif
int cpu_emulate(int cycles)
{
  int clen, temp;
  int remaining = cycles;
  int count = 0;
  byte op, b;
  cpu_reg_t acc;

  if (!GBC_cpu.double_speed)
    remaining >>= 1;

next:
  // Skip idle cycles 
  if (GBC_cpu.halted) {
    clen = 1;
    goto _skip;
  }

  // Handle interrupts 
  if (IME && (temp = R_IF & R_IE))
  {
    COND_EXEC_INT(IF_VBLANK, 0);
    COND_EXEC_INT(IF_STAT, 1);
    COND_EXEC_INT(IF_TIMER, 2);
    COND_EXEC_INT(IF_SERIAL, 3);
    COND_EXEC_INT(IF_PAD, 4);
  }
  IME = IMA;

  // if (GBC_cpu.disassemble)
  //  debug_disassemble(GBC_PC, 1);

  op = FETCH;
  clen = cycles_table[op];

  switch (op)
  {
    case 0x00: // NOP 
    case 0x40: // LD GBC_B,GBC_B 
    case 0x49: // LD GBC_C,GBC_C 
    case 0x52: // LD GBC_D,GBC_D 
    case 0x5B: // LD GBC_E,GBC_E 
    case 0x64: // LD GBC_H,GBC_H 
    case 0x6D: // LD GBC_L,GBC_L 
    case 0x7F: // LD GBC_A,GBC_A 
      break;

    case 0x41: // LD GBC_B,GBC_C 
      GBC_B = GBC_C; break;
    case 0x42: // LD GBC_B,GBC_D 
      GBC_B = GBC_D; break;
    case 0x43: // LD GBC_B,GBC_E 
      GBC_B = GBC_E; break;
    case 0x44: // LD GBC_B,GBC_H 
      GBC_B = GBC_H; break;
    case 0x45: // LD GBC_B,GBC_L 
      GBC_B = GBC_L; break;
    case 0x46: // LD GBC_B,(HL) 
      GBC_B = readb(HL); break;
    case 0x47: // LD GBC_B,GBC_A 
      GBC_B = GBC_A; break;

    case 0x48: // LD GBC_C,GBC_B 
      GBC_C = GBC_B; break;
    case 0x4A: // LD GBC_C,GBC_D 
      GBC_C = GBC_D; break;
    case 0x4B: // LD GBC_C,GBC_E 
      GBC_C = GBC_E; break;
    case 0x4C: // LD GBC_C,GBC_H 
      GBC_C = GBC_H; break;
    case 0x4D: // LD GBC_C,GBC_L 
      GBC_C = GBC_L; break;
    case 0x4E: // LD GBC_C,(HL) 
      GBC_C = readb(HL); break;
    case 0x4F: // LD GBC_C,GBC_A 
      GBC_C = GBC_A; break;

    case 0x50: // LD GBC_D,GBC_B 
      GBC_D = GBC_B; break;
    case 0x51: // LD GBC_D,GBC_C 
      GBC_D = GBC_C; break;
    case 0x53: // LD GBC_D,GBC_E 
      GBC_D = GBC_E; break;
    case 0x54: // LD GBC_D,GBC_H 
      GBC_D = GBC_H; break;
    case 0x55: // LD GBC_D,GBC_L 
      GBC_D = GBC_L; break;
    case 0x56: // LD GBC_D,(HL) 
      GBC_D = readb(HL); break;
    case 0x57: // LD GBC_D,GBC_A 
      GBC_D = GBC_A; break;

    case 0x58: // LD GBC_E,GBC_B 
      GBC_E = GBC_B; break;
    case 0x59: // LD GBC_E,GBC_C 
      GBC_E = GBC_C; break;
    case 0x5A: // LD GBC_E,GBC_D 
      GBC_E = GBC_D; break;
    case 0x5C: // LD GBC_E,GBC_H 
      GBC_E = GBC_H; break;
    case 0x5D: // LD GBC_E,GBC_L 
      GBC_E = GBC_L; break;
    case 0x5E: // LD GBC_E,(HL) 
      GBC_E = readb(HL); break;
    case 0x5F: // LD GBC_E,GBC_A 
      GBC_E = GBC_A; break;

    case 0x60: // LD GBC_H,GBC_B 
      GBC_H = GBC_B; break;
    case 0x61: // LD GBC_H,GBC_C 
      GBC_H = GBC_C; break;
    case 0x62: // LD GBC_H,GBC_D 
      GBC_H = GBC_D; break;
    case 0x63: // LD GBC_H,GBC_E 
      GBC_H = GBC_E; break;
    case 0x65: // LD GBC_H,GBC_L 
      GBC_H = GBC_L; break;
    case 0x66: // LD GBC_H,(HL) 
      GBC_H = readb(HL); break;
    case 0x67: // LD GBC_H,GBC_A 
      GBC_H = GBC_A; break;

    case 0x68: // LD GBC_L,GBC_B 
      GBC_L = GBC_B; break;
    case 0x69: // LD GBC_L,GBC_C 
      GBC_L = GBC_C; break;
    case 0x6A: // LD GBC_L,GBC_D 
      GBC_L = GBC_D; break;
    case 0x6B: // LD GBC_L,GBC_E 
      GBC_L = GBC_E; break;
    case 0x6C: // LD GBC_L,GBC_H 
      GBC_L = GBC_H; break;
    case 0x6E: // LD GBC_L,(HL) 
      GBC_L = readb(HL); break;
    case 0x6F: // LD GBC_L,GBC_A 
      GBC_L = GBC_A; break;

    case 0x70: // LD (HL),GBC_B 
      writeb(HL, GBC_B); break;
    case 0x71: // LD (HL),GBC_C 
      writeb(HL, GBC_C); break;
    case 0x72: // LD (HL),GBC_D 
      writeb(HL, GBC_D); break;
    case 0x73: // LD (HL),GBC_E 
      writeb(HL, GBC_E); break;
    case 0x74: // LD (HL),GBC_H 
      writeb(HL, GBC_H); break;
    case 0x75: // LD (HL),GBC_L 
      writeb(HL, GBC_L); break;
    case 0x77: // LD (HL),GBC_A 
      writeb(HL, GBC_A); break;

    case 0x78: // LD GBC_A,GBC_B 
      GBC_A = GBC_B; break;
    case 0x79: // LD GBC_A,GBC_C 
      GBC_A = GBC_C; break;
    case 0x7A: // LD GBC_A,GBC_D 
      GBC_A = GBC_D; break;
    case 0x7B: // LD GBC_A,GBC_E 
      GBC_A = GBC_E; break;
    case 0x7C: // LD GBC_A,GBC_H 
      GBC_A = GBC_H; break;
    case 0x7D: // LD GBC_A,GBC_L 
      GBC_A = GBC_L; break;
    case 0x7E: // LD GBC_A,(HL) 
      GBC_A = readb(HL); break;

    case 0x01: // LD BC,imm 
      BC = readw(GBC_PC); GBC_PC += 2; break;
    case 0x11: // LD DE,imm 
      DE = readw(GBC_PC); GBC_PC += 2; break;
    case 0x21: // LD HL,imm 
      HL = readw(GBC_PC); GBC_PC += 2; break;
    case 0x31: // LD GBC_SP,imm 
      GBC_SP = readw(GBC_PC); GBC_PC += 2; break;

    case 0x02: // LD (BC),GBC_A 
      writeb(BC, GBC_A); break;
    case 0x0A: // LD GBC_A,(BC) 
      GBC_A = readb(BC); break;
    case 0x12: // LD (DE),GBC_A 
      writeb(DE, GBC_A); break;
    case 0x1A: // LD GBC_A,(DE) 
      GBC_A = readb(DE); break;

    case 0x22: // LDI (HL),GBC_A 
      writeb(HL, GBC_A); HL++; break;
    case 0x2A: // LDI GBC_A,(HL) 
      GBC_A = readb(HL); HL++; break;
    case 0x32: // LDD (HL),GBC_A 
      writeb(HL, GBC_A); HL--; break;
    case 0x3A: // LDD GBC_A,(HL) 
      GBC_A = readb(HL); HL--; break;

    case 0x06: // LD GBC_B,imm 
      GBC_B = FETCH; break;
    case 0x0E: // LD GBC_C,imm 
      GBC_C = FETCH; break;
    case 0x16: // LD GBC_D,imm 
      GBC_D = FETCH; break;
    case 0x1E: // LD GBC_E,imm 
      GBC_E = FETCH; break;
    case 0x26: // LD GBC_H,imm 
      GBC_H = FETCH; break;
    case 0x2E: // LD GBC_L,imm 
      GBC_L = FETCH; break;
    case 0x36: // LD (HL),imm 
      writeb(HL, FETCH); break;
    case 0x3E: // LD GBC_A,imm 
      GBC_A = FETCH; break;

    case 0x08: // LD (imm),GBC_SP 
      writew(readw(GBC_PC), GBC_SP); GBC_PC += 2; break;
    case 0xEA: // LD (imm),GBC_A 
      writeb(readw(GBC_PC), GBC_A); GBC_PC += 2; break;

    case 0xE0: // LDH (imm),GBC_A 
      writeb(0xff00 + FETCH, GBC_A); break;
    case 0xE2: // LDH (GBC_C),GBC_A 
      writeb(0xff00 + GBC_C, GBC_A); break;
    case 0xF0: // LDH GBC_A,(imm) 
      GBC_A = readb(0xff00 + FETCH); break;
    case 0xF2: // LDH GBC_A,(GBC_C) (undocumented) 
      GBC_A = readb(0xff00 + GBC_C); break;

    case 0xF8: // LD HL,GBC_SP+imm 
      // https://gammpei.github.io/blog/posts/2018-03-04/how-to-write-a-game-boy-emulator-part-8-blarggs-GBC_cpu-test-roms-1-3-4-5-7-8-9-10-11.html
      b = FETCH;
      temp = (int)(GBC_SP) + (signed char)b;

      GBC_F &= ~(FZ | FN | FH | FC);

      if (((GBC_SP & 0xff) ^ b ^ temp) & 0x10) GBC_F |= FH;
      if ((GBC_SP & 0xff) + b > 0xff) GBC_F |= FC;

      HL = temp;
      break;
    case 0xF9: // LD GBC_SP,HL 
      GBC_SP = HL; break;
    case 0xFA: // LD GBC_A,(imm) 
      GBC_A = readb(readw(GBC_PC)); GBC_PC += 2; break;

      ALU_CASES(0x80, 0xC6, ADD, __ADD)
      ALU_CASES(0x88, 0xCE, ADC, __ADC)
      ALU_CASES(0x90, 0xD6, SUB, __SUB)
      ALU_CASES(0x98, 0xDE, SBC, __SBC)
      ALU_CASES(0xA0, 0xE6, AND, __AND)
      ALU_CASES(0xA8, 0xEE, XOR, __XOR)
      ALU_CASES(0xB0, 0xF6, OR, __OR)
      ALU_CASES(0xB8, 0xFE, CP, __CP)

    case 0x09: // ADD HL,BC 
      ADDW(BC); break;
    case 0x19: // ADD HL,DE 
      ADDW(DE); break;
    case 0x39: // ADD HL,GBC_SP 
      ADDW(GBC_SP); break;
    case 0x29: // ADD HL,HL 
      ADDW(HL); break;

    case 0x04: // GBC_INC GBC_B 
      GBC_INC(GBC_B); break;
    case 0x0C: // GBC_INC GBC_C 
      GBC_INC(GBC_C); break;
    case 0x14: // GBC_INC GBC_D 
      GBC_INC(GBC_D); break;
    case 0x1C: // GBC_INC GBC_E 
      GBC_INC(GBC_E); break;
    case 0x24: // GBC_INC GBC_H 
      GBC_INC(GBC_H); break;
    case 0x2C: // GBC_INC GBC_L 
      GBC_INC(GBC_L); break;
    case 0x34: // GBC_INC (HL) 
      b = readb(HL);
      GBC_INC(b);
      writeb(HL, b);
      break;
    case 0x3C: // GBC_INC GBC_A 
      GBC_INC(GBC_A); break;

    case 0x03: // GBC_INC BC 
      INCW(BC); break;
    case 0x13: // GBC_INC DE 
      INCW(DE); break;
    case 0x23: // GBC_INC HL 
      INCW(HL); break;
    case 0x33: // GBC_INC GBC_SP 
      INCW(GBC_SP); break;

    case 0x05: // GBC_DEC GBC_B 
      GBC_DEC(GBC_B); break;
    case 0x0D: // GBC_DEC GBC_C 
      GBC_DEC(GBC_C); break;
    case 0x15: // GBC_DEC GBC_D 
      GBC_DEC(GBC_D); break;
    case 0x1D: // GBC_DEC GBC_E 
      GBC_DEC(GBC_E); break;
    case 0x25: // GBC_DEC GBC_H 
      GBC_DEC(GBC_H); break;
    case 0x2D: // GBC_DEC GBC_L 
      GBC_DEC(GBC_L); break;
    case 0x35: // GBC_DEC (HL) 
      b = readb(HL);
      GBC_DEC(b);
      writeb(HL, b);
      break;
    case 0x3D: // GBC_DEC GBC_A 
      GBC_DEC(GBC_A); break;

    case 0x0B: // GBC_DEC BC 
      DECW(BC); break;
    case 0x1B: // GBC_DEC DE 
      DECW(DE); break;
    case 0x2B: // GBC_DEC HL 
      DECW(HL); break;
    case 0x3B: // GBC_DEC GBC_SP 
      DECW(GBC_SP); break;

    case 0x07: // RLCA 
      RLCA(GBC_A); break;
    case 0x0F: // RRCA 
      RRCA(GBC_A); break;
    case 0x17: // RLA 
      RLA(GBC_A); break;
    case 0x1F: // RRA 
      RRA(GBC_A); break;

    case 0x27: // DAA 
      //http://forums.nesdev.com/viewtopic.php?t=9088
      temp = GBC_A;

      if (GBC_F & FN)
      {
        if (GBC_F & FH) temp = (temp - 6) & 0xff;
        if (GBC_F & FC) temp -= 0x60;
      }
      else
      {
        if ((GBC_F & FH) || ((temp & 0x0f) > 9)) temp += 0x06;
        if ((GBC_F & FC) || (temp > 0x9f)) temp += 0x60;
      }

      GBC_A = (byte)temp;
      GBC_F &= ~(FH | FZ);

      if (temp & 0x100)   GBC_F |= FC;
      if (!(temp & 0xff)) GBC_F |= FZ;
      break;

    case 0x2F: // CPL 
      CPL(GBC_A); break;

    case 0x18: // JR 
      JR; break;
    case 0x20: // JR NZ 
      if (!(GBC_F & FZ)) JR; else NOJR; break;
    case 0x28: // JR Z 
      if (GBC_F & FZ) JR; else NOJR; break;
    case 0x30: // JR NC 
      if (!(GBC_F & FC)) JR; else NOJR; break;
    case 0x38: // JR GBC_C 
      if (GBC_F & FC) JR; else NOJR; break;

    case 0xC3: // JP 
      JP; break;
    case 0xC2: // JP NZ 
      if (!(GBC_F & FZ)) JP; else NOJP; break;
    case 0xCA: // JP Z 
      if (GBC_F & FZ) JP; else NOJP; break;
    case 0xD2: // JP NC 
      if (!(GBC_F & FC)) JP; else NOJP; break;
    case 0xDA: // JP GBC_C 
      if (GBC_F & FC) JP; else NOJP; break;
    case 0xE9: // JP HL 
      GBC_PC = HL; break;

    case 0xC9: // RET 
      RET; break;
    case 0xC0: // RET NZ 
      if (!(GBC_F & FZ)) RET; else NORET; break;
    case 0xC8: // RET Z 
      if (GBC_F & FZ) RET; else NORET; break;
    case 0xD0: // RET NC 
      if (!(GBC_F & FC)) RET; else NORET; break;
    case 0xD8: // RET GBC_C 
      if (GBC_F & FC) RET; else NORET; break;
    case 0xD9: // RETI 
      IME = IMA = 1; RET; break;

    case 0xCD: // CALL 
      CALL; break;
    case 0xC4: // CALL NZ 
      if (!(GBC_F & FZ)) CALL; else NOCALL; break;
    case 0xCC: // CALL Z 
      if (GBC_F & FZ) CALL; else NOCALL; break;
    case 0xD4: // CALL NC 
      if (!(GBC_F & FC)) CALL; else NOCALL; break;
    case 0xDC: // CALL GBC_C 
      if (GBC_F & FC) CALL; else NOCALL; break;

    case 0xC7: // RST 0 
      RST(0x00); break;
    case 0xCF: // RST 8 
      RST(0x08); break;
    case 0xD7: // RST 10 
      RST(0x10); break;
    case 0xDF: // RST 18 
      RST(0x18); break;
    case 0xE7: // RST 20 
      RST(0x20); break;
    case 0xEF: // RST 28 
      RST(0x28); break;
    case 0xF7: // RST 30 
      RST(0x30); break;
    case 0xFF: // RST 38 
      RST(0x38); break;

    case 0xC1: // POP BC 
      POP(BC); break;
    case 0xC5: // PUSH BC 
      PUSH(BC); break;
    case 0xD1: // POP DE 
      POP(DE); break;
    case 0xD5: // PUSH DE 
      PUSH(DE); break;
    case 0xE1: // POP HL 
      POP(HL); break;
    case 0xE5: // PUSH HL 
      PUSH(HL); break;
    case 0xF1: // POP AF 
      POP(AF); AF &= 0xfff0; break;
    case 0xF5: // PUSH AF 
      PUSH(AF); break;

    case 0xE8: // ADD GBC_SP,imm 
      // https://gammpei.github.io/blog/posts/2018-03-04/how-to-write-a-game-boy-emulator-part-8-blarggs-GBC_cpu-test-roms-1-3-4-5-7-8-9-10-11.html
      b = FETCH; // ADDSP(b); break;
      temp = GBC_SP + (signed char)b;

      GBC_F &= ~(FZ | FN | FH | FC);

      if ((GBC_SP ^ b ^ temp) & 0x10) GBC_F |= FH;
      if ((GBC_SP & 0xff) + b > 0xff) GBC_F |= FC;

      GBC_SP = temp;
      break;

    case 0xF3: // DI 
      DI; break;
    case 0xFB: // EI 
      EI; break;

    case 0x37: // SCF 
      SCF; break;
    case 0x3F: // CCF 
      CCF; break;

    case 0x10: // STOP 
      GBC_PC++;
      if (R_KEY1 & 1)
      {
        GBC_cpu.double_speed ^= 1;
        R_KEY1 = (R_KEY1 & 0x7E) | ((GBC_cpu.double_speed & 1) << 7);
        break;
      }
      // NOTE - we do not implement dmg STOP whatsoever 
      break;

    case 0x76: // HALT 
      GBC_cpu.halted = 1;
      if (!IME)
      {
        MESSAGE_DEBUG("FIX ME: HALT requested with IME = 0\n");
      }
      break;

    case 0xCB: // CB prefix 
      op = FETCH;
      clen = cb_cycles_table[op];
      switch (op)
      {
          CB_REG_CASES(GBC_B, 0);
          CB_REG_CASES(GBC_C, 1);
          CB_REG_CASES(GBC_D, 2);
          CB_REG_CASES(GBC_E, 3);
          CB_REG_CASES(GBC_H, 4);
          CB_REG_CASES(GBC_L, 5);
          CB_REG_CASES(GBC_A, 7);
        default:
          b = readb(HL);
          switch (op)
          {
              CB_REG_CASES(b, 6);
          }
          if ((op & 0xC0) != 0x40) // exclude BIT 
            writeb(HL, b);
          break;
      }
      break;

    default:
      gnuboy_die(
        "invalid opcode 0x%02X at address 0x%04X, rombank = %d\n",
        op, (GBC_PC - 1) & 0xffff, GBC_cart.rombank);
      break;
  }

_skip:

  remaining -= clen;
  count += clen;

#if COUNTERS_TICK_PERIOD > 1
  if (count >= COUNTERS_TICK_PERIOD || remaining <= 0)
#endif
  {
    // Advance clock-bound counters
    timer_advance(count);
    serial_advance(count);

    if (!GBC_cpu.double_speed)
      count <<= 1;

    // Advance fixed-speed counters 
    lcdc_advance(count);
    sound_advance(count);

    // Here we could calculate when the next event is going to happen
    // So that we can skip even more cycles, but it doesn't seem to save
    // much more CPU and adds lots of complexity...
    count = 0;
  }

  if (remaining > 0)
    goto next;

  return cycles + -remaining;
}


//--------------------------------------------------------------------------------
// HW.c
//--------------------------------------------------------------------------------

gb_cart_t GBC_cart;
gb_hw_t hw;



//  hw_interrupt changes the virtual interrupt line(s) defined by i
//  The interrupt fires (added to R_IF) when the line transitions from 0 to 1.
//  It does not refire if the line was already high.
void hw_interrupt(byte i, int level)
{
  if (level == 0)
  {
    hw.ilines &= ~i;
  }
  else if ((hw.ilines & i) == 0)
  {
    hw.ilines |= i;
    R_IF |= i; // Fire!

    if ((R_IE & i) != 0)
    {
      // Wake up the CPU when an enabled interrupt occurs
      // IME doesn't matter at this point, only IE
      GBC_cpu.halted = 0;
    }
  }
}

//  hw_dma performs plain old memory-to-oam dma, the original dmg
//  dma. Although on the hardware it takes a good deal of time, the GBC_cpu
//  continues running during this mode of dma, so no special tricks to
//  stall the GBC_cpu are necessary.

static void hw_dma(byte b)
{
  addr_t a = ((addr_t)b) << 8;
  for (int i = 0; i < 160; i++, a++)
    lcd.oam.mem[i] = readb(a);
}

static void hw_hdma_cmd(byte c)
{
  // Begin or cancel HDMA 
  if ((hw.hdma | c) & 0x80) {
    hw.hdma = c;
    R_HDMA5 = c & 0x7f;
    return;
  }

  // Perform GDMA 
  addr_t sa = ((addr_t)R_HDMA1 << 8) | (R_HDMA2 & 0xf0);
  addr_t da = 0x8000 | ((addr_t)(R_HDMA3 & 0x1f) << 8) | (R_HDMA4 & 0xf0);
  size_t cnt = ((int)c) + 1;
  // FIXME - this should use GBC_cpu time! 
  //cpu_timers(102 * cnt);
  cnt <<= 4;
  while (cnt--) writeb(da++, readb(sa++));
  R_HDMA1 = sa >> 8;
  R_HDMA2 = sa & 0xF0;
  R_HDMA3 = 0x1F & (da >> 8);
  R_HDMA4 = da & 0xF0;
  R_HDMA5 = 0xFF;
}


void hw_hdma()
{
  addr_t sa = ((addr_t)R_HDMA1 << 8) | (R_HDMA2 & 0xf0);
  addr_t da = 0x8000 | ((addr_t)(R_HDMA3 & 0x1f) << 8) | (R_HDMA4 & 0xf0);
  size_t cnt = 16;

  while (cnt--)
    writeb(da++, readb(sa++));
  R_HDMA1 = sa >> 8;
  R_HDMA2 = sa & 0xF0;
  R_HDMA3 = 0x1F & (da >> 8);
  R_HDMA4 = da & 0xF0;
  R_HDMA5--;
  hw.hdma--;
}



//   pad_refresh updates the P1 register from the pad states, generating
//   the appropriate interrupts (by quickly raising and lowering the
//   interrupt line) if a transition has been made.

static inline void pad_refresh()
{
  byte oldp1 = R_P1;
  R_P1 &= 0x30;
  R_P1 |= 0xc0;
  if (!(R_P1 & 0x10)) R_P1 |= (hw.pad & 0x0F);
  if (!(R_P1 & 0x20)) R_P1 |= (hw.pad >> 4);
  R_P1 ^= 0x0F;
  if (oldp1 & ~R_P1 & 0x0F)
  {
    hw_interrupt(IF_PAD, 1);
    hw_interrupt(IF_PAD, 0);
  }
}


//   hw_setpad updates the state of one or more buttons on the pad and calls
//   pad_refresh() to fire an interrupt if the pad changed.
void hw_setpad(byte btn, int set)
{
  un32 new_pad = hw.pad;

  if (set)
    new_pad |= btn;
  else
    new_pad &= ~btn;

  if (hw.pad != new_pad)
  {
    hw.pad = new_pad;
    pad_refresh();
  }
}

void hw_reset(bool hard)
{
  hw.ilines = 0;
  hw.serial = 0;
  hw.hdma = 0;
  hw.pad = 0;

  memset(hw.ioregs, 0, sizeof(hw.ioregs));
  R_P1 = 0xFF;
  R_LCDC = 0x91;
  R_BGP = 0xFC;
  R_OBP0 = 0xFF;
  R_OBP1 = 0xFF;
  R_SVBK = 0xF9;
  R_HDMA5 = 0xFF;
  R_VBK = 0xFE;

  if (hard)
  {
    //memset(hw.rambanks, 0xff, 4096 * 8);
    memset(gb_hw_rambanks[0], 0xff, 4096); 
    memset(gb_hw_rambanks[1], 0xff, 4096); 
    memset(gb_hw_rambanks[2], 0xff, 4096); 
    memset(gb_hw_rambanks[3], 0xff, 4096); 
    memset(gb_hw_rambanks[4], 0xff, 4096); 
    memset(gb_hw_rambanks[5], 0xff, 4096); 
    memset(gb_hw_rambanks[6], 0xff, 4096); 
    memset(gb_hw_rambanks[7], 0xff, 4096); 
    
    memset(GBC_cart.rambanks, 0xff, 8192 * GBC_cart.ramsize);
  }

  memset(hw.rmap, 0, sizeof(hw.rmap));
  memset(hw.wmap, 0, sizeof(hw.wmap));

  GBC_cart.sram_dirty = 0;
  GBC_cart.bankmode = 0;
  GBC_cart.rombank = 1;
  GBC_cart.rambank = 0;
  GBC_cart.enableram = 0;
  hw_updatemap();
}


void hw_vblank(void)
{
  //
}


//   In order to make reads and writes efficient, we keep tables
//   (indexed by the high nibble of the address) specifying which
//   regions can be read/written without a function call. For such
//   ranges, the pointer in the map table points to the base of the
//   region in host system memory. For ranges that require special
//   processing, the pointer is NULL.
//
//   hw_updatemap is called whenever bank changes or other operations
//   make the old maps potentially invalid.
void hw_updatemap(void)
{
  int rombank = GBC_cart.rombank % GBC_cart.romsize;

  if (GBC_cart.rombanks[rombank] == NULL)
  {
    gnuboy_load_bank(rombank);
  }

  // ROM
  hw.rmap[0x0] = GBC_cart.rombanks[0];
  hw.rmap[0x1] = GBC_cart.rombanks[0];
  hw.rmap[0x2] = GBC_cart.rombanks[0];
  hw.rmap[0x3] = GBC_cart.rombanks[0];

  // Force bios to go through hw_read (speed doesn't really matter here)
  if (hw.bios && (R_BIOS & 1) == 0)
  {
    hw.rmap[0x0] = NULL;
  }

  // Cartridge ROM
  hw.rmap[0x4] = GBC_cart.rombanks[rombank] - 0x4000;
  hw.rmap[0x5] = hw.rmap[0x4];
  hw.rmap[0x6] = hw.rmap[0x4];
  hw.rmap[0x7] = hw.rmap[0x4];

  // Video RAM
  hw.rmap[0x8] = hw.wmap[0x8] = gb_lcd_vbank[R_VBK & 1] - 0x8000;  // mallocated
  hw.rmap[0x9] = hw.wmap[0x9] = gb_lcd_vbank[R_VBK & 1] - 0x8000;  // mallocated

  // Cartridge RAM
  hw.rmap[0xA] = hw.wmap[0xA] = NULL;
  hw.rmap[0xB] = hw.wmap[0xB] = NULL;

  // Work RAM
  hw.rmap[0xC] = hw.wmap[0xC] = gb_hw_rambanks[0] - 0xC000; // mallocated

  // Work RAM (GBC)
  hw.rmap[0xD] = hw.wmap[0xD] = hw.cgb ? (gb_hw_rambanks[(R_SVBK & 0x7) ? : 1] - 0xD000) : NULL; 

  // Mirror of 0xC000
  hw.rmap[0xE] = hw.wmap[0xE] = gb_hw_rambanks[0] - 0xE000;

  // IO port and registers
  hw.rmap[0xF] = hw.wmap[0xF] = NULL;
}

//   Memory bank controllers typically intercept write attempts to
//   0000-7FFF, using the address and byte written as instructions to
//   change GBC_cart or sram banks, control special hardware, etc.
//
//   mbc_write takes an address (which should be in the proper range)
//   and a byte value written to the address.

static inline void mbc_write(addr_t a, byte b)
{
  MESSAGE_DEBUG("mbc %d: GBC_cart bank %02X -[%04X:%02X]-> ", GBC_cart.mbc, GBC_cart.rombank, a, b);

  switch (GBC_cart.mbc)
  {
    case MBC_MBC1:
      switch (a & 0xE000)
      {
        case 0x0000:
          GBC_cart.enableram = ((b & 0x0F) == 0x0A);
          break;
        case 0x2000:
          if ((b & 0x1F) == 0) b = 0x01;
          GBC_cart.rombank = (GBC_cart.rombank & 0x60) | (b & 0x1F);
          break;
        case 0x4000:
          if (GBC_cart.bankmode)
            GBC_cart.rambank = b & 0x03;
          else
            GBC_cart.rombank = (GBC_cart.rombank & 0x1F) | ((int)(b & 3) << 5);
          break;
        case 0x6000:
          GBC_cart.bankmode = b & 0x1;
          break;
      }
      break;

    case MBC_MBC2:
      // 0x0000 - 0x3FFF, bit 0x0100 clear = RAM, set = ROM
      if ((a & 0xC100) == 0x0000)
        GBC_cart.enableram = ((b & 0x0F) == 0x0A);
      else if ((a & 0xC100) == 0x0100)
        GBC_cart.rombank = b & 0x0F;
      break;

    case MBC_HUC3:
    // FIX ME: This isn't right but the previous implementation was wronger...
    case MBC_MBC3:
      switch (a & 0xE000)
      {
        case 0x0000:
          GBC_cart.enableram = ((b & 0x0F) == 0x0A);
          break;
        case 0x2000:
          if ((b & 0x7F) == 0) b = 0x01;
          GBC_cart.rombank = b & 0x7F;
          break;
        case 0x4000:
          rtc.sel = b & 0x0f;
          GBC_cart.rambank = b & 0x03;
          break;
        case 0x6000:
          rtc_latch(b);
          break;
      }
      break;

    case MBC_MBC5:
      switch (a & 0x7000)
      {
        case 0x0000:
        case 0x1000:
          GBC_cart.enableram = ((b & 0x0F) == 0x0A);
          break;
        case 0x2000:
          GBC_cart.rombank = (GBC_cart.rombank & 0x100) | (b);
          break;
        case 0x3000:
          GBC_cart.rombank = (GBC_cart.rombank & 0xFF) | ((int)(b & 1) << 8);
          break;
        case 0x4000:
        case 0x5000:
          if (GBC_cart.has_rumble)
            GBC_cart.rambank = b & 0x0F;
          else
            GBC_cart.rambank = b & ~8;
          break;
        case 0x6000:
        case 0x7000:
          // Nothing but Radikal Bikers tries to access it.
          break;
        default:
          MESSAGE_ERROR("MBC_MBC5: invalid write to 0x%x (0x%x)\n", a, b);
          break;
      }
      break;

    case MBC_HUC1: // FIXME - this is all guesswork -- is it right??? 
      switch (a & 0xE000)
      {
        case 0x0000:
          GBC_cart.enableram = ((b & 0x0F) == 0x0A);
          break;
        case 0x2000:
          if ((b & 0x1F) == 0) b = 0x01;
          GBC_cart.rombank = (GBC_cart.rombank & 0x60) | (b & 0x1F);
          break;
        case 0x4000:
          if (GBC_cart.bankmode)
            GBC_cart.rambank = b & 0x03;
          else
            GBC_cart.rombank = (GBC_cart.rombank & 0x1F) | ((int)(b & 3) << 5);
          break;
        case 0x6000:
          GBC_cart.bankmode = b & 0x1;
          break;
      }
      break;
  }

  MESSAGE_DEBUG("%02X\n", GBC_cart.rombank);

  hw_updatemap();
}



//   hw_write is the basic write function. Although it should only be
//   called when the write map contains a NULL for the requested address
//   region, it accepts writes to any address.

void hw_write(addr_t a, byte b)
{
  MESSAGE_DEBUG("write to 0x%04X: 0x%02X\n", a, b);

  switch (a & 0xE000)
  {
    case 0x0000: // MBC control (Cart ROM address space)
    case 0x2000:
    case 0x4000:
    case 0x6000:
      mbc_write(a, b);
      break;

    case 0x8000: // Video RAM
      gb_lcd_vbank[R_VBK & 1][a & 0x1FFF] = b; // mallocated
      break;

    case 0xA000: // Save RAM or RTC
      if (!GBC_cart.enableram)
        break;

      if (rtc.sel & 8)
      {
        rtc_write(b);
      }
      else
      {
        if (GBC_cart.rambanks[GBC_cart.rambank][a & 0x1FFF] != b)
        {
          GBC_cart.rambanks[GBC_cart.rambank][a & 0x1FFF] = b;
          GBC_cart.sram_dirty |= (1 << GBC_cart.rambank);
        }
      }
      break;

    case 0xC000: // System RAM
      if ((a & 0xF000) == 0xC000)
        gb_hw_rambanks[0][a & 0x0FFF] = b; // mallocated
      else
        gb_hw_rambanks[(R_SVBK & 0x7) ? : 1][a & 0x0FFF] = b; // mallocated
      break;

    case 0xE000: // Memory mapped IO
      // Mirror RAM: 0xE000 - 0xFDFF
      if (a < 0xFE00)
      {
        gb_hw_rambanks[(a >> 12) & 1][a & 0xFFF] = b; // mallocated
      }
      // Video: 0xFE00 - 0xFE9F
      else if ((a & 0xFF00) == 0xFE00)
      {
        if (a < 0xFEA0) lcd.oam.mem[a & 0xFF] = b;
      }
      // Sound: 0xFF10 - 0xFF3F
      else if (a >= 0xFF10 && a <= 0xFF3F)
      {
        sound_write(a & 0xFF, b);
      }
      // High RAM: 0xFF80 - 0xFFFE
      else if (a >= 0xFF80 && a <= 0xFFFE)
      {
        REG(a & 0xFF) = b;
      }
      // Hardware registers: 0xFF00 - 0xFF7F and 0xFFFF
      else
      {
        int r = a & 0xFF;
        if (!hw.cgb)
        {
          switch (r)
          {
            case RI_BGP:
              pal_write_dmg(0, 0, b);
              pal_write_dmg(8, 1, b);
              break;
            case RI_OBP0:
              pal_write_dmg(64, 2, b);
              break;
            case RI_OBP1:
              pal_write_dmg(72, 3, b);
              break;

            // These don't exist on DMG:
            case RI_VBK:
            case RI_BCPS:
            case RI_OCPS:
            case RI_BCPD:
            case RI_OCPD:
            case RI_SVBK:
            case RI_KEY1:
            case RI_HDMA1:
            case RI_HDMA2:
            case RI_HDMA3:
            case RI_HDMA4:
            case RI_HDMA5:
              return;
          }
        }

        switch (r)
        {
          case RI_TIMA:
          case RI_TMA:
          case RI_TAC:
          case RI_SCY:
          case RI_SCX:
          case RI_WY:
          case RI_WX:
          case RI_BGP:
          case RI_OBP0:
          case RI_OBP1:
            REG(r) = b;
            break;
          case RI_IF:
          case RI_IE:
            REG(r) = b & 0x1F;
            break;
          case RI_P1:
            REG(r) = b;
            pad_refresh();
            break;
          case RI_SC:
            if ((b & 0x81) == 0x81)
              hw.serial = 1952; // 8 * 122us;
            else
              hw.serial = 0;
            R_SC = b; // & 0x7f; 
            break;
          case RI_SB:
            REG(r) = b;
            break;
          case RI_DIV:
            REG(r) = 0;
            break;
          case RI_LCDC:
            lcd_lcdc_change(b);
            break;
          case RI_STAT:
            R_STAT = (R_STAT & 0x07) | (b & 0x78);
            if (!hw.cgb && !(R_STAT & 2)) // DMG STAT write bug => interrupt 
              hw_interrupt(IF_STAT, 1);
            lcd_stat_trigger();
            break;
          case RI_LYC:
            REG(r) = b;
            lcd_stat_trigger();
            break;
          case RI_VBK:
            REG(r) = b | 0xFE;
            hw_updatemap();
            break;
          case RI_BCPS:
            R_BCPS = b & 0xBF;
            R_BCPD = lcd.pal[b & 0x3F];
            break;
          case RI_OCPS:
            R_OCPS = b & 0xBF;
            R_OCPD = lcd.pal[64 + (b & 0x3F)];
            break;
          case RI_BCPD:
            R_BCPD = b;
            pal_write_cgb(R_BCPS & 0x3F, b);
            if (R_BCPS & 0x80) R_BCPS = (R_BCPS + 1) & 0xBF;
            break;
          case RI_OCPD:
            R_OCPD = b;
            pal_write_cgb(64 + (R_OCPS & 0x3F), b);
            if (R_OCPS & 0x80) R_OCPS = (R_OCPS + 1) & 0xBF;
            break;
          case RI_SVBK:
            REG(r) = b | 0xF8;
            hw_updatemap();
            break;
          case RI_DMA:
            hw_dma(b);
            break;
          case RI_KEY1:
            REG(r) = (REG(r) & 0x80) | (b & 0x01);
            break;
          case RI_BIOS:
            REG(r) = b;
            hw_updatemap();
            break;
          case RI_HDMA1:
          case RI_HDMA2:
          case RI_HDMA3:
          case RI_HDMA4:
            REG(r) = b;
            break;
          case RI_HDMA5:
            hw_hdma_cmd(b);
            break;
        }
      }
  }
}



//   hw_read is the basic read function...not useful for much anymore
//   with the read map, but it's still necessary for the final messy
//   region.

byte hw_read(addr_t a)
{
  MESSAGE_DEBUG("read %04x\n", a);

  switch (a & 0xE000)
  {
    case 0x0000: // Cart ROM bank 0
      // BIOS is overlayed part of bank 0
      if (a < 0x900 && (R_BIOS & 1) == 0)
      {
        if (a < 0x100)
          return hw.bios[a];
        if (hw.cgb && a >= 0x200)
          return hw.bios[a];
      }
    // fall through
    case 0x2000:
      return GBC_cart.rombanks[0][a & 0x3fff];

    case 0x4000: // Cart ROM
    case 0x6000:
      
      if (GBC_IN_HIMEM && GBC_cart.rombanks[GBC_cart.rombank]==NULL) {
        gnuboy_load_bank(GBC_cart.rombank);  
      }
     
      return GBC_cart.rombanks[GBC_cart.rombank][a & 0x3FFF];
       
    case 0x8000: // Video RAM
      return gb_lcd_vbank[R_VBK & 1][a & 0x1FFF]; // mallocated
      

    case 0xA000: // Cart RAM or RTC
      if (!GBC_cart.enableram)
        return 0xFF;
      if (rtc.sel & 8)
        return rtc.regs[rtc.sel & 7];
      return GBC_cart.rambanks[GBC_cart.rambank][a & 0x1FFF];

    case 0xC000: // System RAM
      if ((a & 0xF000) == 0xC000) return gb_hw_rambanks[0][a & 0xFFF]; // mallocated
      return gb_hw_rambanks[(R_SVBK & 0x7) ? : 1][a & 0xFFF];          // mallocated
      

    case 0xE000: // Memory mapped IO
      // Mirror RAM: 0xE000 - 0xFDFF
      if (a < 0xFE00)
      {
        return gb_hw_rambanks[(a >> 12) & 1][a & 0xFFF]; // mallocated
        
      }
      // Video: 0xFE00 - 0xFE9F
      else if ((a & 0xFF00) == 0xFE00)
      {
        return (a < 0xFEA0) ? lcd.oam.mem[a & 0xFF] : 0xFF;
      }
      // Sound: 0xFF10 - 0xFF3F
      else if (a >= 0xFF10 && a <= 0xFF3F)
      {
        // Make sure sound emulation is all caught up
        sound_mix();
      }
      // High RAM: 0xFF80 - 0xFFFE
      // else if ((a & 0xFF80) == 0xFF80)
      // {
      //  return REG(a & 0xFF);
      // }
      // Hardware registers: 0xFF00 - 0xFF7F and 0xFFFF
      // else
      // {

      // We should check that the reg is valid, otherwise return 0xFF.
      // However invalid address should already contain 0xFF (unless incorrectly overwritten)
      // So, for speed, we'll assume that this is correct and always return REG(a)
      return REG(a & 0xFF);
      // }
  }
  return 0xFF;
}


//--------------------------------------------------------------------------------
// RTC.c
//--------------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>

gb_rtc_t rtc;

// Set in the far future for VBA-M support
#define RT_BASE 1893456000

void rtc_reset(bool hard)
{
  if (hard)
  {
    memset(&rtc, 0, sizeof(rtc));
  }
}

void rtc_latch(byte b)
{
  if ((rtc.latch ^ b) & b & 1)
  {
    rtc.regs[0] = rtc.s;
    rtc.regs[1] = rtc.m;
    rtc.regs[2] = rtc.h;
    rtc.regs[3] = rtc.d;
    rtc.regs[4] = rtc.flags;
  }
  rtc.latch = b & 1;
}

void rtc_write(byte b)
{
  MESSAGE_DEBUG("write %02X: %d\n", rtc.sel, b);

  switch (rtc.sel & 0xf)
  {
    case 0x8: // Seconds
      rtc.regs[0] = b;
      rtc.s = b % 60;
      break;
    case 0x9: // Minutes
      rtc.regs[1] = b;
      rtc.m = b % 60;
      break;
    case 0xA: // Hours
      rtc.regs[2] = b;
      rtc.h = b % 24;
      break;
    case 0xB: // Days (lower 8 bits)
      rtc.regs[3] = b;
      rtc.d = ((rtc.d & 0x100) | b) % 365;
      break;
    case 0xC: // Flags (days upper 1 bit, carry, stop)
      rtc.regs[4] = b;
      rtc.flags = b;
      rtc.d = ((rtc.d & 0xff) | ((b & 1) << 9)) % 365;
      break;
  }
  rtc.dirty = 1;
}

void rtc_tick()
{
  if ((rtc.flags & 0x40))
    return; // rtc stop

  if (++rtc.ticks >= 60)
  {
    if (++rtc.s >= 60)
    {
      if (++rtc.m >= 60)
      {
        if (++rtc.h >= 24)
        {
          if (++rtc.d >= 365)
          {
            rtc.d = 0;
            rtc.flags |= 0x80;
          }
          rtc.h = 0;
        }
        rtc.m = 0;
      }
      rtc.s = 0;
    }
    rtc.ticks = 0;
  }
}

int rtc_save(const char *file)
{

}

int rtc_load(const char *file)
{

}


//--------------------------------------------------------------------------------
// LCD.c
//--------------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Our custom colorization palettes
static const uint16_t custom_palettes[][4] = {
  { 0x0000, 0x0000, 0x0000, 0x0000 }, // GB_PALETTE_GBCBIOS
  { 0x6BDD, 0x3ED4, 0x1D86, 0x0860 }, // GB_PALETTE_DEFAULT
  { 0x7FFF, 0x5AD6, 0x318C, 0x0000 }, // GB_PALETTE_2BGRAYS
  { 0x5BFF, 0x3F0F, 0x222D, 0x10EB }, // GB_PALETTE_LINKSAW
  { 0x639E, 0x263A, 0x10D4, 0x2866 }, // GB_PALETTE_NSUPRGB
  { 0x36D5, 0x260E, 0x1D47, 0x18C4 }, // GB_PALETTE_NGBARNE
  { 0x6FDF, 0x36DE, 0x4996, 0x34AC }, // GB_PALETTE_GRAPEFR
  { 0x6739, 0x6E6D, 0x4588, 0x1882 }, // GB_PALETTE_MEGAMAN
  { 0x7FBF, 0x46DE, 0x4DD0, 0x0843 }, // GB_PALETTE_POKEMON
  { 0x0272, 0x0DCA, 0x0D45, 0x0102 }, // GB_PALETTE_DMGREEN
};

// Game-specific colorization palettes extracted from GBC's BIOS
static const uint16_t colorization_palettes[32][3][4] = {
  {{0x7FFF, 0x01DF, 0x0112, 0x0000}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x42B5, 0x3DC8, 0x0000}},
  {{0x231F, 0x035F, 0x00F2, 0x0009}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x4FFF, 0x7ED2, 0x3A4C, 0x1CE0}},
  {{0x7FFF, 0x7FFF, 0x7E8C, 0x7C00}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x03ED, 0x7FFF, 0x255F, 0x0000}},
  {{0x7FFF, 0x7FFF, 0x7E8C, 0x7C00}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x036A, 0x021F, 0x03FF, 0x7FFF}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x03EF, 0x01D6, 0x0000}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x03EA, 0x011F, 0x0000}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x027F, 0x001F, 0x0000}},
  {{0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x03FF, 0x001F, 0x0000}},
  {{0x299F, 0x001A, 0x000C, 0x0000}, {0x7C00, 0x7FFF, 0x3FFF, 0x7E00}, {0x7E74, 0x03FF, 0x0180, 0x0000}},
  {{0x7FFF, 0x01DF, 0x0112, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x67FF, 0x77AC, 0x1A13, 0x2D6B}},
  {{0x0000, 0x7FFF, 0x421F, 0x1CF2}, {0x0000, 0x7FFF, 0x421F, 0x1CF2}, {0x7ED6, 0x4BFF, 0x2175, 0x0000}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x3FFF, 0x7E00, 0x001F}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}},
  {{0x231F, 0x035F, 0x00F2, 0x0009}, {0x7FFF, 0x7EEB, 0x001F, 0x7C00}, {0x7FFF, 0x6E31, 0x454A, 0x0000}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x6E31, 0x454A, 0x0000}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}},
  {{0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}},
  {{0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}},
  {{0x7FFF, 0x03E0, 0x0206, 0x0120}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x421F, 0x1CF2, 0x0000}},
  {{0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x0000, 0x4200, 0x037F, 0x7FFF}},
  {{0x03FF, 0x001F, 0x000C, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}},
  {{0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x42B5, 0x3DC8, 0x0000}},
  {{0x7FFF, 0x5294, 0x294A, 0x0000}, {0x7FFF, 0x5294, 0x294A, 0x0000}, {0x7FFF, 0x5294, 0x294A, 0x0000}},
  {{0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x53FF, 0x4A5F, 0x7E52, 0x0000}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}},
  {{0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x639F, 0x4279, 0x15B0, 0x04CB}},
  {{0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x1BEF, 0x0200, 0x0000}, {0x7FFF, 0x03FF, 0x012F, 0x0000}},
  {{0x7FFF, 0x033F, 0x0193, 0x0000}, {0x7FFF, 0x033F, 0x0193, 0x0000}, {0x7FFF, 0x033F, 0x0193, 0x0000}},
  {{0x7FFF, 0x421F, 0x1CF2, 0x0000}, {0x7FFF, 0x7E8C, 0x7C00, 0x0000}, {0x7FFF, 0x1BEF, 0x6180, 0x0000}},
  {{0x2120, 0x8022, 0x8281, 0x1110}, {0xFF7F, 0xDF7F, 0x1201, 0x0001}, {0xFF00, 0xFF7F, 0x1F03, 0x0000}},
  {{0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}},
  {{0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}, {0x7FFF, 0x32BF, 0x00D0, 0x0000}}
};

#define BG (lcd.BG)
#define WND (lcd.WND)
#define BUF (lcd.BUF)
#define PRI (lcd.PRI)
#define VS (lcd.VS)
#define CYCLES (lcd.cycles)
#define S lcd.S // tilemap position 
#define GBC_T lcd.GBC_T
#define U lcd.U // position within tile 
#define V lcd.V
#define WX lcd.WX
#define WY lcd.WY
#define WT lcd.WT
#define WV lcd.WV

gb_lcd_t lcd;

#define priused(attr) ({un32 *a = (un32*)(attr); (int)((a[0]|a[1]|a[2]|a[3]|a[4]|a[5]|a[6]|a[7])&0x80808080);})

#define blendcpy(dest, src, b, cnt) {         \
    byte *s = (src), *d = (dest), _b = (b), c = (cnt);  \
    while(c--) *(d + c) = *(s + c) | _b;        \
  }



//   Drawing routines

__attribute__((optimize("unroll-loops")))
static inline byte *get_patpix(int tile, int x)
{
  const byte *vram = gb_lcd_vbank[0]; // mallocated
  
  static byte pix[8];

  if (tile & (1 << 11)) // Vertical Flip
    vram += ((tile & 0x3FF) << 4) | ((7 - x) << 1);
  else
    vram += ((tile & 0x3FF) << 4) | (x << 1);

  if (tile & (1 << 10)) // Horizontal Flip
    for (int k = 0; k < 8; ++k)
    {
      pix[k] = ((vram[0] >> k) & 1) | (((vram[1] >> k) & 1) << 1);
    }
  else
    for (int k = 0; k < 8; ++k)
    {
      pix[7 - k] = ((vram[0] >> k) & 1) | (((vram[1] >> k) & 1) << 1);
    }

  return pix;
}

static inline void tilebuf()
{
  int cnt, base;
  byte *tilemap, *attrmap;
  int *tilebuf;

  // Background tiles 

  const int8_t wraptable[64] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32
  };
  const int8_t *wrap = wraptable + S;

  base = ((R_LCDC & 0x08) ? 0x1C00 : 0x1800) + (GBC_T << 5) + S;
  tilemap = gb_lcd_vbank[0] + base; // mallocated
  attrmap = gb_lcd_vbank[1] + base; // mallocated

  tilebuf = BG;
  cnt = ((WX + 7) >> 3) + 1;

  if (hw.cgb)
  {
    if (R_LCDC & 0x10)
      for (int i = cnt; i > 0; i--)
      {
        *(tilebuf++) = *tilemap
                       | (((int) * attrmap & 0x08) << 6)
                       | (((int) * attrmap & 0x60) << 5);
        *(tilebuf++) = (((int) * attrmap & 0x07) << 2);
        attrmap += *wrap + 1;
        tilemap += *(wrap++) + 1;
      }
    else
      for (int i = cnt; i > 0; i--)
      {
        *(tilebuf++) = (0x100 + ((n8) * tilemap))
                       | (((int) * attrmap & 0x08) << 6)
                       | (((int) * attrmap & 0x60) << 5);
        *(tilebuf++) = (((int) * attrmap & 0x07) << 2);
        attrmap += *wrap + 1;
        tilemap += *(wrap++) + 1;
      }
  }
  else
  {
    if (R_LCDC & 0x10)
      for (int i = cnt; i > 0; i--)
      {
        *(tilebuf++) = *(tilemap++);
        tilemap += *(wrap++);
      }
    else
      for (int i = cnt; i > 0; i--)
      {
        *(tilebuf++) = (0x100 + ((n8) * (tilemap++)));
        tilemap += *(wrap++);
      }
  }

  if (WX >= 160) return;

  // Window tiles 

  base = ((R_LCDC & 0x40) ? 0x1C00 : 0x1800) + (WT << 5);

  tilemap = gb_lcd_vbank[0] + base; // mallocated
  attrmap = gb_lcd_vbank[1] + base; // mallocated
  
  tilebuf = WND;
  cnt = ((160 - WX) >> 3) + 1;

  if (hw.cgb)
  {
    if (R_LCDC & 0x10)
      for (int i = cnt; i > 0; i--)
      {
        *(tilebuf++) = *(tilemap++)
                       | (((int) * attrmap & 0x08) << 6)
                       | (((int) * attrmap & 0x60) << 5);
        *(tilebuf++) = (((int) * (attrmap++) & 0x7) << 2);
      }
    else
      for (int i = cnt; i > 0; i--)
      {
        *(tilebuf++) = (0x100 + ((n8) * (tilemap++)))
                       | (((int) * attrmap & 0x08) << 6)
                       | (((int) * attrmap & 0x60) << 5);
        *(tilebuf++) = (((int) * (attrmap++) & 0x7) << 2);
      }
  }
  else
  {
    if (R_LCDC & 0x10)
      for (int i = cnt; i > 0; i--)
        *(tilebuf++) = *(tilemap++);
    else
      for (int i = cnt; i > 0; i--)
        *(tilebuf++) = (0x100 + ((n8) * (tilemap++)));
  }
}

static inline void bg_scan()
{
  int cnt;
  byte *src, *dest;
  int *tile;

  if (WX <= 0) return;

  cnt = WX;
  tile = BG;
  dest = BUF;

  src = get_patpix(*(tile++), V) + U;
  memcpy(dest, src, 8 - U);
  dest += 8 - U;
  cnt -= 8 - U;

  while (cnt >= 0)
  {
    src = get_patpix(*(tile++), V);
    memcpy(dest, src, 8);
    dest += 8;
    cnt -= 8;
  }
}

static inline void wnd_scan()
{
  int cnt;
  byte *src, *dest;
  int *tile;

  cnt = 160 - WX;
  tile = WND;
  dest = BUF + WX;

  while (cnt >= 0)
  {
    src = get_patpix(*(tile++), WV);
    memcpy(dest, src, 8);
    dest += 8;
    cnt -= 8;
  }
}

static inline void bg_scan_pri()
{
  int cnt, i;
  byte *src, *dest;

  if (WX <= 0) return;

  i = S;
  cnt = WX;
  dest = PRI;
  src = gb_lcd_vbank[1] + ((R_LCDC & 0x08) ? 0x1C00 : 0x1800) + (GBC_T << 5); // mallocated

  if (!priused(src))
  {
    memset(dest, 0, cnt);
    return;
  }

  memset(dest, src[i++ & 31] & 128, 8 - U);
  dest += 8 - U;
  cnt -= 8 - U;

  if (cnt <= 0) return;

  while (cnt >= 8)
  {
    memset(dest, src[i++ & 31] & 128, 8);
    dest += 8;
    cnt -= 8;
  }
  memset(dest, src[i & 31] & 128, cnt);
}

static inline void wnd_scan_pri()
{
  int cnt, i;
  byte *src, *dest;

  if (WX >= 160) return;

  i = 0;
  cnt = 160 - WX;
  dest = PRI + WX;
  //src = lcd.vbank[1] + ((R_LCDC & 0x40) ? 0x1C00 : 0x1800) + (WT << 5);
  src = gb_lcd_vbank[1] + ((R_LCDC & 0x40) ? 0x1C00 : 0x1800) + (WT << 5);
  

  if (!priused(src))
  {
    memset(dest, 0, cnt);
    return;
  }

  while (cnt >= 8)
  {
    memset(dest, src[i++] & 128, 8);
    dest += 8;
    cnt -= 8;
  }

  memset(dest, src[i] & 128, cnt);
}

static inline void bg_scan_color()
{
  int cnt;
  byte *src, *dest;
  int *tile;

  if (WX <= 0) return;

  cnt = WX;
  tile = BG;
  dest = BUF;

  src = get_patpix(*(tile++), V) + U;
  blendcpy(dest, src, *(tile++), 8 - U);
  dest += 8 - U;
  cnt -= 8 - U;

  while (cnt >= 0)
  {
    src = get_patpix(*(tile++), V);
    blendcpy(dest, src, *(tile++), 8);
    dest += 8;
    cnt -= 8;
  }
}

static inline void wnd_scan_color()
{
  int cnt;
  byte *src, *dest;
  int *tile;

  if (WX >= 160) return;

  cnt = 160 - WX;
  tile = WND;
  dest = BUF + WX;

  while (cnt >= 0)
  {
    src = get_patpix(*(tile++), WV);
    blendcpy(dest, src, *(tile++), 8);
    dest += 8;
    cnt -= 8;
  }
}

static inline int spr_enum()
{
  if (!(R_LCDC & 0x02))
    return 0;

  gb_vs_t ts[10];
  int line = R_LY;
  int GBC_NS = 0;

  for (int i = 0; i < 40; ++i)
  {
    gb_obj_t *obj = &lcd.oam.obj[i];
    int v, pat;

    if (line >= obj->y || line + 16 < obj->y)
      continue;
    if (line + 8 >= obj->y && !(R_LCDC & 0x04))
      continue;

    VS[GBC_NS].x = (int)obj->x - 8;
    v = line - (int)obj->y + 16;

    if (hw.cgb)
    {
      pat = obj->pat | (((int)obj->flags & 0x60) << 5)
            | (((int)obj->flags & 0x08) << 6);
      VS[GBC_NS].pal = 32 + ((obj->flags & 0x07) << 2);
    }
    else
    {
      pat = obj->pat | (((int)obj->flags & 0x60) << 5);
      VS[GBC_NS].pal = 32 + ((obj->flags & 0x10) >> 2);
    }

    VS[GBC_NS].pri = (obj->flags & 0x80) >> 7;

    if ((R_LCDC & 0x04))
    {
      pat &= ~1;
      if (v >= 8)
      {
        v -= 8;
        pat++;
      }
      if (obj->flags & 0x40) pat ^= 1;
    }

    VS[GBC_NS].pat = pat;
    VS[GBC_NS].v = v;

    if (++GBC_NS == 10) break;
  }

  // Sort sprites
  if (!hw.cgb)
  {
    // not quite optimal but it finally works! 
    for (int i = 0; i < GBC_NS; ++i)
    {
      int l = 0;
      int x = VS[0].x;
      for (int j = 1; j < GBC_NS; ++j)
      {
        if (VS[j].x < x)
        {
          l = j;
          x = VS[j].x;
        }
      }
      ts[i] = VS[l];
      VS[l].x = 160;
    }

    memcpy(VS, ts, sizeof(ts));
  }

  return GBC_NS;
}

static inline void spr_scan(int ns)
{
  byte *src, *dest, *bg, *pri;
  int i, b, x, pal;
  gb_vs_t *vs;
  byte bgdup[256];

  memcpy(bgdup, BUF, 256);

  vs = &VS[ns - 1];

  for (; ns; ns--, vs--)
  {
    pal = vs->pal;
    x = vs->x;

    if (x >= 160 || x <= -8)
      continue;

    src = get_patpix(vs->pat, vs->v);
    dest = BUF;

    if (x < 0)
    {
      src -= x;
      i = 8 + x;
    }
    else
    {
      dest += x;
      if (x > 152) i = 160 - x;
      else i = 8;
    }

    if (vs->pri)
    {
      bg = bgdup + (dest - BUF);
      while (i--)
      {
        b = src[i];
        if (b && !(bg[i] & 3)) dest[i] = pal | b;
      }
    }
    else if (hw.cgb)
    {
      bg = bgdup + (dest - BUF);
      pri = PRI + (dest - BUF);
      while (i--)
      {
        b = src[i];
        if (b && (!pri[i] || !(bg[i] & 3)))
          dest[i] = pal | b;
      }
    }
    else
    {
      while (i--) if (src[i]) dest[i] = pal | src[i];
    }
  }
}

void lcd_reset(bool hard)
{
  if (hard)
  {
    memset(gb_lcd_vbank[0], 0, 8192); // mallocated
    memset(gb_lcd_vbank[1], 0, 8192); // mallocated
    
    memset(&lcd.oam, 0, sizeof(lcd.oam));
    memset(&lcd.pal, 0, sizeof(lcd.pal));
    memset(&lcd.pal, 0, sizeof(lcd.pal));
  }

  memset(BG, 0, sizeof(BG));
  memset(WND, 0, sizeof(WND));
  memset(BUF, 0, sizeof(BUF));
  memset(PRI, 0, sizeof(PRI));
  memset(VS, 0, sizeof(VS));

  WX = WY = WT = WV = 0;
  S = GBC_T = U = V = 0;

  WY = R_WY;
  lcd_rebuildpal();
}

//--------------------------------------------------------------------------------    
uint16_t Yline=0;
uint16_t Ybuf=0;
//--------------------------------------------------------------------------------    

static inline void lcd_renderline()
{
  if (!lcd.out.enabled)
    return;

  if (!(R_LCDC & 0x80))
    return; // should not happen... 

  int SX, SY, SL, GBC_NS;

  SL = R_LY;
  SX = R_SCX;
  SY = (R_SCY + SL) & 0xff;
  S = SX >> 3;
  GBC_T = SY >> 3;
  U = SX & 7;
  V = SY & 7;

  WX = R_WX - 7;
  if (WY > SL || WY < 0 || WY > 143 || WX < -7 || WX > 160 || !(R_LCDC & 0x20))
    WX = 160;
  WT = (SL - WY) >> 3;
  WV = (SL - WY) & 7;

  // Fix for Fushigi no Dungeon - Fuurai no Shiren GB2 and Donkey Kong
  // This is a hack, the real problem is elsewhere
  if (lcd.enable_window_offset_hack && (R_LCDC & 0x20))
  {
    WT %= 12;
  }

  GBC_NS = spr_enum();
  tilebuf();

  if (hw.cgb)
  {
    bg_scan_color();
    wnd_scan_color();
    if (GBC_NS)
    {
      bg_scan_pri();
      wnd_scan_pri();
    }
  }
  else
  {
    bg_scan();
    wnd_scan();
    blendcpy(BUF + WX, BUF + WX, 0x04, 160 - WX);
  }

  spr_scan(GBC_NS);

//--------------------------------------------------------------------------------
#if VGA_OUTPUT
  if (Ybuf>=240) Ybuf=0;
  int cnt = 160;
  un8* dst = (un8*)VGA.m_viewPort[Ybuf];
  un8* dst2;
  if (Ybuf<239) {
    dst2 = (un8*)VGA.m_viewPort[Ybuf+1];
  } else {
    dst2 = (un8*)VGA.m_viewPort[239];
  }
  byte* src = BUF;

  while (cnt) {
    uint8_t reducedCOLOR=GBC_PAL[*(src)];
    uint8_t reducedCOLOR2=0b11000000 | default_6bit[GBC_PAL[*(src++)]];
    uint8_t reducedCOLOR3=0b11000000 | default_6bit[GBC_PAL[*(src++)]];
    uint8_t reducedCOLOR0=0b11000000 | default_6bit[GBC_PAL[*(src++)]];
    uint8_t reducedCOLOR1=0b11000000 | default_6bit[GBC_PAL[*(src++)]];
    
    *(dst++) = reducedCOLOR3;
    *(dst++) = reducedCOLOR3; //2 pixels
    *(dst++) = reducedCOLOR2;
    *(dst++) = reducedCOLOR2; //2 pixels
    *(dst++) = reducedCOLOR1;
    *(dst++) = reducedCOLOR1; //2 pixels
    *(dst++) = reducedCOLOR0;
    *(dst++) = reducedCOLOR0; //2 pixels

    if (Yline%5==0 || Yline%5==2|| Yline%5==4)  {
      *(dst2++) = reducedCOLOR3;
      *(dst2++) = reducedCOLOR3; //every 4 byte is same        
      *(dst2++) = reducedCOLOR2;
      *(dst2++) = reducedCOLOR2; //every 4 byte is same        
      *(dst2++) = reducedCOLOR1;
      *(dst2++) = reducedCOLOR1; //every 4 byte is same        
      *(dst2++) = reducedCOLOR0;
      *(dst2++) = reducedCOLOR0; //every 4 byte is same        
    }
    cnt-=4;
  }
  if (Yline%5==0 || Yline%5==2|| Yline%5==4) Ybuf++;
  Ybuf++;
  Yline=SL;
  Yline++;
  if (Yline>=144) {Yline=0; Ybuf=0;}    
//--------------------------------------------------------------------------------
#else
//--------------------------------------------------------------------------------
//240*240 DRAW
  if (Ybuf>=240) Ybuf=0;
  int cnt = 160;
  un8* dst = SCREENMEMORY_LINE[Ybuf];
  un8* dst2;
  if (Ybuf<239) {
    dst2 = SCREENMEMORY_LINE[Ybuf+1];
  } else {
    dst2 = SCREENMEMORY_LINE[239];
  }
  byte* src = BUF;
  while (cnt--) {

    uint8_t reducedCOLOR=GBC_PAL[*(src)];
      
    *(dst++) = reducedCOLOR;
    if (cnt%2==0) {
      *(dst++) = reducedCOLOR; //every 4 byte is same        
    }  

    if (Yline%5==0 || Yline%5==2|| Yline%5==4)  {
      *(dst2++) = reducedCOLOR;
      if (cnt%2==0) {
        *(dst2++) = reducedCOLOR; //every 4 byte is same        
      }        
    }
    src++;
  }
  if (Yline%5==0 || Yline%5==2|| Yline%5==4) Ybuf++;
  Ybuf++;
  Yline=SL;
  Yline++;
  if (Yline>=144) {Yline=0; Ybuf=0;}
#endif     
//--------------------------------------------------------------------------------
}

static inline void pal_update(byte i)
{
  un32 c = ((un16*)lcd.pal)[i];
  un16 r = c & 0x1f;         // bit 0-4 red 
  un16 g = (c >> 5) & 0x1f;  // bit 5-9 green 
  un16 b = (c >> 10) & 0x1f; // bit 10-14 blue 
  un16 out = (r << 11) | (g << (5 + 1)) | (b); 
  out = (out << 8) | (out >> 8);
  lcd.out.cgb_pal[i] = out;
  out = ((r>>2) << 3) | ((g>>3) << (6)) | (b>>2); 
  GBC_PAL[i]=out; 
}

void pal_write_cgb(byte i, byte b)
{
  if (lcd.pal[i] == b) return;
  lcd.pal[i] = b;
  pal_update(i >> 1);
}

void pal_write_dmg(byte i, byte mapnum, byte d)
{
  un16 *map = lcd.out.dmg_pal[mapnum & 3];

  for (int j = 0; j < 8; j += 2)
  {
    int c = map[(d >> j) & 3];
    // FIXME - handle directly without faking cgb 
    pal_write_cgb(i + j, c & 0xff);
    pal_write_cgb(i + j + 1, c >> 8);
  }
}

void lcd_rebuildpal()
{
  if (!hw.cgb)
  {
    const uint16_t *bgp, *obp0, *obp1;

    int palette = lcd.out.colorize % GB_PALETTE_COUNT;

    if (palette == GB_PALETTE_GBCBIOS && GBC_cart.colorize)
    {
      uint8_t palette = GBC_cart.colorize & 0x1F;
      uint8_t flags = (GBC_cart.colorize & 0xE0) >> 5;

      bgp  = colorization_palettes[palette][2];
      obp0 = colorization_palettes[palette][(flags & 1) ? 0 : 1];
      obp1 = colorization_palettes[palette][(flags & 2) ? 0 : 1];

      if (!(flags & 4)) {
        obp1 = colorization_palettes[palette][2];
      }

      MESSAGE_DEBUG("Using GBC palette %d\n", palette);
    }
    else
    {
      palette = palette ? palette : 1; // This shouldn't happen

      bgp = obp0 = obp1 = custom_palettes[palette];

      MESSAGE_DEBUG("Using Built-in palette %d\n", palette);
    }

    memcpy(&lcd.out.dmg_pal[0], bgp, 8);
    memcpy(&lcd.out.dmg_pal[1], bgp, 8);
    memcpy(&lcd.out.dmg_pal[2], obp0, 8);
    memcpy(&lcd.out.dmg_pal[3], obp1, 8);

    pal_write_dmg(0, 0, R_BGP);
    pal_write_dmg(8, 1, R_BGP);
    pal_write_dmg(64, 2, R_OBP0);
    pal_write_dmg(72, 3, R_OBP1);
  }

  for (int i = 0; i < 64; i++)
  {
    pal_update(i);
  }
}



//   LCD Controller routines



//   lcd_stat_trigger updates the STAT interrupt line to reflect whether any
//   of the conditions set to be tested (by bits 3-6 of R_STAT) are met.
//   This function should be called whenever any of the following occur:
//   1) LY or LYC changes.
//   2) GBC_A state transition affects the low 2 bits of R_STAT (see below).
//   3) The program writes to the upper bits of R_STAT.
//   lcd_stat_trigger also updates bit 2 of R_STAT to reflect whether LY=LYC.

void lcd_stat_trigger()
{
  int condbits[4] = { 0x08, 0x10, 0x20, 0x00 };
  int mask = condbits[R_STAT & 3];

  if (R_LY == R_LYC)
    R_STAT |= 0x04;
  else
    R_STAT &= ~0x04;

  hw_interrupt(IF_STAT, (R_LCDC & 0x80) && ((R_STAT & 0x44) == 0x44 || (R_STAT & mask)));
}



//   stat_change is called when a transition results in a change to the
//   LCD STAT condition (the low 2 bits of R_STAT).  It raises or lowers
//   the VBLANK interrupt line appropriately and calls lcd_stat_trigger to
//   update the STAT interrupt line.
//   FIXME: function now will only lower vblank interrupt, description does not match anymore

static void inline stat_change(int stat)
{
  R_STAT = (R_STAT & 0x7C) | (stat & 3);
  if (stat != 1)
    hw_interrupt(IF_VBLANK, 0);
  lcd_stat_trigger();
}


void lcd_lcdc_change(byte b)
{
  byte old = R_LCDC;
  R_LCDC = b;
  if ((R_LCDC ^ old) & 0x80) // lcd on/off change 
  {
    R_LY = 0;
    stat_change(2);
    CYCLES = 40;  // Correct value seems to be 38
    WY = R_WY;
  }
}



//  LCD controller operates with 154 lines per frame, of which lines
//  #0..#143 are visible and lines #144..#153 are processed in vblank
//  state.
//
//  lcd_emulate() performs cyclic switching between lcdc states (OAM
//  search/data transfer/hblank/vblank), updates system state and time
//  counters accordingly. Control is returned to the caller immediately
//  after a step that sets LCDC ahead of CPU, so that LCDC is always
//  ahead of CPU by one state change. Once CPU reaches same point in
//  time, LCDC is advanced through the next step.
//
//  For each visible line LCDC goes through states 2 (search), 3
//  (transfer) and then 0 (hblank). At the first line of vblank LCDC
//  is switched to state 1 (vblank) and remains there till line #0 is
//  reached (execution is still interrupted after each line so that
//  function could return if it ran out of time).
//
//  Irregardless of state switches per line, time spent in each line
//  adds up to exactly 228 double-speed cycles (109us).
//
//  LCDC emulation begins with R_LCDC set to "operation enabled", R_LY
//  set to line #0 and R_STAT set to state-hblank. lcd.cycles is also
//  set to zero, to begin emulation we call lcd_emulate() once to
//  force-advance LCD through the first iteration.
//
//  Docs aren't entirely accurate about time intervals within single
//  line; besides that, intervals will vary depending on number of
//  sprites on the line and probably other factors. States 1, 2 and 3
//  do not require precise sub-line CPU-LCDC sync, but state 0 might do.

void lcd_emulate()
{
  // LCD disabled 
  if (!(R_LCDC & 0x80))
  {
    // LCDC operation disabled (short route)
    while (CYCLES <= 0)
    {
      switch (R_STAT & 3)
      {
        case 0: // hblank 
        case 1: // vblank 
          // lcd_renderline();
          stat_change(2);
          CYCLES += 40;
          break;
        case 2: // search 
          stat_change(3);
          CYCLES += 86;
          break;
        case 3: // transfer 
          stat_change(0);
          // FIXME: check docs; HDMA might require operating LCDC 
          if (hw.hdma & 0x80)
            hw_hdma();
          else
            CYCLES += 102;
          break;
      }
      return;
    }
  }

  while (CYCLES <= 0)
  {
    switch (R_STAT & 3)
    {
      case 0:
        // hblank -> 
        if (++R_LY >= 144)
        {
          // FIXME: pick _one_ place to trigger vblank interrupt
          //  this better be done here or within stat_change(),
          //  otherwise CPU will have a chance to run for some time
          //  before interrupt is triggered 
          if (GBC_cpu.halted)
          {
            hw_interrupt(IF_VBLANK, 1);
            CYCLES += 228;
          }
          else CYCLES += 10;
          stat_change(1); // -> vblank 
          break;
        }

        // Hack for Worms Armageddon
        if (R_STAT == 0x48)
          hw_interrupt(IF_STAT, 0);

        stat_change(2); // -> search 
        CYCLES += 40;
        break;
      case 1:
        // vblank -> 
        if (!(hw.ilines & IF_VBLANK))
        {
          hw_interrupt(IF_VBLANK, 1);
          CYCLES += 218;
          break;
        }
        if (R_LY == 0)
        {
          WY = R_WY;
          stat_change(2); // -> search 
          CYCLES += 40;
          break;
        }
        else if (R_LY < 152)
          CYCLES += 228;
        else if (R_LY == 152)
          // Handling special case on the last line; see
          // docs/HACKING 
          CYCLES += 28;
        else
        {
          R_LY = -1;
          CYCLES += 200;
        }
        R_LY++;
        lcd_stat_trigger();
        break;
      case 2:
        // search -> 
        lcd_renderline();
        stat_change(3); // -> transfer 
        CYCLES += 86;
        break;
      case 3:
        // transfer -> 
        stat_change(0); // -> hblank 
        if (hw.hdma & 0x80)
          hw_hdma();
        // FIXME -- how much of the hblank does hdma use?? 
        // else 
        CYCLES += 102;
        break;
    }
  }
}

//--------------------------------------------------------------------------------
// SOUND.h
//--------------------------------------------------------------------------------

#include <string.h>

#define MEMCPY memcpy

static const byte dmgwave[16] =
{
  0xac, 0xdd, 0xda, 0x48,
  0x36, 0x02, 0xcf, 0x16,
  0x2c, 0x04, 0xe5, 0x2c,
  0xac, 0xdd, 0xda, 0x48
};

static const byte cgbwave[16] =
{
  0x00, 0xff, 0x00, 0xff,
  0x00, 0xff, 0x00, 0xff,
  0x00, 0xff, 0x00, 0xff,
  0x00, 0xff, 0x00, 0xff,
};

static const byte sqwave[4][8] =
{
  {  0, 0, -1, 0, 0, 0, 0, 0 },
  {  0, -1, -1, 0, 0, 0, 0, 0 },
  { -1, -1, -1, -1, 0, 0, 0, 0 },
  { -1, 0, 0, -1, -1, -1, -1, -1 }
};

static const int freqtab[8] =
{
  (1 << 14) * 2,
  (1 << 14),
  (1 << 14) / 2,
  (1 << 14) / 3,
  (1 << 14) / 4,
  (1 << 14) / 5,
  (1 << 14) / 6,
  (1 << 14) / 7
};


gb_pcm_t pcm;
gb_snd_t snd;

#define RATE (snd.rate)
#define WAVE (snd.wave) // hw.ioregs+0x30 
#define S1 (snd.ch[0])
#define S2 (snd.ch[1])
#define S3 (snd.ch[2])
#define S4 (snd.ch[3])

#define s1_freq() {int d = 2048 - (((R_NR14&7)<<8) + R_NR13); S1.freq = (RATE > (d<<4)) ? 0 : (RATE << 17)/d;}
#define s2_freq() {int d = 2048 - (((R_NR24&7)<<8) + R_NR23); S2.freq = (RATE > (d<<4)) ? 0 : (RATE << 17)/d;}
#define s3_freq() {int d = 2048 - (((R_NR34&7)<<8) + R_NR33); S3.freq = (RATE > (d<<3)) ? 0 : (RATE << 21)/d;}
#define s4_freq() {S4.freq = (freqtab[R_NR43&7] >> (R_NR43 >> 4)) * RATE; if (S4.freq >> 18) S4.freq = 1<<18;}

void sound_dirty()
{
  S1.swlen = ((R_NR10 >> 4) & 7) << 14;
  S1.len = (64 - (R_NR11 & 63)) << 13;
  S1.envol = R_NR12 >> 4;
  S1.endir = (R_NR12 >> 3) & 1;
  S1.endir |= S1.endir - 1;
  S1.enlen = (R_NR12 & 7) << 15;
  s1_freq();
  S2.len = (64 - (R_NR21 & 63)) << 13;
  S2.envol = R_NR22 >> 4;
  S2.endir = (R_NR22 >> 3) & 1;
  S2.endir |= S2.endir - 1;
  S2.enlen = (R_NR22 & 7) << 15;
  s2_freq();
  S3.len = (256 - R_NR31) << 20;
  s3_freq();
  S4.len = (64 - (R_NR41 & 63)) << 13;
  S4.envol = R_NR42 >> 4;
  S4.endir = (R_NR42 >> 3) & 1;
  S4.endir |= S4.endir - 1;
  S4.enlen = (R_NR42 & 7) << 15;
  s4_freq();
}

void sound_off()
{
  memset(&S1, 0, sizeof S1);
  memset(&S2, 0, sizeof S2);
  memset(&S3, 0, sizeof S3);
  memset(&S4, 0, sizeof S4);
  R_NR10 = 0x80;
  R_NR11 = 0xBF;
  R_NR12 = 0xF3;
  R_NR14 = 0xBF;
  R_NR21 = 0x3F;
  R_NR22 = 0x00;
  R_NR24 = 0xBF;
  R_NR30 = 0x7F;
  R_NR31 = 0xFF;
  R_NR32 = 0x9F;
  R_NR33 = 0xBF;
  R_NR41 = 0xFF;
  R_NR42 = 0x00;
  R_NR43 = 0x00;
  R_NR44 = 0xBF;
  R_NR50 = 0x77;
  R_NR51 = 0xF3;
  R_NR52 = 0xF1;
  sound_dirty();
}

void sound_reset(bool hard)
{
  memset(&snd, 0, sizeof snd);
  memcpy(WAVE, hw.cgb ? cgbwave : dmgwave, 16);
  memcpy(hw.ioregs + 0x30, WAVE, 16);
  snd.rate = pcm.hz ? (int)(((1 << 21) / (double)pcm.hz) + 0.5) : 0;
  pcm.pos = 0;
  
  sound_off();
  R_NR52 = 0xF1;
}

void sound_mix()
{
 
  int s, l, r, f, n;

  if (!RATE || snd.cycles < RATE) return;

  for (; snd.cycles >= RATE; snd.cycles -= RATE)
  {
    l = r = 0;

    if (S1.on)
    {
      int s = sqwave[R_NR11 >> 6][(S1.pos >> 18) & 7] & S1.envol;
      S1.pos += S1.freq;

      if ((R_NR14 & 64) && ((S1.cnt += RATE) >= S1.len))
        S1.on = 0;

      if (S1.enlen && (S1.encnt += RATE) >= S1.enlen)
      {
        S1.encnt -= S1.enlen;
        S1.envol += S1.endir;
        if (S1.envol < 0) S1.envol = 0;
        if (S1.envol > 15) S1.envol = 15;
      }

      if (S1.swlen && (S1.swcnt += RATE) >= S1.swlen)
      {
        S1.swcnt -= S1.swlen;
        int f = S1.swfreq;

        if (R_NR10 & 8)
          f -= (f >> (R_NR10 & 7));
        else
          f += (f >> (R_NR10 & 7));

        if (f > 2047)
          S1.on = 0;
        else
        {
          S1.swfreq = f;
          R_NR13 = f;
          R_NR14 = (R_NR14 & 0xF8) | (f >> 8);
          s1_freq();
        }
      }
      s <<= 2;
      if (R_NR51 & 1) r += s;
      if (R_NR51 & 16) l += s;
    }

    if (S2.on)
    {
      s = sqwave[R_NR21 >> 6][(S2.pos >> 18) & 7] & S2.envol;
      S2.pos += S2.freq;
      if ((R_NR24 & 64) && ((S2.cnt += RATE) >= S2.len))
        S2.on = 0;
      if (S2.enlen && (S2.encnt += RATE) >= S2.enlen)
      {
        S2.encnt -= S2.enlen;
        S2.envol += S2.endir;
        if (S2.envol < 0) S2.envol = 0;
        if (S2.envol > 15) S2.envol = 15;
      }
      s <<= 2;
      if (R_NR51 & 2) r += s;
      if (R_NR51 & 32) l += s;
    }

    if (S3.on)
    {
      s = WAVE[(S3.pos >> 22) & 15];
      if (S3.pos & (1 << 21)) s &= 15;
      else s >>= 4;
      s -= 8;
      S3.pos += S3.freq;
      if ((R_NR34 & 64) && ((S3.cnt += RATE) >= S3.len))
        S3.on = 0;
      if (R_NR32 & 96) s <<= (3 - ((R_NR32 >> 5) & 3));
      else s = 0;
      if (R_NR51 & 4) r += s;
      if (R_NR51 & 64) l += s;
    }

    if (S4.on)
    {
      if (R_NR43 & 8) s = 1 & (noise7[
                                   (S4.pos >> 20) & 15] >> (7 - ((S4.pos >> 17) & 7)));
      else s = 1 & (noise15[
                        (S4.pos >> 20) & 4095] >> (7 - ((S4.pos >> 17) & 7)));
      s = (-s) & S4.envol;
      S4.pos += S4.freq;
      if ((R_NR44 & 64) && ((S4.cnt += RATE) >= S4.len))
        S4.on = 0;
      if (S4.enlen && (S4.encnt += RATE) >= S4.enlen)
      {
        S4.encnt -= S4.enlen;
        S4.envol += S4.endir;
        if (S4.envol < 0) S4.envol = 0;
        if (S4.envol > 15) S4.envol = 15;
      }
      s += s << 1;
      if (R_NR51 & 8) r += s;
      if (R_NR51 & 128) l += s;
    }

    l *= (R_NR50 & 0x07);
    r *= ((R_NR50 & 0x70) >> 4);

    if (pcm.buf) {
      if ((pcm.stereo && pcm.pos >= pcm.len-2) || (!pcm.stereo && pcm.pos >= pcm.len-1))  pcm_submit();
      if ((pcm.stereo && pcm.pos < pcm.len-2) || (!pcm.stereo && pcm.pos < pcm.len-1)) {
        if (pcm.stereo) {       
          pcm.buf[pcm.pos++] = l<<2; 
          pcm.buf[pcm.pos++] = r<<2; 
        } else pcm.buf[pcm.pos++] = (((l + r) >> 1) + 128) <<2;
      }  
    }   
  }
  R_NR52 = (R_NR52 & 0xf0) | S1.on | (S2.on << 1) | (S3.on << 2) | (S4.on << 3);
}



byte sound_read(byte r)
{
  sound_mix();
  // printf("read %02X: %02X\n", r, REG(r)); 
  return REG(r);
}

static inline void s1_init()
{
  S1.swcnt = 0;
  S1.swfreq = ((R_NR14 & 7) << 8) + R_NR13;
  S1.envol = R_NR12 >> 4;
  S1.endir = (R_NR12 >> 3) & 1;
  S1.endir |= S1.endir - 1;
  S1.enlen = (R_NR12 & 7) << 15;
  if (!S1.on) S1.pos = 0;
  S1.on = 1;
  S1.cnt = 0;
  S1.encnt = 0;
}

static inline void s2_init()
{
  S2.envol = R_NR22 >> 4;
  S2.endir = (R_NR22 >> 3) & 1;
  S2.endir |= S2.endir - 1;
  S2.enlen = (R_NR22 & 7) << 15;
  if (!S2.on) S2.pos = 0;
  S2.on = 1;
  S2.cnt = 0;
  S2.encnt = 0;
}

static inline void s3_init()
{
  if (!S3.on) S3.pos = 0;
  S3.cnt = 0;
  S3.on = R_NR30 >> 7;
  if (!S3.on) return;
  for (int i = 0; i < 16; i++)
    hw.ioregs[i + 0x30] = 0x13 ^ hw.ioregs[i + 0x31];
}

static inline void s4_init()
{
  S4.envol = R_NR42 >> 4;
  S4.endir = (R_NR42 >> 3) & 1;
  S4.endir |= S4.endir - 1;
  S4.enlen = (R_NR42 & 7) << 15;
  S4.on = 1;
  S4.pos = 0;
  S4.cnt = 0;
  S4.encnt = 0;
}


void sound_write(byte r, byte b)
{
#if 0
  static void *timer;
  if (!timer) timer = sys_timer();
  printf("write %02X: %02X @ %d\n", r, b, sys_elapsed(timer));
#endif

  if (!(R_NR52 & 128) && r != RI_NR52) return;
  if ((r & 0xF0) == 0x30)
  {
    if (S3.on) sound_mix();
    if (!S3.on)
      WAVE[r - 0x30] = hw.ioregs[r] = b;
    return;
  }
  sound_mix();
  switch (r)
  {
    case RI_NR10:
      R_NR10 = b;
      S1.swlen = ((R_NR10 >> 4) & 7) << 14;
      S1.swfreq = ((R_NR14 & 7) << 8) + R_NR13;
      break;
    case RI_NR11:
      R_NR11 = b;
      S1.len = (64 - (R_NR11 & 63)) << 13;
      break;
    case RI_NR12:
      R_NR12 = b;
      S1.envol = R_NR12 >> 4;
      S1.endir = (R_NR12 >> 3) & 1;
      S1.endir |= S1.endir - 1;
      S1.enlen = (R_NR12 & 7) << 15;
      break;
    case RI_NR13:
      R_NR13 = b;
      s1_freq();
      break;
    case RI_NR14:
      R_NR14 = b;
      s1_freq();
      if (b & 128) s1_init();
      break;
    case RI_NR21:
      R_NR21 = b;
      S2.len = (64 - (R_NR21 & 63)) << 13;
      break;
    case RI_NR22:
      R_NR22 = b;
      S2.envol = R_NR22 >> 4;
      S2.endir = (R_NR22 >> 3) & 1;
      S2.endir |= S2.endir - 1;
      S2.enlen = (R_NR22 & 7) << 15;
      break;
    case RI_NR23:
      R_NR23 = b;
      s2_freq();
      break;
    case RI_NR24:
      R_NR24 = b;
      s2_freq();
      if (b & 128) s2_init();
      break;
    case RI_NR30:
      R_NR30 = b;
      if (!(b & 128)) S3.on = 0;
      break;
    case RI_NR31:
      R_NR31 = b;
      S3.len = (256 - R_NR31) << 13;
      break;
    case RI_NR32:
      R_NR32 = b;
      break;
    case RI_NR33:
      R_NR33 = b;
      s3_freq();
      break;
    case RI_NR34:
      R_NR34 = b;
      s3_freq();
      if (b & 128) s3_init();
      break;
    case RI_NR41:
      R_NR41 = b;
      S4.len = (64 - (R_NR41 & 63)) << 13;
      break;
    case RI_NR42:
      R_NR42 = b;
      S4.envol = R_NR42 >> 4;
      S4.endir = (R_NR42 >> 3) & 1;
      S4.endir |= S4.endir - 1;
      S4.enlen = (R_NR42 & 7) << 15;
      break;
    case RI_NR43:
      R_NR43 = b;
      s4_freq();
      break;
    case RI_NR44:
      R_NR44 = b;
      if (b & 128) s4_init();
      break;
    case RI_NR50:
      R_NR50 = b;
      break;
    case RI_NR51:
      R_NR51 = b;
      break;
    case RI_NR52:
      R_NR52 = b;
      if (!(R_NR52 & 128))
        sound_off();
      break;
    default:
      return;
  }
}

//--------------------------------------------------------------------------------
// SAVE.c
//--------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

#ifdef IS_LITTLE_ENDIAN
#define LIL(x) (x)
#else
#define LIL(x) ((x<<24)|((x&0xff00)<<8)|((x>>8)&0xff00)|(x>>24))
#endif

#define SAVE_VERSION 0x107

#define wavofs  (4096 - 784)
#define hiofs   (4096 - 768)
#define palofs  (4096 - 512)
#define oamofs  (4096 - 256)

#define I1(s, p) { 1, s, p }
#define I2(s, p) { 2, s, p }
#define I4(s, p) { 4, s, p }
#define GBC_END { 0, "\0\0\0\0", 0 }

typedef struct
{
  int len;
  char key[5]; 
  void *ptr;
} svar_t;

typedef struct
{
  void *ptr;
  int len;
} sblock_t;

static un32 ver;

static const svar_t svars[] =
{
  I4("GbSs", &ver),

  I2("PC  ", &GBC_PC),
  I2("SP  ", &GBC_SP),
  I2("BC  ", &BC),
  I2("DE  ", &DE),
  I2("HL  ", &HL),
  I2("AF  ", &AF),

  I4("IME ", &GBC_cpu.ime),
  I4("ima ", &GBC_cpu.ima),
  I4("spd ", &GBC_cpu.double_speed),
  I4("halt", &GBC_cpu.halted),
  I4("div ", &GBC_cpu.div),
  I4("tim ", &GBC_cpu.timer),
  I4("lcdc", &lcd.cycles),
  I4("snd ", &snd.cycles),

  I4("ints", &hw.ilines),
  I4("pad ", &hw.pad),
  I4("cgb ", &hw.cgb),
  I4("hdma", &hw.hdma),
  I4("seri", &hw.serial),

  I4("mbcm", &GBC_cart.bankmode),
  I4("romb", &GBC_cart.rombank),
  I4("ramb", &GBC_cart.rambank),
  I4("enab", &GBC_cart.enableram),
  // I4("batt", &GBC_cart.batt),

  I4("rtcR", &rtc.sel),
  I4("rtcL", &rtc.latch),
  I4("rtcF", &rtc.flags),
  I4("rtcd", &rtc.d),
  I4("rtch", &rtc.h),
  I4("rtcm", &rtc.m),
  I4("rtcs", &rtc.s),
  I4("rtct", &rtc.ticks),
  I1("rtR8", &rtc.regs[0]),
  I1("rtR9", &rtc.regs[1]),
  I1("rtRA", &rtc.regs[2]),
  I1("rtRB", &rtc.regs[3]),
  I1("rtRC", &rtc.regs[4]),

  I4("S1on", &snd.ch[0].on),
  I4("S1p ", &snd.ch[0].pos),
  I4("S1c ", &snd.ch[0].cnt),
  I4("S1ec", &snd.ch[0].encnt),
  I4("S1sc", &snd.ch[0].swcnt),
  I4("S1sf", &snd.ch[0].swfreq),

  I4("S2on", &snd.ch[1].on),
  I4("S2p ", &snd.ch[1].pos),
  I4("S2c ", &snd.ch[1].cnt),
  I4("S2ec", &snd.ch[1].encnt),

  I4("S3on", &snd.ch[2].on),
  I4("S3p ", &snd.ch[2].pos),
  I4("S3c ", &snd.ch[2].cnt),

  I4("S4on", &snd.ch[3].on),
  I4("S4p ", &snd.ch[3].pos),
  I4("S4c ", &snd.ch[3].cnt),
  I4("S4ec", &snd.ch[3].encnt),

  GBC_END
};



int sram_load(const char *file)
{
 
}


int sram_save(const char *file)
{
 
}


int sram_update(const char *file)
{
  if (!GBC_cart.has_battery || !GBC_cart.ramsize || !file || !*file)
    return -1;

  return -9000;
#if 0
  FILE *fp = fopen(file, "wb");
  if (!fp)
  {
    MESSAGE_ERROR("Unable to open SRAM file: %s", file);
    goto _cleanup;
  }

  for (int pos = 0; pos < (GBC_cart.ramsize * 8192); pos += SRAM_SECTOR_SIZE)
  {
    int sector = pos / SRAM_SECTOR_SIZE;

    if (GBC_cart.sram_dirty_sector[sector])
    {
      // MESSAGE_INFO("Writing sram sector #%d @ %ld\n", sector, pos);

      if (fseek(fp, pos, SEEK_SET) != 0)
      {
        MESSAGE_ERROR("Failed to seek sram sector #%d\n", sector);
        goto _cleanup;
      }

      if (fwrite(&GBC_cart.sram[pos], SRAM_SECTOR_SIZE, 1, fp) != 1)
      {
        MESSAGE_ERROR("Failed to write sram sector #%d\n", sector);
        goto _cleanup;
      }

      GBC_cart.sram_dirty_sector[sector] = 0;
    }
  }

  if (fseek(fp, GBC_cart.ramsize * 8192, SEEK_SET) == 0)
  {
    rtc_save(fp);
  }

_cleanup:
  // Keeping the file open between calls is dangerous unfortunately

  // if (GBC_cart.romsize < 64)
  // {
  //  fflush(GBC_cart.sramFile);
  // }
  // else

  if (fclose(fp) == 0)
  {
    GBC_cart.sram_dirty = 0;
  }
  return 0;
#endif
}


//--------------------------------------------------------------------------------
// GNUBOY.c
//--------------------------------------------------------------------------------

#include <sys/param.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int gnuboy_init(void) {
  gnuboy_reset(true);
  return 0;
}

//  gnuboy_reset is called to initialize the state of the emulated
//  system. It should set GBC_cpu registers, hardware registers, etc. to
//  their appropriate values at power up time.

void gnuboy_reset(bool hard)
{
  hw_reset(hard);
  rtc_reset(hard);
  lcd_reset(hard);
  cpu_reset(hard);
  sound_reset(hard);
}



//  Time intervals throughout the code, unless otherwise noted, are
//  specified in double-speed machine cycles (2MHz), each unit
//  roughly corresponds to 0.477us.
// 
//  For CPU each cycle takes 2dsc (0.954us) in single-speed mode
//  and 1dsc (0.477us) in double speed mode.
// 
//  Although hardware gbc LCDC would operate at completely different
//  and fixed frequency, for emulation purposes timings for it are
//  also specified in double-speed cycles.
// 
//  line = 228 dsc (109us)
//  frame (154 lines) = 35112 dsc (16.7ms)
//  of which
//    visible lines x144 = 32832 dsc (15.66ms)
//    vblank lines x10 = 2280 dsc (1.08ms)


static int framelen = 16743;
static int framecount = 0;
extern bool emuquit;
extern void *sys_timer();

void gnuboy_run(bool draw)
{
  int delay_;

  lcd.out.enabled = draw;
  for (;;)
  {
    cpu_emulate(2280);
    while (R_LY > 0 && R_LY < 144)
      cpu_emulate(lcd.cycles);

    rtc_tick();
    sound_mix();

    if (AUDIOREADY == 0) AUDIOREADY = 1; //  
  
    ev_poll();
    if (framecount) {
      if (!--framecount) die("finished\n");
    }

    if (!(R_LCDC & 0x80))
      cpu_emulate(32832);

    while (R_LY > 0) // wait for next frame 
      cpu_emulate(lcd.cycles);
  }
}


void gnuboy_die(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  abort();
  //RG_PANIC(fmt); // Lazy
}


int gnuboy_load_bios(const char *file)
{
  MESSAGE_INFO("Loading BIOS file: '%s'\n", file);
  return 0;
}


void gnuboy_load_bank(int bank)
{
  const size_t BANK_SIZE = 0x4000;
  const size_t OFFSET = bank * BANK_SIZE;

  MESSAGE_DEBUG("loading bank %d.\n", bank);
//--------------------------------------------------------------------------------
  if (GBC_IN_HIMEM==0) {
#ifndef NCAT_NO_PSRAM
    GBC_cart.rombanks[bank]=&DATA[OFFSET];
#else 
    printf("Redirecting FLASH data for GBC EMULATOR...");
    GBC_cart.rombanks[bank]=DATA+OFFSET;
    printf("[ OK ]\n");
#endif  
  } else {
//--------------------------------------------------------------------------------
    if (bank==0) {
      if (GBC_HIMEM_BANK0_BUFFER_LOADED==0) {      
        GBC_HIMEM_BANK0_BUFFER=(uint8_t*)ps_malloc(16384);
//----------------------------------------
        if (!GBC_HIMEM_BANK_BUFFER_ALLOCATED) {
          GBC_HIMEM_BANK_BUFFER=(uint8_t*)malloc(4096);        
          GBC_HIMEM_BANK_BUFFER_ALLOCATED=1;
        }
        printf(">>> HIMEM READ BANK: %d \n",bank);
        memset(GBC_HIMEM_BANK_BUFFER,0,4096); //NCX: important clear buffer
        himem_read(OFFSET,4096,GBC_HIMEM_BANK_BUFFER); //read from zero address
        memcpy(&GBC_HIMEM_BANK0_BUFFER[0],GBC_HIMEM_BANK_BUFFER,4096);
        memset(GBC_HIMEM_BANK_BUFFER,0,4096); //NCX: important clear buffer
        himem_read(OFFSET+4096*1,4096,GBC_HIMEM_BANK_BUFFER); //read from zero address
        memcpy(&GBC_HIMEM_BANK0_BUFFER[4096*1],GBC_HIMEM_BANK_BUFFER,4096);
        memset(GBC_HIMEM_BANK_BUFFER,0,4096); //NCX: important clear buffer
        himem_read(OFFSET+4096*2,4096,GBC_HIMEM_BANK_BUFFER); //read from zero address
        memcpy(&GBC_HIMEM_BANK0_BUFFER[4096*2],GBC_HIMEM_BANK_BUFFER,4096);
        memset(GBC_HIMEM_BANK_BUFFER,0,4096); //NCX: important clear buffer
        himem_read(OFFSET+4096*3,4096,GBC_HIMEM_BANK_BUFFER); //read from zero address
        memcpy(&GBC_HIMEM_BANK0_BUFFER[4096*3],GBC_HIMEM_BANK_BUFFER,4096);
//----------------------------------------
        GBC_HIMEM_BANK0_BUFFER_LOADED=1;
      }  
      GBC_cart.rombanks[0]=GBC_HIMEM_BANK0_BUFFER; //MAIN ZERO ROM BANK in HIMEM
      return;
    } else {
      int tmp=0;
      for (tmp=0;tmp<64;tmp++) { //check if we have mirrored ROM BANK
        if (GBC_HIMEM_BANK_SELECTED[tmp]==bank) {
          GBC_cart.rombanks[bank]=GBC_HIMEM_BANK[tmp];
          return;
        }   
      }
      if (tmp==64) { //bank is not mirrored so create new mirror
        printf("CREATING NEW BANK %d at POSITION: %d\n",bank,GBC_HIMEM_BANK_UPDATE_POSITION);
//----------------------------------------
        if (!GBC_HIMEM_BANK_BUFFER_ALLOCATED) {
          GBC_HIMEM_BANK_BUFFER=(uint8_t*)malloc(16384);  
          GBC_HIMEM_BANK_BUFFER_ALLOCATED=1;
        }
        printf(">>> HIMEM READ BANK: %d \n",bank);
        //remove previous pointer of rombank  
        if (GBC_HIMEM_BANK_SELECTED[GBC_HIMEM_BANK_UPDATE_POSITION]>0) {
          printf("---> free ROMBANK: %d \n",GBC_HIMEM_BANK_SELECTED[GBC_HIMEM_BANK_UPDATE_POSITION]);
          GBC_cart.rombanks[GBC_HIMEM_BANK_SELECTED[GBC_HIMEM_BANK_UPDATE_POSITION]]=NULL;
        }
        GBC_HIMEM_BANK_SELECTED[GBC_HIMEM_BANK_UPDATE_POSITION]=bank;
        printf("---> alloc ROMBANK: %d \n",GBC_HIMEM_BANK_SELECTED[GBC_HIMEM_BANK_UPDATE_POSITION]);

        memset(GBC_HIMEM_BANK_BUFFER,0,4096); //NCX: important clear buffer
        himem_read(OFFSET,4096,GBC_HIMEM_BANK_BUFFER); //read from zero address
        memcpy(&GBC_HIMEM_BANK[GBC_HIMEM_BANK_UPDATE_POSITION][0],GBC_HIMEM_BANK_BUFFER,4096);

        memset(GBC_HIMEM_BANK_BUFFER,0,4096); //NCX: important clear buffer
        himem_read(OFFSET+4096*1,4096,GBC_HIMEM_BANK_BUFFER); //read from zero address
        memcpy(&GBC_HIMEM_BANK[GBC_HIMEM_BANK_UPDATE_POSITION][4096*1],GBC_HIMEM_BANK_BUFFER,4096);

        memset(GBC_HIMEM_BANK_BUFFER,0,4096); //NCX: important clear buffer
        himem_read(OFFSET+4096*2,4096,GBC_HIMEM_BANK_BUFFER); //read from zero address
        memcpy(&GBC_HIMEM_BANK[GBC_HIMEM_BANK_UPDATE_POSITION][4096*2],GBC_HIMEM_BANK_BUFFER,4096);

        memset(GBC_HIMEM_BANK_BUFFER,0,4096); //NCX: important clear buffer
        himem_read(OFFSET+4096*3,4096,GBC_HIMEM_BANK_BUFFER); //read from zero address
        memcpy(&GBC_HIMEM_BANK[GBC_HIMEM_BANK_UPDATE_POSITION][4096*3],GBC_HIMEM_BANK_BUFFER,4096);
//----------------------------------------
        //add new pointer of rombank
        GBC_cart.rombanks[bank]=GBC_HIMEM_BANK[GBC_HIMEM_BANK_UPDATE_POSITION];
        GBC_HIMEM_BANK_UPDATE_POSITION++;
        if (GBC_HIMEM_BANK_UPDATE_POSITION>=64) GBC_HIMEM_BANK_UPDATE_POSITION=0;
      }
    }
//--------------------------------------------------------------------------------
  }
}

void gnuboy_free_rom(void)
{
  for (int i = 0; i < 512; i++)
  {
    if (GBC_cart.rombanks[i]) {
      //free(GBC_cart.rombanks[i]);
      GBC_cart.rombanks[i] = NULL;
    }
  }
  free(GBC_cart.rambanks);
  GBC_cart.rambanks = NULL;

  memset(&GBC_cart, 0, sizeof(GBC_cart));
}

void gnuboy_get_time(int *day, int *hour, int *minute, int *second)
{
  if (day) *day = rtc.d;
  if (hour) *hour = rtc.h;
  if (minute) *minute = rtc.m;
  if (second) *second = rtc.s;
}


void gnuboy_set_time(int day, int hour, int minute, int second)
{
  rtc.d = MIN(MAX(day, 0), 365);
  rtc.h = MIN(MAX(hour, 0), 24);
  rtc.m = MIN(MAX(minute, 0), 60);
  rtc.s = MIN(MAX(second, 0), 60);
  rtc.ticks = 0;
  rtc.dirty = 0;
}


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

int gnuboy_load_rom(const char *GBC_DATA)
{
  byte header[0x200];
  if (GBC_IN_HIMEM==1) {
    gnuboy_load_bank(0);
    memcpy(header,GBC_cart.rombanks[0],0x200);
  } else {
    memcpy(header,&GBC_DATA[0],0x200);
  }  
  // Memory Bank Controller names
  const char *mbc_names[16] = {
    "MBC_NONE", "MBC_MBC1", "MBC_MBC2", "MBC_MBC3",
    "MBC_MBC5", "MBC_MBC6", "MBC_MBC7", "MBC_HUC1",
    "MBC_HUC3", "MBC_MMM01", "INVALID", "INVALID",
    "INVALID", "INVALID", "INVALID", "INVALID",
  };

  int type = header[0x0147];
  int romsize = header[0x0148];
  int ramsize = header[0x0149];

  hw.cgb = (header[0x0143] == 0x80 || header[0x0143] == 0xC0);

  memcpy(&GBC_cart.checksum, header + 0x014E, 2);
  memcpy(&GBC_cart.name, header + 0x0134, 16);
  GBC_cart.name[16] = 0;

  GBC_cart.has_battery = (type == 3 || type == 6 || type == 9 || type == 13 || type == 15 ||
                      type == 16 || type == 19 || type == 27 || type == 30 || type == 255);
  GBC_cart.has_rtc  = (type == 15 || type == 16);
  GBC_cart.has_rumble = (type == 28 || type == 29 || type == 30);
  GBC_cart.has_sensor = (type == 34);
  GBC_cart.colorize = 0;

  if (type >= 1 && type <= 3)
    GBC_cart.mbc = MBC_MBC1;
  else if (type >= 5 && type <= 6)
    GBC_cart.mbc = MBC_MBC2;
  else if (type >= 11 && type <= 13)
    GBC_cart.mbc = MBC_MMM01;
  else if (type >= 15 && type <= 19)
    GBC_cart.mbc = MBC_MBC3;
  else if (type >= 25 && type <= 30)
    GBC_cart.mbc = MBC_MBC5;
  else if (type == 32)
    GBC_cart.mbc = MBC_MBC6;
  else if (type == 34)
    GBC_cart.mbc = MBC_MBC7;
  else if (type == 254)
    GBC_cart.mbc = MBC_HUC3;
  else if (type == 255)
    GBC_cart.mbc = MBC_HUC1;
  else
    GBC_cart.mbc = MBC_NONE;

  if (romsize < 9)
  {
    GBC_cart.romsize = (2 << romsize);
  }
  else if (romsize > 0x51 && romsize < 0x55)
  {
    GBC_cart.romsize = 128; // (2 << romsize) + 64;
  }
  else
  {
    gnuboy_die("Invalid ROM size: %d\n", romsize);
  }

  if (ramsize < 6)
  {
    const byte ramsize_table[] = {1, 1, 1, 4, 16, 8};
    GBC_cart.ramsize = ramsize_table[ramsize];
  }
  else
  {
    MESSAGE_ERROR("Invalid RAM size: %d\n", ramsize);
    GBC_cart.ramsize = 1;
  }

#ifndef NCAT_NO_PSRAM
  GBC_cart.rambanks = (byte (*)[8192])ps_malloc(8192 * GBC_cart.ramsize); 
#else 
  GBC_cart.rambanks = (byte (*)[8192])malloc(8192 * GBC_cart.ramsize); 
#endif
  
  if (!GBC_cart.rambanks)
  {
    gnuboy_die("SRAM alloc failed");
  }

  // Detect colorization palette that the real GBC would be using
  if (!hw.cgb)
  {
    //
    // The following algorithm was adapted from visualboyadvance-m at
    // https://github.com/visualboyadvance-m/visualboyadvance-m/blob/master/src/gb/GB.cpp
    //

    // Title checksums that are treated specially by the CGB boot ROM
    const uint8_t col_checksum[79] = {
      0x00, 0x88, 0x16, 0x36, 0xD1, 0xDB, 0xF2, 0x3C, 0x8C, 0x92, 0x3D, 0x5C,
      0x58, 0xC9, 0x3E, 0x70, 0x1D, 0x59, 0x69, 0x19, 0x35, 0xA8, 0x14, 0xAA,
      0x75, 0x95, 0x99, 0x34, 0x6F, 0x15, 0xFF, 0x97, 0x4B, 0x90, 0x17, 0x10,
      0x39, 0xF7, 0xF6, 0xA2, 0x49, 0x4E, 0x43, 0x68, 0xE0, 0x8B, 0xF0, 0xCE,
      0x0C, 0x29, 0xE8, 0xB7, 0x86, 0x9A, 0x52, 0x01, 0x9D, 0x71, 0x9C, 0xBD,
      0x5D, 0x6D, 0x67, 0x3F, 0x6B, 0xB3, 0x46, 0x28, 0xA5, 0xC6, 0xD3, 0x27,
      0x61, 0x18, 0x66, 0x6A, 0xBF, 0x0D, 0xF4
    };

    // The fourth character of the game title for disambiguation on collision.
    const uint8_t col_disambig_chars[29] = {
      'GBC_B', 'GBC_E', 'GBC_F', 'GBC_A', 'GBC_A', 'R', 'GBC_B', 'GBC_E',
      'K', 'GBC_E', 'K', ' ', 'R', '-', 'U', 'R',
      'GBC_A', 'R', ' ', 'I', 'N', 'GBC_A', 'I', 'GBC_L',
      'I', 'GBC_C', 'GBC_E', ' ', 'R'
    };

    // Palette ID | (Flags << 5)
    const uint8_t col_palette_info[94] = {
      0x7C, 0x08, 0x12, 0xA3, 0xA2, 0x07, 0x87, 0x4B, 0x20, 0x12, 0x65, 0xA8,
      0x16, 0xA9, 0x86, 0xB1, 0x68, 0xA0, 0x87, 0x66, 0x12, 0xA1, 0x30, 0x3C,
      0x12, 0x85, 0x12, 0x64, 0x1B, 0x07, 0x06, 0x6F, 0x6E, 0x6E, 0xAE, 0xAF,
      0x6F, 0xB2, 0xAF, 0xB2, 0xA8, 0xAB, 0x6F, 0xAF, 0x86, 0xAE, 0xA2, 0xA2,
      0x12, 0xAF, 0x13, 0x12, 0xA1, 0x6E, 0xAF, 0xAF, 0xAD, 0x06, 0x4C, 0x6E,
      0xAF, 0xAF, 0x12, 0x7C, 0xAC, 0xA8, 0x6A, 0x6E, 0x13, 0xA0, 0x2D, 0xA8,
      0x2B, 0xAC, 0x64, 0xAC, 0x6D, 0x87, 0xBC, 0x60, 0xB4, 0x13, 0x72, 0x7C,
      0xB5, 0xAE, 0xAE, 0x7C, 0x7C, 0x65, 0xA2, 0x6C, 0x64, 0x85
    };

    uint8_t infoIdx = 0;
    uint8_t checksum = 0;

    // Calculate the checksum over 16 title bytes.
    for (int i = 0; i < 16; i++)
    {
      checksum += header[0x0134 + i];
    }

    // Check if the checksum is in the list.
    for (size_t idx = 0; idx < 79; idx++)
    {
      if (col_checksum[idx] == checksum)
      {
        infoIdx = idx;

        // Indexes above 0x40 have to be disambiguated.
        if (idx <= 0x40)
          break;

        // No idea how that works. But it works.
        for (size_t i = idx - 0x41, j = 0; i < 29; i += 14, j += 14) {
          if (header[0x0137] == col_disambig_chars[i]) {
            infoIdx += j;
            break;
          }
        }
        break;
      }
    }

    GBC_cart.colorize = col_palette_info[infoIdx];
  }
  MESSAGE_INFO("Cart loaded: name='%s', cgb=%d, mbc=%s, romsize=%dK, ramsize=%dK, colorize=%d\n",
             GBC_cart.name, hw.cgb, mbc_names[GBC_cart.mbc], GBC_cart.romsize * 16, GBC_cart.ramsize * 8, GBC_cart.colorize);

  // Gameboy color games can be very large so we only load 1024K for faster boot
  // Also 4/8MB games do not fully fit, our bank manager takes care of swapping.

  int preload = GBC_cart.romsize < 64 ? GBC_cart.romsize : 64;

  MESSAGE_INFO("Preloading the first %d banks\n", preload);
  for (int i = 0; i < preload; i++)
  {
    gnuboy_load_bank(i);
  }

  // Apply game-specific hacks
  if (strncmp(GBC_cart.name, "SIREN GB2 ", 11) == 0 || strncmp(GBC_cart.name, "DONKEY KONG", 11) == 0)
  {
    MESSAGE_INFO("HACK: Window offset hack enabled\n");
    lcd.enable_window_offset_hack = 1;
  }
  return 0;
}

#endif
