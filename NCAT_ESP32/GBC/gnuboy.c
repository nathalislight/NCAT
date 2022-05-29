// Hacks to get gnuboy to compile 
#include "rc.h"
#include "pcm.h"
#include "fb.h"
#include "hw.h"
#include "sys.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "driver/i2s.h" ///audio I2S

#define GB_WIDTH 160
#define GB_HEIGHT 144
#define GB_OFFSET 0

///uint8_t * fb_ram;
///uint32_t * fb_mono;

extern SemaphoreHandle_t draw_mutex;
extern int done_drawing;
extern int done_rendering; ///

///int framenum;

extern int PLAYING; //audio player active

// badge button constants 
enum button_t {
  BUTTON_UP_GBC     =  1,
  BUTTON_DOWN_GBC   =  2,
  BUTTON_LEFT_GBC   =  3,
  BUTTON_RIGHT_GBC  =  4,

  BUTTON_A_GBC      =  6,
  BUTTON_B_GBC      =  7,
  BUTTON_SELECT_GBC =  8,
  BUTTON_START_GBC  =  9,
  BUTTON_FLASH_GBC  = 10,

  // Number of buttons on the badge
  BUTTONS_NUM       = 10,
}; 

// What is "black" versus "white" on the mono display?
#define THRESHOLD 0x60

uint32_t input_button_state;

extern uint8_t gbc_I2S_enabled;

void vid_setpal(int i, int r, int g, int b)
{
	// should set the pallete; ignore for now
}

struct pcm pcm;
struct fb fb;







//----------------------------------------------------------------------------------------        
// audio to process
 
int pcm_submit() {
   
   //"HEAVY" audio correction:

   ///if (gbcaudio_busy) return 0;
///   while (gbcaudio_busy) vTaskDelay(1);

   while (gbcaudio_busy) delayMicroseconds(1); // need wait to sound_mix() complete...


           
   pcm_busy=1;


while(!gbcaudio_busy && BUFIDX2<BUFIDX1) {
      AUDIO_WRITE((uint8_t*)audio_frame[BUFIDX2%8],4*BUFSIZE[BUFIDX2%8],0); ///
      BUFIDX2++;
   }



   /*while(BUFIDX2+7<BUFIDX1) {
      printf(">>> BUFFER OVERLOAD TUNING DESTINATION ___ BUFIDX1:%d BUFIDX2: %d\n",BUFIDX1,BUFIDX2);
      BUFIDX2++;
   }*/
/*   if (BUFIDX2<BUFIDX1) {
      //if (deltatimeAUDIO==0)
      //AUDIO_WRITE((uint8_t*)audio_frame[BUFIDX2%8],4*BUFSIZE[BUFIDX2%8],17); ///portMAX_DELAY
      //else 
      AUDIO_WRITE((uint8_t*)audio_frame[BUFIDX2%8],4*BUFSIZE[BUFIDX2%8],portMAX_DELAY); ///
      BUFIDX2++;
   }  
   if (BUFIDX2<BUFIDX1) {
      //if (deltatimeAUDIO==0)
      //AUDIO_WRITE((uint8_t*)audio_frame[BUFIDX2%8],4*BUFSIZE[BUFIDX2%8],17); ///portMAX_DELAY
      //else 
      AUDIO_WRITE((uint8_t*)audio_frame[BUFIDX2%8],4*BUFSIZE[BUFIDX2%8],portMAX_DELAY); ///
      BUFIDX2++;
   }  
*/
   ///deltatimeAUDIO=0;

   /*if (BUFIDX2<BUFIDX1) {
      AUDIO_WRITE((uint8_t*)audio_frame[BUFIDX2%8],4*BUFSIZE[BUFIDX2%8],portMAX_DELAY); ///portMAX_DELAY
      BUFIDX2++;
   }*/
   pcm_busy=0;
   return 0;
}
//----------------------------------------------------------------------------------------    

// timers are just uint32_t
void * sys_timer_() {
	 static unsigned long timer__;
	 return &timer__;
}
//--------------------------------------------------------------------------------

bool buttonchange;
extern uint16_t POWER;

//--------------------------------------------------------------------------------
void doevents() {  

input_button_state=0;

if (INPUTREAD()) {

   if (((INPUT_START == 1 && INPUT_SELECT == 1) || INPUT_SYSTEM == 1) && !INPUT_A && !INPUT_B) {
      if (InGame_Menu==2) { //if is enabled in game menu
         vTaskDelay(100);
         do_window_menu();
         INPUTCLEAR();
         vTaskDelay(100);
      } else {
        POWER=0; //exit
      }
      buttonchange=false;
      return; ///! 
   }

   if (INPUT_A==1) input_button_state |= 1 << 6;
   else input_button_state &= 0b11111111111111111111111110111111;  

   if (INPUT_B==1) input_button_state |= 1 << 7;
   else input_button_state &= 0b11111111111111111111111101111111;
 
   if (INPUT_START==1) input_button_state |= 1 << 9;
   else input_button_state &= 0b11111111111111111111110111111111;

   if (INPUT_SELECT==1) input_button_state |= 1 << 8;
   else input_button_state &= 0b11111111111111111111111011111111;  

   if (INPUT_UP==1) input_button_state |= 1 << 1;
   else input_button_state &= 0b11111111111111111111111111111101;  

   if (INPUT_DOWN==1) input_button_state |= 1 << 2;
   else input_button_state &= 0b11111111111111111111111111111011;  

   if (INPUT_LEFT==1) input_button_state |= 1 << 3;
   else input_button_state &= 0b11111111111111111111111111110111;  

   if (INPUT_RIGHT==1) input_button_state |= 1 << 4;
   else input_button_state &= 0b11111111111111111111111111101111;

   

   buttonchange=true;
   INPUTCLEAR();
}   
//--------------------------------------------------------------------------------
 
    uint32_t button=buttonchange; ///
	 if (button == 0) return;
    uint32_t buttons = input_button_state;
	 static uint32_t old_buttons;
	 uint32_t delta = buttons ^ old_buttons;

	 if (delta == 0)	return;

	 //printf("%d %08x\n", button, buttons, delta);
  
	 if (delta & (1 << BUTTON_UP_GBC)) pad_set(PAD_UP, buttons & (1 << BUTTON_UP_GBC));
	 if (delta & (1 << BUTTON_DOWN_GBC)) pad_set(PAD_DOWN, buttons & (1 << BUTTON_DOWN_GBC));
	 if (delta & (1 << BUTTON_LEFT_GBC)) pad_set(PAD_LEFT, buttons & (1 << BUTTON_LEFT_GBC));
	 if (delta & (1 << BUTTON_RIGHT_GBC)) pad_set(PAD_RIGHT, buttons & (1 << BUTTON_RIGHT_GBC));
	 if (delta & (1 << BUTTON_START_GBC)) pad_set(PAD_START, buttons & (1 << BUTTON_START_GBC));
	 if (delta & (1 << BUTTON_SELECT_GBC)) pad_set(PAD_SELECT, buttons & (1 << BUTTON_SELECT_GBC));
	 if (delta & (1 << BUTTON_A_GBC)) pad_set(PAD_A, buttons & (1 << BUTTON_A_GBC));
	 if (delta & (1 << BUTTON_B_GBC))	pad_set(PAD_B, buttons & (1 << BUTTON_B_GBC));

  	old_buttons = input_button_state;
   buttonchange=false;
 	pad_refresh();
}


//================================================================================


void vid_init() { 
  ///fb_ram = (uint8_t *)ps_calloc(GB_WIDTH*GB_HEIGHT * 2,1); ///must be internal memory, PSRAM is slow for this.   NCXE:
	///if (!fb_ram) die("fb alloc failed\n");

	// use the pitch of the eink display, but the width of
	// the game boy display.
	fb.w = GB_WIDTH;
	fb.h = GB_HEIGHT;
   fb.pitch = GB_WIDTH*2;

	///fb.ptr = fb_ram;
   fb.ptr = SCREENMEMORY; ///
	fb.pelsize = 2;
	fb.indexed = 0;

	// we have no color, but we pack r/g/b into 8 bits
	fb.cc[0].r = 5;
	fb.cc[1].r = 5;
	fb.cc[2].r = 6;

	fb.cc[0].l = 0;
	fb.cc[1].l = 3;
	fb.cc[2].l = 6;
///	fb.enabled = 1;
	fb.enabled = 0; //start with fb.disabled... this fix initial picture scroll error !!!

	fb.dirty = 1;
}

/*
void vid_begin()
{
	//printf("begin\n"); 
}

///	 static unsigned long last_draw;
///	 static unsigned long last_skip;    
///	 static unsigned skipped;
void vid_end() 
{

/*
	 if(!fb.enabled){
      ///if (done_drawing) fb.enabled = 1;
      if (done_rendering) fb.enabled = 1;
      skipped++;
		return;
	 }

	 framenum++;
	 unsigned long delta = sys_micros_() - last_draw;
	
///if (!fb.enabled) delta = sys_micros_() - last_skip;
///if (framenum%2==0 && !fb.enabled) fb.enabled=1;

   printf("frame %d: skip %d %lu   ...   %lu\n", framenum, skipped, delta , (sys_micros_() - last_skip));

if (!fb.enabled && (sys_micros_() - last_skip)>0 && skipped>1) {
  fb.enabled=1;
  skipped = 0;
  last_skip= sys_micros_();
  last_draw = sys_micros_();

  ///vTaskDelay(1);
  return;
} else if (!fb.enabled) {
  skipped++;
  last_skip= sys_micros_();
  last_draw = sys_micros_();
  ///vTaskDelay(1);
}

if (skipped>0) {   
  fb.enabled=0;
  last_skip= sys_micros_();
  last_draw = sys_micros_();
  return;
}

//--------------------------------------------------------------------------------
   //GBC EMU render to SCREENMEMORY
/*   if (fb_ram && done_drawing==1) { 
      for (uint32_t Ytmp=0;Ytmp<144;Ytmp++) {
         for (uint32_t Xtmp=0;Xtmp<160;Xtmp++) {
            ///uint8_t RED=0;
            ///uint8_t GREEN=0;
            ///uint8_t BLUE=0;
            uint32_t c = fb_ram[(Ytmp*160+Xtmp)*2];
            c |= (fb_ram[(Ytmp*160+Xtmp)*2+1]<<16);
            
            uint32_t color =((c&0b000001100000000000000000 )>>12 | (c&0b110000000000000000000000)>>20 | (c&0b000000000000000000011000)>>3);
            
            //NCX: new SCREENMEMORY was moved +1px down        
/*            SCREENMEMORY_LINE[1+ (Ytmp)* 15/9][((Xtmp)*3)>>1]=color;
            SCREENMEMORY_LINE[1+ (Ytmp)* 15/9 + 1][((Xtmp)*3)>>1]=color;
            SCREENMEMORY_LINE[1+ (Ytmp)* 15/9][(((Xtmp)*3)>>1)  + 1]=color;
            SCREENMEMORY_LINE[1+ (Ytmp)* 15/9 + 1][(((Xtmp)*3)>>1) +1]=color;  */
/*         } 
      } 
      done_rendering=1;
      fb.enabled=0;
   }    */ 

/*
if (fb.enabled && delta>framelen)   {
   fb.enabled=0;
   skipped=1;
}   
//--------------------------------------------------------------------------------
      done_rendering=1;
   
	 

 	 // mark that the fb is in use and wake the drawing task
   ///	done_rendering = 0; ///NCXE: 

   if (done_rendering==1) {
       done_drawing=0;
       xSemaphoreGive(draw_mutex);
   }
	 last_draw = sys_micros_();

}
    */