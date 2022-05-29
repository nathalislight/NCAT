

#include "defs.h"
#include "lcd.h"

#define BUF (GBC_scan->buf)

#ifdef USE_ASM
#include "asm.h"
#endif


#ifndef ASM_REFRESH_1
void refresh_1(byte *dest, byte *src, byte *pal, int cnt)
{
	while(cnt--) *(dest++) = pal[*(src++)];
}
#endif

#ifndef ASM_REFRESH_2
void refresh_2(uint16_t *dest, byte *src, uint16_t *pal, int cnt)
{
	while (cnt--) *(dest++) = pal[*(src++)];
}
#endif

#ifndef ASM_REFRESH_3
void refresh_3(byte *dest, byte *src, uint32_t *pal, int cnt)
{
	uint32_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c>>8;
		*(dest++) = c>>16;
	}
}
#endif

#ifndef ASM_REFRESH_4
void refresh_4(uint32_t *dest, byte *src, uint32_t *pal, int cnt)
{
	while (cnt--) *(dest++) = pal[*(src++)];
}
#endif




#ifndef ASM_REFRESH_1_2X
void refresh_1_2x(byte *dest, byte *src, byte *pal, int cnt)
{
	byte c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_2_2X
void refresh_2_2x(uint16_t *dest, byte *src, uint16_t *pal, int cnt)
{
	uint16_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_2X
void refresh_3_2x(byte *dest, byte *src, uint32_t *pal, int cnt)
{
	uint32_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = c;
		dest[1] = dest[4] = c>>8;
		dest[2] = dest[5] = c>>16;
		dest += 6;
	}
}
#endif

#ifndef ASM_REFRESH_4_2X
void refresh_4_2x(uint32_t *dest, byte *src, uint32_t *pal, int cnt)
{
	uint32_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_2_3X
void refresh_2_3x(uint16_t *dest, byte *src, uint16_t *pal, int cnt)
{
	uint16_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_3X
void refresh_3_3x(byte *dest, byte *src, uint32_t *pal, int cnt)
{
	uint32_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = dest[6] = c;
		dest[1] = dest[4] = dest[7] = c>>8;
		dest[2] = dest[5] = dest[8] = c>>16;
		dest += 9;
	}
}
#endif

#ifndef ASM_REFRESH_4_3X
void refresh_4_3x(uint32_t *dest, byte *src, uint32_t *pal, int cnt)
{
	uint32_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_4X
void refresh_3_4x(byte *dest, byte *src, uint32_t *pal, int cnt)
{
	uint32_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = dest[6] = dest[9] = c;
		dest[1] = dest[4] = dest[7] = dest[10] = c>>8;
		dest[2] = dest[5] = dest[8] = dest[11] = c>>16;
		dest += 12;
	}
}
#endif

#ifndef ASM_REFRESH_4_4X
void refresh_4_4x(uint32_t *dest, byte *src, uint32_t *pal, int cnt)
{
	uint32_t c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

