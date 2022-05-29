#pragma GCC optimize ("O3")

#include <string.h>

#include "gnuboy.h"
#include "defs.h"
#include "regs.h"
#include "hw.h"
#include "mem.h"
#include "lcd.h"
#include "rc.h"
#include "fb.h"
#ifdef USE_ASM
#include "asm.h"
#endif

extern int done_drawing;

#include <stdlib.h>
#include <esp_attr.h>
#include <stdint.h>


///struct lcd lcd;
///struct scan scan;

#define BG (GBC_scan->bg)
#define WND (GBC_scan->wnd)
#define BUF (GBC_scan->buf)
#define PRI (GBC_scan->pri)

#define PAL2 (GBC_scan->pal2)

#define VS (GBC_scan->vs) /* vissprites */
#define GBC_NS (GBC_scan->ns)

#define L_GBC (GBC_scan->l) /* line */
#define GBC_X (GBC_scan->x) /* screen position */
#define GBC_Y (GBC_scan->y)
#define GBC_S (GBC_scan->s) /* tilemap position */
#define GBC_T (GBC_scan->t)
#define GBC_U (GBC_scan->u) /* position within tile */
#define GBC_V (GBC_scan->v)
#define WX (GBC_scan->wx)
#define WY (GBC_scan->wy)
#define WT (GBC_scan->wt)
#define WV (GBC_scan->wv)


static int sprsort = 1;
static int sprdebug = 0;

// BGR
#if 0
// Testing/Debug palette
static int dmg_pal[4][4] = {{0xffffff, 0x808080, 0x404040, 0x000000},
              {0xff0000, 0x800000, 0x400000, 0x000000},
              {0x00ff00, 0x008000, 0x004000, 0x000000},
              {0x0000ff, 0x000080, 0x000040, 0x000000} };
#else
#define GB_DEFAULT_PALETTE { 0xd5f3ef, 0x7ab6a3, 0x3b6137, 0x161c04 }
static int dmg_pal[4][4] = {GB_DEFAULT_PALETTE,
              GB_DEFAULT_PALETTE,
              GB_DEFAULT_PALETTE,
              GB_DEFAULT_PALETTE };
#endif

byte *vdest;

//#ifdef ALLOW_UNALIGNED_IO /* long long is ok since this is i386-only anyway? */
#define MEMCPY8(d, s) ((*(long long *)(d)) = (*(long long *)(s)))
//#else
//#define MEMCPY8(d, s) memcpy((d), (s), 8)
//#endif

static byte pix[8];

__attribute__((optimize("unroll-loops")))

///IRAM_ATTR
static const byte*  get_patpix(int i, int x)
{
  const int index = i & 0x3ff; // 1024 entries
  const int rotation = i >> 10; // / 1024;

  int j;
  int a, c;

const byte* const vram = lcd_vbank[0];

  switch (rotation)
  {
    case 0:
      a = ((index << 4) | (x << 1));

      for (byte k = 0; k < 8; k++)
      {
        c = vram[a] & (1 << k) ? 1 : 0;
        c |= vram[a+1] & (1 << k) ? 2 : 0;
        pix[7 - k] = c;
      }
      break;

    case 1:
      a = ((index << 4) | (x << 1));

      for (byte k = 0; k < 8; k++)
      {
        c = vram[a] & (1 << k) ? 1 : 0;
        c |= vram[a+1] & (1 << k) ? 2 : 0;
        pix[k] = c;
      }
      break;

    case 2:
      j = 7 - x;
      a = ((index << 4) | (j << 1));

      for (byte k = 0; k < 8; k++)
      {
        c = vram[a] & (1 << k) ? 1 : 0;
        c |= vram[a+1] & (1 << k) ? 2 : 0;
        pix[7 - k] = c;
      }
      break;

    case 3:
      j = 7 - x;
      a = ((index << 4) | (j << 1));

      for (byte k = 0; k < 8; k++)
      {
        c = vram[a] & (1 << k) ? 1 : 0;
        c |= vram[a+1] & (1 << k) ? 2 : 0;
        pix[k] = c;
      }
      break;
  }

  return pix;
}


#ifndef ASM_UPDATEPATPIX
inline void updatepatpix()
{
}
#endif /* ASM_UPDATEPATPIX */


static const short DRAM_ATTR wraptable[64] =
{
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,-32
};

///IRAM_ATTR 
static void  tilebuf()
{
  int i, cnt;
  int base;
  byte *tilemap, *attrmap;
  int *tilebuf;
  short *wrap;


  base = ((R_LCDC&0x08)?0x1C00:0x1800) + (GBC_T<<5) + GBC_S;
  tilemap = lcd_vbank[0] + base;
  attrmap = lcd_vbank[1] + base;
  tilebuf = BG;
  wrap = (short int*)wraptable + GBC_S; ///
  cnt = ((WX + 7) >> 3) + 1;

  if (hw.cgb)
  {
    if (R_LCDC & 0x10)
      for (i = cnt; i > 0; i--)
      {
        *(tilebuf++) = *tilemap
          | (((int)*attrmap & 0x08) << 6)
          | (((int)*attrmap & 0x60) << 5);
        *(tilebuf++) = (((int)*attrmap & 0x07) << 2);
        attrmap += *wrap + 1;
        tilemap += *(wrap++) + 1;
      }
    else
      for (i = cnt; i > 0; i--)
      {
        *(tilebuf++) = (256 + ((n8)*tilemap))
          | (((int)*attrmap & 0x08) << 6)
          | (((int)*attrmap & 0x60) << 5);
        *(tilebuf++) = (((int)*attrmap & 0x07) << 2);
        attrmap += *wrap + 1;
        tilemap += *(wrap++) + 1;
      }
  }
  else
  {
    if (R_LCDC & 0x10)
      for (i = cnt; i > 0; i--)
      {
        *(tilebuf++) = *(tilemap++);
        tilemap += *(wrap++);
      }
    else
      for (i = cnt; i > 0; i--)
      {
        *(tilebuf++) = (256 + ((n8)*(tilemap++)));
        tilemap += *(wrap++);
      }
  }

  if (WX >= 160) return;

  base = ((R_LCDC&0x40)?0x1C00:0x1800) + (WT<<5);
  tilemap = lcd_vbank[0] + base;
  attrmap = lcd_vbank[1] + base;
  tilebuf = WND;
  cnt = ((160 - WX) >> 3) + 1;

  if (hw.cgb)
  {
    if (R_LCDC & 0x10)
      for (i = cnt; i > 0; i--)
      {
        *(tilebuf++) = *(tilemap++)
          | (((int)*attrmap & 0x08) << 6)
          | (((int)*attrmap & 0x60) << 5);
        *(tilebuf++) = (((int)*(attrmap++)&7) << 2);
      }
    else
      for (i = cnt; i > 0; i--)
      {
        *(tilebuf++) = (256 + ((n8)*(tilemap++)))
          | (((int)*attrmap & 0x08) << 6)
          | (((int)*attrmap & 0x60) << 5);
        *(tilebuf++) = (((int)*(attrmap++)&7) << 2);
      }
  }
  else
  {
    if (R_LCDC & 0x10)
      for (i = cnt; i > 0; i--)
        *(tilebuf++) = *(tilemap++);
    else
      for (i = cnt; i > 0; i--)
        *(tilebuf++) = (256 + ((n8)*(tilemap++)));
  }
}


///IRAM_ATTR 
static void  bg_scan()
{
  int cnt;
  byte *src, *dest;
  int *tile;

 

  if (WX <= 0) return;
  cnt = WX;
  tile = BG;
  dest = BUF;

  src = (byte*)get_patpix(*(tile++), GBC_V) + GBC_U;

#if 0
  memcpy(dest, src, 8-GBC_U);
#else
  byte tmp = 8-GBC_U;
  switch ((tmp))
  {
    case 8:
      dest[7] = src[7];
    case 7:
      dest[6] = src[6];
    case 6:
      dest[5] = src[5];
    case 5:
      dest[4] = src[4];
    case 4:
      dest[3] = src[2];
    case 3:
      dest[2] = src[2];
    case 2:
      dest[1] = src[1];
    case 1:
      dest[0] = src[0];
    default:
      break;
  }
#endif

  dest += 8-GBC_U;
  cnt -= 8-GBC_U;
  if (cnt <= 0) return;
  while (cnt >= 8)
  {
    src = (byte*)get_patpix(*(tile++), GBC_V);

#if 0
    MEMCPY8(dest, src);
#else
    int* tmpDest =(int*)dest;
    int* tmpSrc = (int*)src;
    tmpDest[0] = tmpSrc[0];
    tmpDest[1] = tmpSrc[1];
#endif

    dest += 8;
    cnt -= 8;
  }
  src = (byte*)get_patpix(*tile, GBC_V);
  while (cnt--)
    *(dest++) = *(src++);
}

static void  wnd_scan()
{
  int cnt;
  byte *src, *dest;
  int *tile;

  if (WX >= 160) return;
  cnt = 160 - WX;
  tile = WND;
  dest = BUF + WX;

  while (cnt >= 8)
  {
    src = (byte*)get_patpix(*(tile++), WV);

#if 0
    MEMCPY8(dest, src);
#else
    int* tmpDest =(int*)dest;
    int* tmpSrc = (int*)src;
    tmpDest[0] = tmpSrc[0];
    tmpDest[1] = tmpSrc[1];
#endif

    dest += 8;
    cnt -= 8;
  }
  src = (byte*)get_patpix(*tile, WV);
  while (cnt--)
    *(dest++) = *(src++);
}

///IRAM_ATTR
inline static void blendcpy(byte *dest, byte *src, byte b, int cnt)
{
  while (cnt--) *(dest++) = *(src++) | b;
}

///IRAM_ATTR
inline static int priused(void *attr)
{
  un32 *a = (un32*)attr; ///
  return (int)((a[0]|a[1]|a[2]|a[3]|a[4]|a[5]|a[6]|a[7])&0x80808080);
}

///IRAM_ATTR
static void  bg_scan_pri()
{
  int cnt, i;
  byte *src, *dest;

  if (WX <= 0) return;
  i = GBC_S;
  cnt = WX;
  dest = PRI;
  src = lcd_vbank[1] + ((R_LCDC&0x08)?0x1C00:0x1800) + (GBC_T<<5);

  if (!priused(src))
  {
    memset(dest, 0, cnt);
    return;
  }

  memset(dest, src[i++&31]&128, 8-GBC_U);
  dest += 8-GBC_U;
  cnt -= 8-GBC_U;
  if (cnt <= 0) return;
  while (cnt >= 8)
  {
    memset(dest, src[i++&31]&128, 8);
    dest += 8;
    cnt -= 8;
  }
  memset(dest, src[i&31]&128, cnt);
}

///IRAM_ATTR
static void  wnd_scan_pri()
{
  int cnt, i;
  byte *src, *dest;



  if (WX >= 160) return;
  i = 0;
  cnt = 160 - WX;
  dest = PRI + WX;
  src = lcd_vbank[1] + ((R_LCDC&0x40)?0x1C00:0x1800) + (WT<<5);

  if (!priused(src))
  {
    memset(dest, 0, cnt);
    return;
  }

  while (cnt >= 8)
  {
    memset(dest, src[i++]&128, 8);
    dest += 8;
    cnt -= 8;
  }
  memset(dest, src[i]&128, cnt);
}

#ifndef ASM_BG_SCAN_COLOR

///IRAM_ATTR
static void  bg_scan_color()
{
  int cnt;
  byte *src, *dest;
  int *tile;



  if (WX <= 0) return;
  cnt = WX;
  tile = BG;
  dest = BUF;

  src = (byte*)get_patpix(*(tile++),GBC_V) + GBC_U;
  blendcpy(dest, src, *(tile++), 8-GBC_U);
  dest += 8-GBC_U;
  cnt -= 8-GBC_U;
  if (cnt <= 0) return;
  while (cnt >= 8)
  {
    src = (byte*)get_patpix(*(tile++), GBC_V);
    blendcpy(dest, src, *(tile++), 8);
    dest += 8;
    cnt -= 8;
  }
  src = (byte*)get_patpix(*(tile++), GBC_V);
  blendcpy(dest, src, *(tile++), cnt);
}
#endif

///IRAM_ATTR
static void  wnd_scan_color()
{
  int cnt;
  byte *src, *dest;
  int *tile;


  if (WX >= 160) return;
  cnt = 160 - WX;
  tile = WND;
  dest = BUF + WX;

  while (cnt >= 8)
  {
    src = (byte*)get_patpix(*(tile++), WV);
    blendcpy(dest, src, *(tile++), 8);
    dest += 8;
    cnt -= 8;
  }
  src = (byte*)get_patpix(*(tile++), WV);
  blendcpy(dest, src, *(tile++), cnt);
}

///IRAM_ATTR
inline static void recolor(byte *buf, byte fill, int cnt)
{
  while (cnt--) *(buf++) |= fill;
}

///IRAM_ATTR
static void  spr_count()
{
  int i;
  struct obj *o;

  GBC_NS = 0;
  if (!(R_LCDC & 0x02)) return;

  o = GBC_lcd->oam.obj;

  for (i = 40; i; i--, o++)
  {
    if (L_GBC >= o->y || L_GBC + 16 < o->y)
      continue;
    if (L_GBC + 8 >= o->y && !(R_LCDC & 0x04))
      continue;
    if (++GBC_NS == 10) break;
  }
}


static struct vissprite ts[10];

///IRAM_ATTR
static void  spr_enum()
{
  int i, j;
  struct obj *o;
  int v, pat;
  int l, x;



  GBC_NS = 0;
  if (!(R_LCDC & 0x02)) return;

  o = GBC_lcd->oam.obj;

  for (i = 40; i; i--, o++)
  {
    if (L_GBC >= o->y || L_GBC + 16 < o->y)
      continue;
    if (L_GBC + 8 >= o->y && !(R_LCDC & 0x04))
      continue;
    VS[GBC_NS].x = (int)o->x - 8;
    v = L_GBC - (int)o->y + 16;
    if (hw.cgb)
    {
      pat = o->pat | (((int)o->flags & 0x60) << 5)
        | (((int)o->flags & 0x08) << 6);
      VS[GBC_NS].pal = 32 + ((o->flags & 0x07) << 2);
    }
    else
    {
      pat = o->pat | (((int)o->flags & 0x60) << 5);
      VS[GBC_NS].pal = 32 + ((o->flags & 0x10) >> 2);
    }
    VS[GBC_NS].pri = (o->flags & 0x80) >> 7;
    if ((R_LCDC & 0x04))
    {
      pat &= ~1;
      if (v >= 8)
      {
        v -= 8;
        pat++;
      }
      if (o->flags & 0x40) pat ^= 1;
    }
    VS[GBC_NS].pat = pat;
    VS[GBC_NS].v = v;

    if (++GBC_NS == 10) break;
  }
  if (!sprsort || hw.cgb) return;
  /* not quite optimal but it finally works! */
  for (i = 0; i < GBC_NS; i++)
  {
    l = 0;
    x = VS[0].x;
    for (j = 1; j < GBC_NS; j++)
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

#if 1
  //TODO: Check why is not working
  //memcpy(VS, ts, sizeof VS);
  int* vsPtr = (int*)VS;
  int* tsPtr = (int*)ts;
  int count = 16;
  while(count--)
  {
    vsPtr[0] = tsPtr[0];
    vsPtr++[1] = tsPtr++[1];
  }
#else
  
#endif
}


static byte bgdup[256];

///IRAM_ATTR
static void  spr_scan()
{
  int i, x;
  byte pal, b, ns = GBC_NS;
  byte *src, *dest, *bg, *pri;
  struct vissprite *vs;



  if (!ns) return;

#if 1
  memcpy(bgdup, BUF, 256);
#else
  for (i = 0; i < 64; ++i)
  {
    ((int*)bgdup)[i] = ((int*)BUF)[i];
  }
#endif

  vs = &VS[ns-1];

  for (; ns; ns--, vs--)
  {
    byte* sbuf = (byte*)get_patpix(vs->pat, vs->v);

    x = vs->x;
    if (x >= 160) continue;
    if (x <= -8) continue;
    if (x < 0)
    {
      src = sbuf - x;
      dest = BUF;
      i = 8 + x;
    }
    else
    {
      src = sbuf;
      dest = BUF + x;
      if (x > 152) i = 160 - x;
      else i = 8;
    }
    pal = vs->pal;
    if (vs->pri)
    {
      bg = bgdup + (dest - BUF);
      while (i--)
      {
        b = src[i];
        if (b && !(bg[i]&3)) dest[i] = pal|b;
      }
    }
    else if (hw.cgb)
    {
      bg = bgdup + (dest - BUF);
      pri = PRI + (dest - BUF);
      while (i--)
      {
        b = src[i];
        if (b && (!pri[i] || !(bg[i]&3)))
          dest[i] = pal|b;
      }
    }
    else while (i--) if (src[i]) dest[i] = pal|src[i];
  }
  if (sprdebug) for (i = 0; i < GBC_NS; i++) BUF[i<<1] = 36;
}


inline void lcd_begin()
{
///  vdest = fb.ptr + ((fb.w*fb.pelsize)>>1) - (80*fb.pelsize) + ((fb.h>>1) - 72) * fb.pitch;

///  vdest = fb.ptr;
  vdest = SCREENMEMORY; ///
  WY = R_WY;
}




///extern int framenum;
///extern uint16_t* displayBuffer[2];
int lastLcdDisabled = 0;

uint16_t Yline=0;
uint16_t Ybuf=0;

///IRAM_ATTR
void  lcd_refreshline()
{
  byte *dest;
  
/// if ((frame % 7) == 0) ++frame;


  L_GBC = R_LY;
  GBC_X = R_SCX;
  GBC_Y = (R_SCY + L_GBC) & 0xff;
  GBC_S = GBC_X >> 3;
  GBC_T = GBC_Y >> 3;
  GBC_U = GBC_X & 7;
  GBC_V = GBC_Y & 7;

  WX = R_WX - 7;
  if (WY>L_GBC || WY<0 || WY>143 || WX<-7 || WX>159 || !(R_LCDC&0x20))
    WX = 160;
  WT = (L_GBC - WY) >> 3;
  WV = (L_GBC - WY) & 7;

/// if ((framenum % 2) == 0)
if (fb.enabled)
///  if (1)
  {
    if (!(R_LCDC & 0x80))
    {
      if (!lastLcdDisabled)
      {
///       memset(displayBuffer[0], 0xff, 144 * 160 * 2);
///       memset(displayBuffer[1], 0xff, 144 * 160 * 2);

        lastLcdDisabled = 1;
      }

      return;
    }

    lastLcdDisabled = 0;


    spr_enum();
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
      recolor(BUF+WX, 0x04, 160-WX);
    }
    spr_scan();

    dest = vdest;
/*
    int cnt = 160;
    un16* dst = (un16*)dest;
    byte* src = BUF;

    while (cnt--) *(dst++) = PAL2[*(src++)];*/
//--------------------------------------------------------------------------------
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
      uint16_t Red = (PAL2[*(src)]  & 0b1111100000000000) >> 11;
      uint16_t Green = (PAL2[*(src)] & 0b0000011111100000) >> 6;
      uint16_t Blue = (PAL2[*(src)] & 0b0000000000011111) >> 0;

      uint8_t reducedCOLOR = (((Green >> 3) << 6) | ((Red >> 2) << 3) | ((Blue >> 2) << 0)); //RGB

      
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
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

if (Yline%5==0 || Yline%5==2|| Yline%5==4) Ybuf++;
Ybuf++;

Yline++;
if (Yline==144) {Yline=0; Ybuf=0;}
  }

  vdest += fb.pitch;
}

inline static void updatepalette(int i)
{
  short c;
  short r, g, b; //, y, u, v, rr, gg;

  short low = GBC_lcd->pal[i << 1];
  short high = GBC_lcd->pal[(i << 1) | 1];

  c = (low | (high << 8)) & 0x7fff;

  //bit 0-4 red
  r = c & 0x1f;

  // bit 5-9 green
  g = (c >> 5) & 0x1f;

  // bit 10-14 blue
  b = (c >> 10) & 0x1f;

  PAL2[i] = (r << 11) | (g << (5 + 1)) | (b);
}

inline void pal_write(int i, byte b)
{
  if (GBC_lcd->pal[i] != b)
  {
    GBC_lcd->pal[i] = b;
    updatepalette(i>>1);
  }
}

void  pal_write_dmg(int i, int mapnum, byte d)
{
  int j;
  int * const cmap = dmg_pal[mapnum & 0x3];
  int c;
  int r, g, b;

  
  if (hw.cgb) return;

  for (j = 0; j < 8; j += 2)
  {
    c = cmap[(d >> j) & 3];
    r = (c & 0xf8) >> 3;
    g = (c & 0xf800) >> 6;
    b = (c & 0xf80000) >> 9;
    c = r | g | b;

    /* FIXME - handle directly without faking cgb */
    pal_write(i+j, c & 0xff);
    pal_write(i+j+1, c >> 8);
  }

  //printf("pal_write_dmg: i=%d, d=0x%x\n", i , d);
}

///IRAM_ATTR
inline void vram_write(int a, byte b)
{
  //if (lcd_vbank[R_VBK&1][a] != b)
  {
    lcd_vbank[R_VBK&1][a] = b;
    if (a >= 0x1800) return;
  }
}

///IRAM_ATTR
void vram_dirty()
{
}

///IRAM_ATTR
void pal_dirty()
{
  int i;
  if (!hw.cgb)
  {
    pal_write_dmg(0, 0, R_BGP);
    pal_write_dmg(8, 1, R_BGP);
    pal_write_dmg(64, 2, R_OBP0);
    pal_write_dmg(72, 3, R_OBP1);
  }
  //else
  {
    for (i = 0; i < 64; i++)
    {
      updatepalette(i);
    }
  }
}

void lcd_reset()
{
  ///memset(&lcd, 0, sizeof lcd); ///NCX: cant because is malloc...
  ///memset(GBC_lcd, 0, sizeof(lcd_t)); ///NCX
  
//--------

  /*if (!lcd_vbank[0])*/ lcd_vbank[0]=(uint8_t*)malloc(8192 * 2 ); ///NCXE: memory saving... 16kB PSRAM CAN NOT BE USED
  lcd_vbank[1]=lcd_vbank[0]+8192;

  
  memset(lcd_vbank[0],0,8192);
  memset(lcd_vbank[1],0,8192);
  
/*  memset(GBC_lcd->oam.mem,0,256);
  memset(GBC_lcd->oam.obj,0,sizeof(GBC_lcd->oam.obj) );
  memset(GBC_lcd->pal,0,128);
*/  
//--------  


  lcd_begin();
  vram_dirty();
  pal_dirty();
}
