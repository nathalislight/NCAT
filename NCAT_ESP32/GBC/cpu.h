

#ifndef __CPU_H__
#define __CPU_H__



#include "defs.h"


union reg
{
	uint8_t b[2][2];
	uint16_t w[2];
	uint32_t d; /* padding for alignment, carry */
};

struct cpu_GBC
{
	union reg pc, sp, bc, de, hl, af;
	int ime, ima;
	int speed;
	int halt;
	int div, tim;
	int lcdc;
	int snd;
};

extern struct cpu_GBC cpu__GBC;

void cpu_reset();
void div_advance(int cnt);
void timer_advance(int cnt);
void lcdc_advance(int cnt);
void sound_advance(int cnt);
void cpu_timers(int cnt);
int cpu_emulate(int cycles);

#endif


