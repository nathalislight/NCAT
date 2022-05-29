#include "defs.h"
#include "regs.h"
#include "hw.h"
#include "cpu.h"
#include "mem.h"
#include "lcd.h"
#include "rc.h"
#include "rtc.h"
#include "sys.h"
#include "fb.h"
#include "sound.h"
#include "cpu.h"

extern byte *vdest;
extern uint16_t POWER;
static int framecount = 0;

void emu_init()
{
	
}

// emu_reset is called to initialize the state of the emulated
// system. It should set cpu registers, hardware registers, etc. to
// their appropriate values at powerup time.

void emu_reset() {
//--------------------------------------------------------------------------------
   //ram_ibank[0]=(byte*)ps_malloc(4096 * 8); ///moved to PSRAM 32kB
   ram_ibank[0]=(byte*)ps_calloc(4096 * 8 + 64,1); ///moved to PSRAM 32kB
   ram_ibank[1]=ram_ibank[0]+4096*1;
   ram_ibank[2]=ram_ibank[0]+4096*2;
   ram_ibank[3]=ram_ibank[0]+4096*3;
   ram_ibank[4]=ram_ibank[0]+4096*4;
   ram_ibank[5]=ram_ibank[0]+4096*5;
   ram_ibank[6]=ram_ibank[0]+4096*6;
   ram_ibank[7]=ram_ibank[0]+4096*7;

   if (NULL == GBC_scan) GBC_scan = (scan_t*)ps_malloc(sizeof(scan_t));
   if (NULL == GBC_lcd) GBC_lcd = (lcd_t*)ps_malloc(sizeof(lcd_t));
//--------------------------------------------------------------------------------

   vid_init(); ///NCX: here moved.
	
	 hw_reset();
	 lcd_reset();
	 cpu_reset();
	 mbc_reset();
	 sound_reset();
}

void emu_step()
{
	cpu_emulate(cpu__GBC.lcdc);
}



// This mess needs to be moved to another module; it's just here to
// make things work in the mean time. 

/*
void *sys_timer_();
#define EMU_GBC 2

void do_audio_frame_gbc() {
  pcm_submit();
}
//--------------------------------------------------------------------------------
unsigned long sys_micros_() {
  return micros();
}
//================================================================================
int sys_elapsed_(uint32_t * prev) {
   uint32_t now = sys_micros_();
   uint32_t delta = now - *prev;
   *prev = now;
   //printf("elapsed: %d\n", delta);
   return delta;
}
//--------------------------------------------------------------------------------
void sys_sleep_(int us) {
   if (us < 0) return;
   vTaskDelay(us/1000);
}
//================================================================================
*/

/*
void emu_run()
{
	void *timer = sys_timer();
	int delay;

	vid_begin();
	lcd_begin();
	for (;;)
	{
		cpu_emulate(2280);
		while (R_LY > 0 && R_LY < 144)
			emu_step();
		
		vid_end();
		rtc_tick();
		sound_mix();
		if (!pcm_submit())
		{
			delay = framelen - sys_elapsed(timer);
			sys_sleep(delay);
			sys_elapsed(timer);
		}
		doevents();
		vid_begin();
		if (framecount) { if (!--framecount) die("finished\n"); }
		
		if (!(R_LCDC & 0x80))
			cpu_emulate(32832);
		
		while (R_LY > 0) // wait for next frame 
			emu_step();
	}
}
*/