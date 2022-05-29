#ifndef REFRESH_H
#define REFRESH_H
#include "defs.h"

void refresh_1(byte *dest, byte *src, byte *pal, int cnt);
void refresh_2(uint16_t *dest, byte *src, uint16_t *pal, int cnt);
void refresh_3(byte *dest, byte *src, uint32_t *pal, int cnt);
void refresh_4(uint32_t *dest, byte *src, uint32_t *pal, int cnt);
void refresh_1_2x(byte *dest, byte *src, byte *pal, int cnt);
void refresh_2_2x(uint16_t *dest, byte *src, uint16_t *pal, int cnt);
void refresh_3_2x(byte *dest, byte *src, uint32_t *pal, int cnt);
void refresh_4_2x(uint32_t *dest, byte *src, uint32_t *pal, int cnt);
void refresh_2_3x(uint16_t *dest, byte *src, uint16_t *pal, int cnt);
void refresh_3_3x(byte *dest, byte *src, uint32_t *pal, int cnt);
void refresh_4_3x(uint32_t *dest, byte *src, uint32_t *pal, int cnt);
void refresh_3_4x(byte *dest, byte *src, uint32_t *pal, int cnt);
void refresh_4_4x(uint32_t *dest, byte *src, uint32_t *pal, int cnt);

#endif

