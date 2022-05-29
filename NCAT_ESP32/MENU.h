void EVENTS();
uint8_t UPDATE_STATUS_BAR();
uint8_t HIDE_VOLUME_INDICATOR();

#define CLOCK_SHADOW 3

uint16_t loadedFileNames=0;
uint16_t onlyFileNames=0;
uint16_t SELECTED_FILE_POS=0;
uint8_t LOADED_ROM=0;

//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒

#define SETUP_CLOCK_SWITCH_INTERVAL 250 //250ms

uint8_t CLOCK_MODE=0; //0=OFF 1=running 2=setup 
uint16_t CLOCK_COLOR=0b1111111111111111;
int32_t SECONDS=0;

uint32_t SECOND=0;
uint32_t MINUTE=0;
uint32_t HOUR=0;

uint8_t DATE_DAY=1; 
uint8_t DATE_MONTH=1;
int32_t DATE_YEAR=2000;
uint8_t DATE_DAYOFWEEK=0;
 
uint8_t DIGIT0=0;
uint8_t DIGIT1=0;
uint8_t DIGIT2=0;
uint8_t DIGIT3=0;
uint8_t DIGIT4=0;
uint8_t DIGIT5=0;

uint8_t prev_DIGIT0=0;
uint8_t prev_DIGIT1=0;
uint8_t prev_DIGIT2=0;
uint8_t prev_DIGIT3=0;
uint8_t prev_DIGIT4=0;
uint8_t prev_DIGIT5=0;

uint32_t TIMECOUNTER=0;
uint32_t SETUPCOUNTER=0;

#if CLOCK
#ifdef NCAT_BOARD
struct ts RTC_time;
#endif

#ifdef NCAT_LITE
ErriezDS1307 ds1307rtc; // Use rtc in sketches
struct tm RTC_time;
#endif

#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
RTC_Date RTC_time;
#endif
#endif

//--------------------------------------------------------------------------------
int dayofweek(int d, int m, int y) {  
  static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };  
  y -= m < 3;  
  return ( y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;  
}
//--------------------------------------------------------------------------------
bool checkLeapYear(int year) {
  // If a year is multiple of 400,
  // then it is a leap year
  if (year % 400 == 0) return true;
 
  // Else If a year is multiple of 100,
  // then it is not a leap year
  if (year % 100 == 0) return false;
 
  // Else If a year is multiple of 4,
  // then it is a leap year
  if (year % 4 == 0) return true;
  return false;
}
//--------------------------------------------------------------------------------
void DO_INIT_CLOCK() {
  if (Clock_Color==11) CLOCK_COLOR=0b1111100000000000;       //RED
  else if (Clock_Color==12) CLOCK_COLOR=0b0000011111000000;  //GREEN
  else if (Clock_Color==13) CLOCK_COLOR=0b0000000000011111;  //BLUE
  else if (Clock_Color==14) CLOCK_COLOR=0b1111111111000000;  //YELLOW
  else if (Clock_Color==15) CLOCK_COLOR=0b1111100000011111;  //PINK
  else if (Clock_Color==16) CLOCK_COLOR=0b0000011111011111;  //CYAN
  else if (Clock_Color==17) CLOCK_COLOR=0b1111101111001000;  //ORANGE
  else if (Clock_Color==18) CLOCK_COLOR=0b0111101111001111;  //BLACK
  else if (Clock_Color==19) CLOCK_COLOR=0b0000000000000000;  //GREY
  else CLOCK_COLOR=0b1111111111111111;                       //WHITE

#if CLOCK
  if (Clock_Enabled==2) {
    printf("INIT_CLOCK...\n");
    TIMECOUNTER = millis___();
    SETUPCOUNTER = millis___();
    POWER=APP_EMU_CLOCK;
    CLOCK_MODE=1;
#ifdef NCAT_BOARD  
    DS3231_get(&RTC_time); //update every one minute from DS3231   
    vTaskDelay___(100);
  
    SECONDS = RTC_time.hour * 3600 + RTC_time.min * 60 + RTC_time.sec;
    SECONDS=SECONDS%86400;

    DATE_DAY=RTC_time.mday; 
    DATE_MONTH = RTC_time.mon;
    DATE_YEAR=RTC_time.year;
    DATE_DAYOFWEEK=RTC_time.wday;
#endif

#ifdef NCAT_LITE
    if (!ds1307rtc.read(&RTC_time)) printf("DS1307 read error...\n");
    SECONDS = ((RTC_time.tm_hour * 3600) + (RTC_time.tm_min * 60) + (RTC_time.tm_sec));  

    DATE_DAY = RTC_time.tm_mday;
    DATE_MONTH = RTC_time.tm_mon;
    DATE_YEAR = RTC_time.tm_year + 1900;
    DATE_DAYOFWEEK=dayofweek(DATE_DAY,DATE_MONTH,DATE_YEAR);  
#endif

#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
    RTC_time = ttgo->rtc->getDateTime();
    SECONDS = RTC_time.hour * 3600 + RTC_time.minute * 60 + RTC_time.second;  

    DATE_DAY = RTC_time.day;
    DATE_MONTH = RTC_time.month;
    DATE_YEAR = RTC_time.year;
    DATE_DAYOFWEEK=dayofweek(DATE_DAY,DATE_MONTH,DATE_YEAR);  
#endif

    SECOND=SECONDS%60;
    MINUTE=(SECONDS%3600 - SECOND) / 60;
    HOUR=(SECONDS%86400 - MINUTE*60 - SECOND) / 3600;

    DIGIT0=(SECOND%10);
    DIGIT1=(SECOND/10)%6;
    DIGIT2=(MINUTE%10);
    DIGIT3=(MINUTE/10)%6;
    DIGIT4=(HOUR%10);
    DIGIT5=(HOUR/10)%3;

    RESTORE_BACKGROUND(40+160,64, 32,64 );
    draw_char_xy_static_transparent(40+160 +CLOCK_SHADOW,64 +CLOCK_SHADOW,'0'+DIGIT0,font_7SEGMENT,0x0000);  
    draw_char_xy_static_transparent(40+160,64,'0'+DIGIT0,font_7SEGMENT,CLOCK_COLOR);  
    RESTORE_BACKGROUND(40+128+0,64, 32,64 );
    draw_char_xy_static_transparent(40+128 +CLOCK_SHADOW,64 +CLOCK_SHADOW,'0'+DIGIT1,font_7SEGMENT,0x0000);  
    draw_char_xy_static_transparent(40+128,64,'0'+DIGIT1,font_7SEGMENT,CLOCK_COLOR);  
    RESTORE_BACKGROUND(24+96,64, 32,64 );
    draw_char_xy_static_transparent(24+96 +CLOCK_SHADOW,64 +CLOCK_SHADOW,'0'+DIGIT2,font_7SEGMENT,0x0000);  
    draw_char_xy_static_transparent(24+96,64,'0'+DIGIT2,font_7SEGMENT,CLOCK_COLOR);  
    RESTORE_BACKGROUND(24+64,64, 32,64 );
    draw_char_xy_static_transparent(24+64 +CLOCK_SHADOW,64 +CLOCK_SHADOW,'0'+DIGIT3,font_7SEGMENT,0x0000);  
    draw_char_xy_static_transparent(24+64,64,'0'+DIGIT3,font_7SEGMENT,CLOCK_COLOR);  
    RESTORE_BACKGROUND(8+32,64, 32,64 );
    draw_char_xy_static_transparent(8+32 +CLOCK_SHADOW,64 +CLOCK_SHADOW,'0'+DIGIT4,font_7SEGMENT,0x0000);  
    draw_char_xy_static_transparent(8+32,64,'0'+DIGIT4,font_7SEGMENT,CLOCK_COLOR);  
    RESTORE_BACKGROUND(8+0,64, 32,64 );
    draw_char_xy_static_transparent(8+0 +CLOCK_SHADOW,64 +CLOCK_SHADOW,'0'+DIGIT5,font_7SEGMENT,0x0000);   
    draw_char_xy_static_transparent(8+0,64,'0'+DIGIT5,font_7SEGMENT,CLOCK_COLOR);   

    draw_char_xy_static_transparent(8+64 +CLOCK_SHADOW,64 +CLOCK_SHADOW,':',font_7SEGMENT_DOTS,0x0000);      //shadow
    draw_char_xy_static_transparent(24+64+64 +CLOCK_SHADOW,64 +CLOCK_SHADOW,':',font_7SEGMENT_DOTS,0x0000);  //shadow

    draw_char_xy_static_transparent(8+64,64,':',font_7SEGMENT_DOTS,CLOCK_COLOR);  
    draw_char_xy_static_transparent(24+64+64,64,':',font_7SEGMENT_DOTS,CLOCK_COLOR);  

//DATE
    uint8_t DATECHARSIZE=0;
    switch(DATE_DAYOFWEEK) {
      case 0:
        sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
        DATECHARSIZE=17;
        break;
      case 1:
        sprintf(TEXTBUFFER, (char*)"MONDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
        DATECHARSIZE=17;
        break;
      case 2:
        sprintf(TEXTBUFFER, (char*)"TUESDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
        DATECHARSIZE=18;
        break;
      case 3:
        sprintf(TEXTBUFFER, (char*)"WEDNESDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
        DATECHARSIZE=20;
        break;
      case 4:
        sprintf(TEXTBUFFER, (char*)"THURSDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
        DATECHARSIZE=19;
        break;
      case 5:
        sprintf(TEXTBUFFER, (char*)"FRIDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
        DATECHARSIZE=17;
        break;
      case 6:
        sprintf(TEXTBUFFER, (char*)"SATURDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
        DATECHARSIZE=19;
        break;
      case 7:
        sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
        DATECHARSIZE=17;
        break;
    }

    RESTORE_BACKGROUND(0,144, 240,16 +1 ); //DATE restore background

    draw_string_xy_transparent((240 - (DATECHARSIZE*8))/ 2 +2 , 144 +2, TEXTBUFFER, font_Ishmeria8x16, 0x0000);
    draw_string_xy_transparent((240 - (DATECHARSIZE*8))/ 2 , 144, TEXTBUFFER, font_Ishmeria8x16, CLOCK_COLOR);

    prev_DIGIT0=DIGIT0;
    prev_DIGIT1=DIGIT1;
    prev_DIGIT2=DIGIT2;
    prev_DIGIT3=DIGIT3;
    prev_DIGIT4=DIGIT4;
    prev_DIGIT5=DIGIT5; 
  }
#endif
}
//--------------------------------------------------------------------------------
void DO_DEINIT_CLOCK() {
  RESTORE_BACKGROUND(0,32, 240,240-32 ); //ALL restore background
  vTaskDelay___(20);
}
//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
uint8_t SETUP_DIGIT=0;
//--------------------------------------------------------------------------------
void DO_UPDATE_DIGITS() {
  if (SETUPCOUNTER+SETUP_CLOCK_SWITCH_INTERVAL<millis___()) {  
    if (INPUTREAD()) {  
    
      if (INPUT_LEFT == 1) {
        if (SETUP_DIGIT<5) SETUP_DIGIT++;        
        else SETUP_DIGIT=0;
      } else if (INPUT_RIGHT == 1) {
        if (SETUP_DIGIT>0) SETUP_DIGIT--;        
        else SETUP_DIGIT=5;
      }

      if (INPUT_UP==1) {
        if (SETUP_DIGIT==0) { SECONDS++; if (SECONDS>86400) SECONDS=SECONDS%86400; }
        else if (SETUP_DIGIT==1) {SECONDS+=60; if (SECONDS>86400) SECONDS=SECONDS%86400; }
        else if (SETUP_DIGIT==2) {SECONDS+=3600; if (SECONDS>86400) SECONDS=SECONDS%86400; }     

        else if (SETUP_DIGIT==3) {
          DATE_YEAR++;
        }
        else if (SETUP_DIGIT==4) {
          DATE_MONTH++;
          if (DATE_MONTH>12) DATE_MONTH=1;
        }
        else if (SETUP_DIGIT==5) {
          DATE_DAY++;
          if ((DATE_MONTH==4 || DATE_MONTH==6 || DATE_MONTH==9 || DATE_MONTH==11) && DATE_DAY>30) DATE_DAY=1;
          else if ((DATE_MONTH==1 || DATE_MONTH==3 || DATE_MONTH==5 || DATE_MONTH==7 || DATE_MONTH==8 || DATE_MONTH==10 || DATE_MONTH==12) && DATE_DAY>31) DATE_DAY=1;
          else if (checkLeapYear(DATE_YEAR)==false && DATE_MONTH==2 && DATE_DAY>28) DATE_DAY=1;        
          else if (checkLeapYear(DATE_YEAR)==true && DATE_MONTH==2 && DATE_DAY>29) DATE_DAY=1;        
        }

      } else if (INPUT_DOWN==1) {
        if (SETUP_DIGIT==0) {  SECONDS--; if (SECONDS<0) SECONDS=86400 + (SECONDS%86400); }
        else if (SETUP_DIGIT==1) { SECONDS-=60; if (SECONDS<0) SECONDS=86400 + (SECONDS%86400);  }
        else if (SETUP_DIGIT==2) { SECONDS-=3600; if (SECONDS<0) SECONDS=86400 + (SECONDS%86400);  }

        else if (SETUP_DIGIT==3) {
          DATE_YEAR--;
        }
        else if (SETUP_DIGIT==4) {
          DATE_MONTH--;
          if (DATE_MONTH<1) DATE_MONTH=12;
        }
        else if (SETUP_DIGIT==5) {
          DATE_DAY--;
          if ((DATE_MONTH==4 || DATE_MONTH==6 || DATE_MONTH==9 || DATE_MONTH==11) && DATE_DAY<1) DATE_DAY=30;
          else if ((DATE_MONTH==1 || DATE_MONTH==3 || DATE_MONTH==5 || DATE_MONTH==7 || DATE_MONTH==8 || DATE_MONTH==10 || DATE_MONTH==12) && DATE_DAY<1) DATE_DAY=31;
          else if (checkLeapYear(DATE_YEAR)==false && DATE_MONTH==2 && DATE_DAY<1) DATE_DAY=28;        
          else if (checkLeapYear(DATE_YEAR)==true && DATE_MONTH==2 && DATE_DAY<1) DATE_DAY=29;        
        }      
      }    

      if (INPUT_SYSTEM==1) CLOCK_MODE=1;  /// return to CLOCK mode
      if (INPUT_A==1) CLOCK_MODE=1;  /// return to CLOCK mode

      SECOND=SECONDS%60;
      MINUTE=(SECONDS%3600 - SECOND) / 60;
      HOUR=(SECONDS%86400 - MINUTE*60 - SECOND) / 3600;

      DIGIT0=(SECOND%10);
      DIGIT1=(SECOND/10)%6;
      DIGIT2=(MINUTE%10);
      DIGIT3=(MINUTE/10)%6;
      DIGIT4=(HOUR%10);
      DIGIT5=(HOUR/10)%3;


      RESTORE_BACKGROUND(40+160,64, 32,64 );  
      RESTORE_BACKGROUND(40+128+0,64, 32,64 );
      draw_char_xy_static_transparent(40+160+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT0,font_7SEGMENT,0x0000);  
      draw_char_xy_static_transparent(40+128+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT1,font_7SEGMENT,0x0000);  

      draw_char_xy_static_transparent(40+160,64,'0'+DIGIT0,font_7SEGMENT,CLOCK_COLOR);  
      draw_char_xy_static_transparent(40+128,64,'0'+DIGIT1,font_7SEGMENT,CLOCK_COLOR);  
      RESTORE_BACKGROUND(24+96,64, 32,64 );
      RESTORE_BACKGROUND(24+64,64, 32,64 );
      draw_char_xy_static_transparent(24+96+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT2,font_7SEGMENT,0x0000);  
      draw_char_xy_static_transparent(24+64+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT3,font_7SEGMENT,0x0000);  

      draw_char_xy_static_transparent(24+96,64,'0'+DIGIT2,font_7SEGMENT,CLOCK_COLOR);  
      draw_char_xy_static_transparent(24+64,64,'0'+DIGIT3,font_7SEGMENT,CLOCK_COLOR);  
      RESTORE_BACKGROUND(8+32,64, 32,64 );
      RESTORE_BACKGROUND(8+0,64, 32,64 );   
      draw_char_xy_static_transparent(8+32+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT4,font_7SEGMENT,0x0000);  
      draw_char_xy_static_transparent(8+0+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT5,font_7SEGMENT,0x0000);  

      draw_char_xy_static_transparent(8+32,64,'0'+DIGIT4,font_7SEGMENT,CLOCK_COLOR);  
      draw_char_xy_static_transparent(8+0,64,'0'+DIGIT5,font_7SEGMENT,CLOCK_COLOR);  

      INPUTCLEAR();
    }  
  SETUPCOUNTER=millis___();
  }
}
//--------------------------------------------------------------------------------
void DO_SETUP_CLOCK() {
  printf("Starting Setup Clock...\n");
#if CLOCK
  if (Clock_Enabled==2) {
    DO_INIT_CLOCK();
    CLOCK_MODE=2;
    SETUP_DIGIT=0;
    TIMECOUNTER = millis___();
    SETUPCOUNTER = millis___();
    POWER=APP_EMU_CLOCK;
    while(POWER==APP_EMU_CLOCK && CLOCK_MODE==2) {
      SECOND=SECONDS%60;
      MINUTE=(SECONDS%3600 - SECOND) / 60;
      HOUR=(SECONDS%86400 - MINUTE*60 - SECOND) / 3600;

      DIGIT0=(SECOND%10);
      DIGIT1=(SECOND/10)%6;
      DIGIT2=(MINUTE%10);
      DIGIT3=(MINUTE/10)%6;
      DIGIT4=(HOUR%10);
      DIGIT5=(HOUR/10)%3;
  
      if (SETUP_DIGIT==0) {
        draw_char_xy_static_transparent(40+160+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT0,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(40+128+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT1,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(40+160,64,'0'+DIGIT0,font_7SEGMENT,CLOCK_COLOR);  
        draw_char_xy_static_transparent(40+128,64,'0'+DIGIT1,font_7SEGMENT,CLOCK_COLOR);  
      } else if (SETUP_DIGIT==1) {
        draw_char_xy_static_transparent(24+96+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT2,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(24+64+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT3,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(24+96,64,'0'+DIGIT2,font_7SEGMENT,CLOCK_COLOR);  
        draw_char_xy_static_transparent(24+64,64,'0'+DIGIT3,font_7SEGMENT,CLOCK_COLOR);  
      } else if (SETUP_DIGIT==2) {
        draw_char_xy_static_transparent(8+32+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT4,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(8+0+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT5,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(8+32,64,'0'+DIGIT4,font_7SEGMENT,CLOCK_COLOR);  
        draw_char_xy_static_transparent(8+0,64,'0'+DIGIT5,font_7SEGMENT,CLOCK_COLOR);  
      }  
//--------------------------------------------------------------------------------
//DATE
      uint8_t DATECHARSIZE=0;
      switch(DATE_DAYOFWEEK) {
        case 0:
          sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
          DATECHARSIZE=17;
          break;
        case 1:
          sprintf(TEXTBUFFER, (char*)"MONDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
          DATECHARSIZE=17;
          break;
        case 2:
          sprintf(TEXTBUFFER, (char*)"TUESDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
          DATECHARSIZE=18;
          break;
        case 3:
          sprintf(TEXTBUFFER, (char*)"WEDNESDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
          DATECHARSIZE=20;
          break;
        case 4:
          sprintf(TEXTBUFFER, (char*)"THURSDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
          DATECHARSIZE=19;
          break;
        case 5:
          sprintf(TEXTBUFFER, (char*)"FRIDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
          DATECHARSIZE=17;
          break;
        case 6:
          sprintf(TEXTBUFFER, (char*)"SATURDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
          DATECHARSIZE=19;
          break;
        case 7:
          sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
          DATECHARSIZE=17;
          break;
      }
      RESTORE_BACKGROUND(0,144, 240,16 +1 ); //DATE restore background
      
      draw_string_xy_transparent((240 - (DATECHARSIZE*8))/ 2 +2 , 144 +2, TEXTBUFFER, font_Ishmeria8x16, 0x0000);
      draw_string_xy_transparent((240 - (DATECHARSIZE*8))/ 2 , 144, TEXTBUFFER, font_Ishmeria8x16, CLOCK_COLOR);
//--------------------------------------------------------------------------------
      while (TIMECOUNTER+500>millis___()) {DO_UPDATE_DIGITS(); vTaskDelay___(1); } //0.5 second
      TIMECOUNTER=millis___();
      
      if (SETUP_DIGIT==0) {
        RESTORE_BACKGROUND(40+160,64, 32,64 );  
        RESTORE_BACKGROUND(40+128+0,64, 32,64 );
      } else if (SETUP_DIGIT==1) {
        RESTORE_BACKGROUND(24+96,64, 32,64 );
        RESTORE_BACKGROUND(24+64,64, 32,64 );
      } else if (SETUP_DIGIT==2) {
        RESTORE_BACKGROUND(8+32,64, 32,64 );
        RESTORE_BACKGROUND(8+0,64, 32,64 );   
      }
//--------------------------------------------------------------------------------
//DATE
      else if (SETUP_DIGIT==3) {
        uint8_t DATECHARSIZE=0;
        switch(DATE_DAYOFWEEK) {
          case 0:
            sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-%02d-    ",DATE_DAY,DATE_MONTH);
            DATECHARSIZE=17;
            break;
          case 1:
            sprintf(TEXTBUFFER, (char*)"MONDAY %02d-%02d-    ",DATE_DAY,DATE_MONTH);
            DATECHARSIZE=17;
            break;
          case 2:
            sprintf(TEXTBUFFER, (char*)"TUESDAY %02d-%02d-    ",DATE_DAY,DATE_MONTH);
            DATECHARSIZE=18;
            break;
          case 3:
            sprintf(TEXTBUFFER, (char*)"WEDNESDAY %02d-%02d-    ",DATE_DAY,DATE_MONTH);
            DATECHARSIZE=20;
            break;
          case 4:
            sprintf(TEXTBUFFER, (char*)"THURSDAY %02d-%02d-    ",DATE_DAY,DATE_MONTH);
            DATECHARSIZE=19;
            break;
          case 5:
            sprintf(TEXTBUFFER, (char*)"FRIDAY %02d-%02d-    ",DATE_DAY,DATE_MONTH);
            DATECHARSIZE=17;
            break;
          case 6:
            sprintf(TEXTBUFFER, (char*)"SATURDAY %02d-%02d-    ",DATE_DAY,DATE_MONTH);
            DATECHARSIZE=19;
            break;
          case 7:
            sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-%02d-    ",DATE_DAY,DATE_MONTH);
            DATECHARSIZE=17;
            break;
        }
      } else if (SETUP_DIGIT==4) { 
        uint8_t DATECHARSIZE=0;
        switch(DATE_DAYOFWEEK) {
           case 0:
             sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-  -%04d",DATE_DAY,DATE_YEAR);
             DATECHARSIZE=17;
             break;
           case 1:
             sprintf(TEXTBUFFER, (char*)"MONDAY %02d-  -%04d",DATE_DAY,DATE_YEAR);
             DATECHARSIZE=17;
             break;
           case 2:
             sprintf(TEXTBUFFER, (char*)"TUESDAY %02d-  -%04d",DATE_DAY,DATE_YEAR);
             DATECHARSIZE=18;
             break;
           case 3:
             sprintf(TEXTBUFFER, (char*)"WEDNESDAY %02d-  -%04d",DATE_DAY,DATE_YEAR);
             DATECHARSIZE=20;
             break;
           case 4:
             sprintf(TEXTBUFFER, (char*)"THURSDAY %02d-  -%04d",DATE_DAY,DATE_YEAR);
             DATECHARSIZE=19;
             break;
           case 5:
             sprintf(TEXTBUFFER, (char*)"FRIDAY %02d-  -%04d",DATE_DAY,DATE_YEAR);
             DATECHARSIZE=17;
             break;
           case 6:
             sprintf(TEXTBUFFER, (char*)"SATURDAY %02d-  -%04d",DATE_DAY,DATE_YEAR);
             DATECHARSIZE=19;
             break;
           case 7:
             sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-  -%d04d",DATE_DAY,DATE_YEAR);
             DATECHARSIZE=17;
             break;
        }     
      } else if (SETUP_DIGIT==5) { 
        switch(DATE_DAYOFWEEK) {
          case 0:
            sprintf(TEXTBUFFER, (char*)"SUNDAY   -%02d-%04d",DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=17;
            break;
          case 1:
            sprintf(TEXTBUFFER, (char*)"MONDAY   -%02d-%04d",DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=17;
            break;
          case 2:
            sprintf(TEXTBUFFER, (char*)"TUESDAY   -%02d-%04d",DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=18;
            break;
          case 3:
            sprintf(TEXTBUFFER, (char*)"WEDNESDAY   -%02d-%04d",DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=20;
            break;
          case 4:
            sprintf(TEXTBUFFER, (char*)"THURSDAY   -%02d-%04d",DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=19;
            break;
          case 5:
            sprintf(TEXTBUFFER, (char*)"FRIDAY   -%02d-%04d",DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=17;
            break;
          case 6:
            sprintf(TEXTBUFFER, (char*)"SATURDAY   -%02d-%04d",DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=19;
            break;
          case 7:
            sprintf(TEXTBUFFER, (char*)"SUNDAY   -%02d-%04d",DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=17;
            break;
        }   
      }
      
      RESTORE_BACKGROUND(0,144, 240,16 +1 ); //DATE restore background
      draw_string_xy_transparent((240 - (DATECHARSIZE*8))/ 2 +2 , 144 +2, TEXTBUFFER, font_Ishmeria8x16, 0x0000);
      draw_string_xy_transparent((240 - (DATECHARSIZE*8))/ 2 , 144, TEXTBUFFER, font_Ishmeria8x16, CLOCK_COLOR);
//--------------------------------------------------------------------------------
      while (TIMECOUNTER+500>millis___()) {DO_UPDATE_DIGITS();  vTaskDelay___(1); } //0.5 second
      TIMECOUNTER=millis___();
    }
    printf("Saving new time...");
#ifdef NCAT_BOARD
    DS3231_get(&RTC_time); //update every one minute from DS3231   

    vTaskDelay___(100);
    
    SECOND=SECONDS%60;
    MINUTE=(SECONDS%3600 - SECOND) / 60;
    HOUR=(SECONDS%86400 - MINUTE*60 - SECOND) / 3600;

    RTC_time.sec = SECOND;
    RTC_time.min = MINUTE;
    RTC_time.hour = HOUR;
    RTC_time.wday = dayofweek(DATE_DAY,DATE_MONTH,DATE_YEAR);
    RTC_time.mday = DATE_DAY;
    RTC_time.mon = DATE_MONTH;
    RTC_time.year = DATE_YEAR;
    DS3231_set(RTC_time);

    printf("[ OK ]\n");
#endif

#ifdef NCAT_LITE
    DATE_DAYOFWEEK=dayofweek(DATE_DAY,DATE_MONTH,DATE_YEAR);  

    RTC_time.tm_sec = SECOND;
    RTC_time.tm_min = MINUTE;
    RTC_time.tm_hour = HOUR;
    RTC_time.tm_wday = dayofweek(DATE_DAY,DATE_MONTH,DATE_YEAR);
    RTC_time.tm_mday = DATE_DAY;
    RTC_time.tm_mon = DATE_MONTH;
    RTC_time.tm_year = DATE_YEAR - 1900;
        
    if (!ds1307rtc.write(&RTC_time)) printf("DS1307 write error!\n");   
#endif

#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
    ttgo->rtc->setDateTime(DATE_YEAR, DATE_MONTH, DATE_DAY, HOUR, MINUTE, SECOND);
#endif
    CLOCK_MODE=1;
  }
#endif  
}
//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
void DO_CLOCK() {
#if CLOCK
  if (Clock_Enabled==2) {
    if (DEBUG>3) printf("CLOCK.\n");

    DO_INIT_CLOCK();

    TIMECOUNTER = millis___();
    SETUPCOUNTER = millis___();

    POWER=APP_EMU_CLOCK;
    CLOCK_MODE=1;

    while(POWER==APP_EMU_CLOCK && CLOCK_MODE==1) {
      if (SECONDS%60==0) {
#ifdef NCAT_BOARD
        DS3231_get(&RTC_time); //update every one minute from DS3231   
        vTaskDelay___(100);
        SECONDS = RTC_time.hour * 3600 + RTC_time.min * 60 + RTC_time.sec;
        DATE_DAY=RTC_time.mday; 
        DATE_MONTH = RTC_time.mon;
        DATE_YEAR=RTC_time.year;
        DATE_DAYOFWEEK=RTC_time.wday;
#endif

#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
        RTC_time = ttgo->rtc->getDateTime();
        SECONDS = RTC_time.hour * 3600 + RTC_time.minute * 60 + RTC_time.second;  
        DATE_DAY = RTC_time.day;
        DATE_MONTH = RTC_time.month;
        DATE_YEAR = RTC_time.year;
        DATE_DAYOFWEEK=dayofweek(DATE_DAY,DATE_MONTH,DATE_YEAR);  
#endif     
      }

      prev_DIGIT0=DIGIT0;
      prev_DIGIT1=DIGIT1;
      prev_DIGIT2=DIGIT2;
      prev_DIGIT3=DIGIT3;
      prev_DIGIT4=DIGIT4;
      prev_DIGIT5=DIGIT5;

      SECOND=SECONDS%60;
      MINUTE=(SECONDS%3600 - SECOND) / 60;
      HOUR=(SECONDS%86400 - MINUTE*60 - SECOND) / 3600;

      DIGIT0=(SECOND%10);
      DIGIT1=(SECOND/10)%6;
      DIGIT2=(MINUTE%10);
      DIGIT3=(MINUTE/10)%6;
      DIGIT4=(HOUR%10);
      DIGIT5=(HOUR/10)%3;

      if (DIGIT0 != prev_DIGIT0) { 
        RESTORE_BACKGROUND(40+160,64, 32,64 );
        draw_char_xy_static_transparent(40+160+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT0,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(40+160,64,'0'+DIGIT0,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT1 != prev_DIGIT1) { 
        RESTORE_BACKGROUND(40+128+0,64, 32,64 );
        draw_char_xy_static_transparent(40+128+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT1,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(40+128,64,'0'+DIGIT1,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT2 != prev_DIGIT2) { 
        RESTORE_BACKGROUND(24+96,64, 32,64 );
        draw_char_xy_static_transparent(24+96+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT2,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(24+96,64,'0'+DIGIT2,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT3 != prev_DIGIT3) { 
        RESTORE_BACKGROUND(24+64,64, 32,64 );
        draw_char_xy_static_transparent(24+64+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT3,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(24+64,64,'0'+DIGIT3,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT4 != prev_DIGIT4) { 
        RESTORE_BACKGROUND(8+32,64, 32,64 );
        draw_char_xy_static_transparent(8+32+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT4,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(8+32,64,'0'+DIGIT4,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT5 != prev_DIGIT5) { 
        RESTORE_BACKGROUND(8+0,64, 32,64 );
        draw_char_xy_static_transparent(8+0+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT5,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(8+0,64,'0'+DIGIT5,font_7SEGMENT,CLOCK_COLOR);  
      }

      draw_char_xy_static_transparent(8+64+CLOCK_SHADOW,64+CLOCK_SHADOW,':',font_7SEGMENT_DOTS,0x0000);  
      draw_char_xy_static_transparent(24+64+64+CLOCK_SHADOW,64+CLOCK_SHADOW,':',font_7SEGMENT_DOTS,0x0000);  
      draw_char_xy_static_transparent(8+64,64,':',font_7SEGMENT_DOTS,CLOCK_COLOR);  
      draw_char_xy_static_transparent(24+64+64,64,':',font_7SEGMENT_DOTS,CLOCK_COLOR);  

//--------------------------------------------------------------------------------
//DATE
      if (SECONDS%60==0) {
  
        uint8_t DATECHARSIZE=0;
        switch(DATE_DAYOFWEEK) {
          case 0:
            sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=17;
            break;
          case 1:
            sprintf(TEXTBUFFER, (char*)"MONDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=17;
            break;
          case 2:
            sprintf(TEXTBUFFER, (char*)"TUESDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=18;
            break;
          case 3:
            sprintf(TEXTBUFFER, (char*)"WEDNESDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=20;
            break;
          case 4:
            sprintf(TEXTBUFFER, (char*)"THURSDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=19;
            break;
          case 5:
            sprintf(TEXTBUFFER, (char*)"FRIDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=17;
            break;
          case 6:
            sprintf(TEXTBUFFER, (char*)"SATURDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=19;
            break;
          case 7:
            sprintf(TEXTBUFFER, (char*)"SUNDAY %02d-%02d-%04d",DATE_DAY,DATE_MONTH,DATE_YEAR);
            DATECHARSIZE=17;
            break;
        }
        RESTORE_BACKGROUND(0,144, 240,16 +1 ); //DATE restore background
        draw_string_xy_transparent((240 - (DATECHARSIZE*8))/ 2 +2 , 144 +2, TEXTBUFFER, font_Ishmeria8x16, 0x0000);
        draw_string_xy_transparent((240 - (DATECHARSIZE*8))/ 2 , 144, TEXTBUFFER, font_Ishmeria8x16, CLOCK_COLOR);
      }
//--------------------------------------------------------------------------------
      while (TIMECOUNTER+500>millis___()) vTaskDelay___(1); //0.5 second
      TIMECOUNTER=millis___();

      RESTORE_BACKGROUND(8+64,64, 16,64 );
      RESTORE_BACKGROUND(24+64+64,64, 16,64 );

      while (TIMECOUNTER+500>millis___()) vTaskDelay___(1); //0.5 second
      TIMECOUNTER=millis___();
      SECONDS++;
  
      if (SETUPCOUNTER+50<millis___()) {  
        if (INPUTREAD()) {  
          if (INPUT_LEFT == 1 || INPUT_RIGHT == 1) {
            POWER=0;
          }
          if (INPUT_START==1) {
            //not implemented yet...
          }
          if (INPUT_SELECT==1) {
            //not implemented yet...
          }  
          if (INPUT_A==1) {
            CLOCK_MODE=2;  
            vTaskDelay___(250);
            INPUTCLEAR();

            DO_SETUP_CLOCK(); //while is inside function
            vTaskDelay___(250);
            DO_INIT_CLOCK();
          }  
        }
        SETUPCOUNTER=millis___();
      }
    }
  }
  DO_DEINIT_CLOCK();
#endif
}
//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
uint8_t TIME_COUNTER_BREAK=0;

void DO_CLOCK_TASK() {
//--------------------------------------------------------------------------------  
  EVENTS(); /// 
//--------------------------------------------------------------------------------  
#if CLOCK
  if (Clock_Enabled==2) {
    //if (DEBUG>4) printf("CLOCK_TASK.\n");
    if (POWER==APP_EMU_CLOCK && CLOCK_MODE==1) {  
      if (TIME_COUNTER_BREAK==1) {
        DO_INIT_CLOCK();
        //DS3231_get(&RTC_time); //update every one minute from DS3231   
        //SECONDS = RTC_time.hour * 3600 + RTC_time.min * 60 + RTC_time.sec;
      }
      SECOND=SECONDS%60;
      MINUTE=(SECONDS%3600 - SECOND) / 60;
      HOUR=(SECONDS%86400 - MINUTE*60 - SECOND) / 3600;

      DIGIT0=(SECOND%10);
      DIGIT1=(SECOND/10)%6;
      DIGIT2=(MINUTE%10);
      DIGIT3=(MINUTE/10)%6;
      DIGIT4=(HOUR%10);
      DIGIT5=(HOUR/10)%3;

      if (DIGIT0 != prev_DIGIT0) { 
        RESTORE_BACKGROUND(40+160,64, 32,64 );
        draw_char_xy_static_transparent(40+160+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT0,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(40+160,64,'0'+DIGIT0,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT1 != prev_DIGIT1) { 
        RESTORE_BACKGROUND(40+128+0,64, 32,64 );
        draw_char_xy_static_transparent(40+128+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT1,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(40+128,64,'0'+DIGIT1,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT2 != prev_DIGIT2) { 
        RESTORE_BACKGROUND(24+96,64, 32,64 );
        draw_char_xy_static_transparent(24+96+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT2,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(24+96,64,'0'+DIGIT2,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT3 != prev_DIGIT3) { 
        RESTORE_BACKGROUND(24+64,64, 32,64 );
        draw_char_xy_static_transparent(24+64+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT3,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(24+64,64,'0'+DIGIT3,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT4 != prev_DIGIT4) { 
        RESTORE_BACKGROUND(8+32,64, 32,64 );
        draw_char_xy_static_transparent(8+32+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT4,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(8+32,64,'0'+DIGIT4,font_7SEGMENT,CLOCK_COLOR);  
      }
      if (DIGIT5 != prev_DIGIT5) { 
        RESTORE_BACKGROUND(8+0,64, 32,64 );
        draw_char_xy_static_transparent(8+0+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT5,font_7SEGMENT,0x0000);  
        draw_char_xy_static_transparent(8+0+CLOCK_SHADOW,64+CLOCK_SHADOW,'0'+DIGIT5,font_7SEGMENT,CLOCK_COLOR);  
      }

      draw_char_xy_static_transparent(8+64+CLOCK_SHADOW,64+CLOCK_SHADOW,':',font_7SEGMENT_DOTS,0x0000);  
      draw_char_xy_static_transparent(24+64+64+CLOCK_SHADOW,64+CLOCK_SHADOW,':',font_7SEGMENT_DOTS,0x0000);  
      draw_char_xy_static_transparent(8+64,64,':',font_7SEGMENT_DOTS,CLOCK_COLOR);  
      draw_char_xy_static_transparent(24+64+64,64,':',font_7SEGMENT_DOTS,CLOCK_COLOR);  

      TIME_COUNTER_BREAK=0;

      while (TIMECOUNTER+500>millis___()) {
        vTaskDelay___(1); // 1ms
        if (INPUTREAD()) {       
          if (INPUT_LEFT == 1 || INPUT_RIGHT == 1 || INPUT_SYSTEM==1 || INPUT_A==1 || INPUT_UP==1 || INPUT_DOWN==1) { //switch MENU request, break.
            TIME_COUNTER_BREAK=1;
            break;      
          }
        }  
      }
      if (TIME_COUNTER_BREAK==0) //dont update timer when button pressed
        TIMECOUNTER=millis___();

      RESTORE_BACKGROUND(8+64,64, 16,64 );
      RESTORE_BACKGROUND(24+64+64,64, 16,64 );

      while (TIMECOUNTER+500>millis___()) {
        vTaskDelay___(1); //0.5 second
        if (INPUTREAD()) {      
          if (INPUT_LEFT == 1 || INPUT_RIGHT == 1 || INPUT_SYSTEM==1 || INPUT_A==1 || INPUT_UP==1 || INPUT_DOWN==1) { //switch MENU request, break.
            TIME_COUNTER_BREAK=1;      
            break;      
          }
        }  
      }
      if (TIME_COUNTER_BREAK==0) { //dont update timer when button pressed
        TIMECOUNTER=millis___();
        SECONDS++;
      }
      if (SETUPCOUNTER+50<millis___()) {  
        if (INPUTREAD()) {  
        
          if (INPUT_LEFT == 1 || INPUT_RIGHT == 1) {
            DO_DEINIT_CLOCK();
            POWER=0;      
          }

          if (INPUT_START==1) {
            //not implemented yet...
          }
          if (INPUT_SELECT==1) {
            //not implemented yet...
          }  
          if (INPUT_SYSTEM==1) {
            CLOCK_MODE=2;  
            vTaskDelay___(250);
            INPUTCLEAR();

            DO_SETUP_CLOCK(); ///while is inside function
            vTaskDelay___(250);
            DO_INIT_CLOCK();
          }
        }
        SETUPCOUNTER=millis___();
      }
      prev_DIGIT0=DIGIT0;
      prev_DIGIT1=DIGIT1;
      prev_DIGIT2=DIGIT2;
      prev_DIGIT3=DIGIT3;
      prev_DIGIT4=DIGIT4;
      prev_DIGIT5=DIGIT5;

      HIDE_VOLUME_INDICATOR(); /// 
    }
  }
#endif 
}
//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒

#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
uint8_t LCD_BACKLIGHT = 32; //0..255
#else 
uint8_t LCD_BACKLIGHT = 128; //0..255
#endif

uint8_t BATERYLEVEL=0; //0..7
uint8_t lastBATERYLEVEL=0; //0..7
uint8_t lastVOLUME = 0;
uint8_t lastBATTERY = 0;
int8_t lastUSBHOST_dev = -2;
int8_t lastBTDEVICESCONNECTED = -1;
int8_t lastBLUETOOTH_APP = -1;
//================================================================================
uint8_t UPDATE_STATUS_BAR() {
   
#ifndef NCAT_NO_PSRAM  
  if (lastVOLUME != VOLUME) {
    if (VOLUME == 0)
      renderPNG(4 , 2, PNG_AUDIO_0, sizeof(PNG_AUDIO_0), PNGOUTPUT_BUFFER, 0, 0xffff);
    else if (VOLUME > 0 && VOLUME <= 63)
      renderPNG(4 , 2, PNG_AUDIO_25, sizeof(PNG_AUDIO_25), PNGOUTPUT_BUFFER, 0, 0xffff);
    else if (VOLUME > 63 && VOLUME <= 127)
      renderPNG(4 , 2, PNG_AUDIO_50, sizeof(PNG_AUDIO_50), PNGOUTPUT_BUFFER, 0, 0xffff);
    else if (VOLUME > 127 && VOLUME <= 191)
      renderPNG(4 , 2, PNG_AUDIO_75, sizeof(PNG_AUDIO_75), PNGOUTPUT_BUFFER, 0, 0xffff);
    else if (VOLUME > 191 && VOLUME <= 255)
      renderPNG(4 , 2, PNG_AUDIO_100, sizeof(PNG_AUDIO_100), PNGOUTPUT_BUFFER, 0, 0xffff);
    lastVOLUME = VOLUME;
  }

  if (lastUSBHOST_dev != USB_HOST_devices) {
     
      if (USB_HOST_devices==0) {
        renderPNG(120 - 16 - 40, 2, USB_ICON_STANDBY, sizeof(USB_ICON_STANDBY), PNGOUTPUT_BUFFER, 0, 0xffff);
      } else if (USB_HOST_devices > 0) { 
        renderPNG(120 - 16 - 40, 2, USB_ICON_ACTIVE, sizeof(USB_ICON_ACTIVE), PNGOUTPUT_BUFFER, 0, 0xffff);
      } else if (USB_HOST_devices == -1) {
        renderPNG(120 - 16 - 40, 2, USB_ICON_INACTIVE, sizeof(USB_ICON_INACTIVE), PNGOUTPUT_BUFFER, 0, 0xffff);
      }
    lastUSBHOST_dev = USB_HOST_devices;
  }
  if (lastBTDEVICESCONNECTED != BLUETOOTH_CONNECTIONS || lastBLUETOOTH_APP!=BLUETOOTH_APP) {
    if (BLUETOOTH_CONNECTIONS > 0) {
      renderPNG(120 + 16 + 8, 2, BLUETOOTH_ICON_ACTIVE, sizeof(BLUETOOTH_ICON_ACTIVE), PNGOUTPUT_BUFFER, 0, 0x0000);
      lastBTDEVICESCONNECTED = BLUETOOTH_CONNECTIONS;
    } else if (BLUETOOTH_CONNECTIONS == 0) {
      renderPNG(120 + 16 + 8, 2, BLUETOOTH_ICON_INACTIVE, sizeof(BLUETOOTH_ICON_INACTIVE), PNGOUTPUT_BUFFER, 0, 0x0000);
      lastBTDEVICESCONNECTED = BLUETOOTH_CONNECTIONS;
    } 
    if (lastBLUETOOTH_APP!=BLUETOOTH_APP) {
      lastBLUETOOTH_APP=BLUETOOTH_APP;
      if (BLUETOOTH_APP == 0) {
        RESTORE_BACKGROUND(120 + 16 + 8, 2, 32, 32);
      }    
    }
  }
  if (lastBATTERY != BATTERY) {
#if (GPIO_BUTTONS==1 || GPIO_BUTTONS==2)
    if (BATTERY>127) BATERYLEVEL=8;
    else if (BATTERY<84 && BATTERY>=40) BATERYLEVEL=7; //~4.2V
    else if (BATTERY<94 && BATTERY>=84) BATERYLEVEL=6; //~4.0V
    else if (BATTERY<104 && BATTERY>=94) BATERYLEVEL=5; //~3.8V
    else if (BATTERY<113 && BATTERY>=104) BATERYLEVEL=4; //~3.6V
    else if (BATTERY<121 && BATTERY>=113) BATERYLEVEL=3; //~3.4V
    else if (BATTERY<160 && BATTERY>=121) BATERYLEVEL=2; //~3.2V
    else  BATERYLEVEL=1;    
#endif
#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
    BATTERY = (ttgo->power->getBattPercentage());
    printf("BATTERY: %d %\n",BATTERY);
    if (BATTERY>=80 && BATTERY<100) BATERYLEVEL=7;
    else if (BATTERY>=64 && BATTERY<80) BATERYLEVEL=6;
    else if (BATTERY>=48 && BATTERY<64) BATERYLEVEL=5;
    else if (BATTERY>=32 && BATTERY<48) BATERYLEVEL=4;
    else if (BATTERY>=16 && BATTERY<32) BATERYLEVEL=3;
    else if (BATTERY>=0 && BATTERY<16) BATERYLEVEL=2;
    else  BATERYLEVEL=1; 
#endif
    if (lastBATERYLEVEL!=BATERYLEVEL) {
      //CHARGING
      if (BATERYLEVEL==8) renderPNG(240 - 48 - 4 , 2, PNG_BATTERY_CHARGING_0, sizeof(PNG_BATTERY_CHARGING_0), PNGOUTPUT_BUFFER, 0, 0xffff);
      //100%
      else if (BATERYLEVEL==7) renderPNG(240 - 48 - 4 , 2, PNG_BATTERY_100, sizeof(PNG_BATTERY_100), PNGOUTPUT_BUFFER, 0, 0xffff);
      //80%
      else if (BATERYLEVEL==6) renderPNG(240 - 48 - 4 , 2, PNG_BATTERY_80, sizeof(PNG_BATTERY_80), PNGOUTPUT_BUFFER, 0, 0xffff);
      //60%
      else if (BATERYLEVEL==5) renderPNG(240 - 48 - 4 , 2, PNG_BATTERY_60, sizeof(PNG_BATTERY_60), PNGOUTPUT_BUFFER, 0, 0xffff);
      //40%
      else if (BATERYLEVEL==4) renderPNG(240 - 48 - 4 , 2, PNG_BATTERY_40, sizeof(PNG_BATTERY_40), PNGOUTPUT_BUFFER, 0, 0xffff);
      //20%
      else if (BATERYLEVEL==3) renderPNG(240 - 48 - 4 , 2, PNG_BATTERY_20, sizeof(PNG_BATTERY_20), PNGOUTPUT_BUFFER, 0, 0xffff);
      //0%
      else if (BATERYLEVEL==2) renderPNG(240 - 48 - 4 , 2, PNG_BATTERY_0, sizeof(PNG_BATTERY_0), PNGOUTPUT_BUFFER, 0, 0xffff);
      //ERROR
      else renderPNG(240 - 48 - 4 , 2, PNG_BATTERY_ERROR, sizeof(PNG_BATTERY_ERROR), PNGOUTPUT_BUFFER, 0, 0xffff);
      lastBATERYLEVEL=BATERYLEVEL;
    }   
    lastBATTERY = BATTERY;
  }
  return 1;
#endif
}
//================================================================================
uint8_t INDICATOR_MODE=1; //0=disabled 1=VOLUME 2=BACKGLIGHT 
uint32_t VOLUME_INDICATOR_TIMEOUT = 0;

uint8_t UPDATE_VOLUME_INDICATOR(uint8_t LEVEL) {
  RESTORE_BACKGROUND(212, 48, 20 + 1, 160 + 1);
  screenmemory_drawrectangle_static(212, 48, 20, 160, 0xffff);
  screenmemory_drawfillrectangle_static(216, 48 + 160 - (LEVEL * 10 / 16), 13, 1 + (LEVEL * 10 / 16), 0b0000011111000000);
  UPDATE_STATUS_BAR();
  VOLUME_INDICATOR_TIMEOUT = millis___();
  return 1;
}
uint8_t HIDE_VOLUME_INDICATOR() {
  RESTORE_BACKGROUND(212, 48, 20 + 1, 160 + 1);
  VOLUME_INDICATOR_TIMEOUT = 0;
  return 1;
}
//================================================================================

//================================================================================
uint8_t wakeup_LCD() {
#if LCD_BACKLIGHT_CONTROL
#if AUTO_POWER_SAVER
    if (LCD_BACKLIGHT==0) {
      if (DEBUG>1) printf("WAKING UP lcd display...\n");
      LCD_BACKLIGHT=BACKLIGHT_PIN_LAST_VALUE;
      BACKLIGHT_PIN_UPDATE(LCD_BACKLIGHT);
      BACKLIGHT_PIN_LAST_VALUE=0;
    }
#endif
#endif
}
//================================================================================

//================================================================================
uint32_t BACKLIGHT_INDICATOR_TIMEOUT = 0;
uint8_t UPDATE_BACKLIGHT_INDICATOR(uint8_t LEVEL) {
  RESTORE_BACKGROUND(212, 48, 20 + 1, 160 + 1);
  screenmemory_drawrectangle_static(212, 48, 20, 160, 0xffff);
  screenmemory_drawfillrectangle_static(216, 48 + 160 - (LEVEL * 10 / 16), 13, 1 + (LEVEL * 10 / 16), 0b0000000000011111); //BLUE
  BACKLIGHT_PIN_UPDATE(LCD_BACKLIGHT); // backlight level was changed...
  BACKLIGHT_INDICATOR_TIMEOUT = millis___();
  return 1;
}
uint8_t HIDE_BACKLIGHT_INDICATOR() {
  RESTORE_BACKGROUND(212, 48, 20 + 1, 160 + 1);
  BACKLIGHT_INDICATOR_TIMEOUT = 0;
  return 1;
}
//================================================================================

uint8_t MAX_MENU_ITEMS=7;

//********************************************************************************
//   FILE EXPLORE & BROWSE
//********************************************************************************
uint8_t EXIT = 0; //0=No Exit, 1=Exit from Browse, 2=Exit to Main Menu

#define MAXFILES 512
#define MAXFILENAME_LENGTH 64
char* filename[MAXFILES];
char fileext[4];
#define FILESPERPAGE 8

//--------------------------------------------------------------------------------
void sortStrings(char* arr[], int n)
{
  char temp[MAXFILENAME_LENGTH];

  // Sorting strings using bubble sort
  for (int j = 0; j < n - 1; j++)
  {
    for (int i = j + 1; i < n; i++)
    {
      if (strcmp(arr[j], arr[i]) > 0)
      {
        strcpy(temp, arr[j]);
        strcpy(arr[j], arr[i]);
        strcpy(arr[i], temp);
      }
    }
  }
}
//--------------------------------------------------------------------------------
void secondsToHMS( const uint32_t seconds, uint16_t &h, uint8_t &m, uint8_t &s ) {
  uint32_t t = seconds;
  s = t % 60;
  t = (t - s) / 60;
  m = t % 60;
  t = (t - m) / 60;
  h = t;
}
//--------------------------------------------------------------------------------

#if MICROSD
SdFile dirFile;
SdFile file;
#endif 

//################################################################################
char* EXPLORE_MICROSD(uint16_t TYPE, char* PATH, char* MESSAGE) {
#if MICROSD
  uint8_t num = 0;
  uint8_t loadedFileNames = 0;
  
  onlyFileNames=0;
  SELECTED_FILE_POS=0; 

  SDCARD_BUSY=1;

  //clear memory variables
  for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) memset (filename[tmp], 0, sizeof(filename[tmp]));
  fileext[0] = 0;
  fileext[1] = 0;
  fileext[2] = 0;
  fileext[3] = 0;

  num = 0;
  loadedFileNames = 0;

//----------------------------------------
  //LOAD FILENAMES INTO MEMORY...
//----------------------------------------
  if (!dirFile.open(PATH, O_READ)) {
    Serial.print("PATH NOT FOUND: ");
    Serial.println(PATH);

    screenmemory_fillscreen(0); //black color
    set_font_XY(32, 64 );
    draw_string((char*)"PATH NOT FOUND: ", 255);
    set_font_XY(32, 64  + 20);
    draw_string(PATH, 255);
    //while (1) {} //HALT.
//----------------------------------------
    //SWITCH SOURCE     
    INPUTCLEAR();
    delay___(1000);
    while(!INPUTREAD()) {
      INPUTCLEAR();
    } 
    delay___(50);
    if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
      INPUTCLEAR();    
      EXIT = 2;
      return MAINPATH;
    }
    if (LOADED_ROM==1) LOADED_ROM=11; //FLASH LOAD
    else if (LOADED_ROM==11) LOADED_ROM=21; //USB STORAGE LOAD
    else if (LOADED_ROM==21) LOADED_ROM=1; //SD CARD LOAD

    EXIT = 1;
    return MAINPATH;
//----------------------------------------    
  }
  while (num < MAXFILES && file.openNext(&dirFile, O_READ)) {
    // Skip hidden files.
    if (!file.isHidden()) {
      file.getName(filename[num], MAXFILENAME_LENGTH);
      Serial.println(filename[num]);
      if (file.isSubDir()) {
        sprintf(filename[num], "%s/", filename[num]);
        num++;
      } else {
        for (uint8_t i = strlen(filename[num]); i > 3; i--) {
          if (filename[num][i] != 0) {
            fileext[3] = '\0';
            fileext[2] = filename[num][i];
            fileext[1] = filename[num][i - 1];
            fileext[0] = filename[num][i - 2];
            break;
          }
        }
//................................................................................
        if (TYPE == APP_EMU_FAKE86) {
          //check DOS IMAGES File extension, then increase index
          if (((fileext[0] == 'B' || fileext[0] == 'b')
               && (fileext[1] == 'I' || fileext[1] == 'i')
               && (fileext[2] == 'N' || fileext[2] == 'n'))
              ||
              ((fileext[0] == 'I' || fileext[0] == 'i')
               && (fileext[1] == 'M' || fileext[1] == 'm')
               && (fileext[2] == 'G' || fileext[2] == 'g'))
              ||
              ((fileext[0] == 'V' || fileext[0] == 'v')
               && (fileext[1] == 'F' || fileext[1] == 'f')
               && (fileext[2] == 'D' || fileext[2] == 'd'))
             ) {
            num++;
            onlyFileNames++;
          }
        }
//................................................................................
        if (TYPE == APP_EMU_NES) {
          //check NES ROM IMAGES File extension, then increase index
          if (((fileext[0] == 'N' || fileext[0] == 'n')
               && (fileext[1] == 'E' || fileext[1] == 'e')
               && (fileext[2] == 'S' || fileext[2] == 's'))
             ) {
            num++;
            onlyFileNames++;
          }
        }
//................................................................................      
        if (TYPE == APP_EMU_GBC) {
          //check GBC ROM IMAGES File extension, then increase index
          if ((((fileext[0] == 'G' || fileext[0] == 'g')
               && (fileext[1] == 'B' || fileext[1] == 'b')
               && (fileext[2] == 'C' || fileext[2] == 'c'))
             )
          || (((fileext[1] == 'G' || fileext[1] == 'g')
               && (fileext[2] == 'B' || fileext[2] == 'b')
               ///&& (fileext[2] == 'C' || fileext[2] == 'c')
             ))) {            
            num++;
            onlyFileNames++;
          }
        }
//................................................................................       
        if (TYPE == APP_EMU_SMS) {
          //check SMS ROM IMAGES File extension, then increase index
          if ((((fileext[0] == 'S' || fileext[0] == 's')
               && (fileext[1] == 'M' || fileext[1] == 'm')
               && (fileext[2] == 'S' || fileext[2] == 's'))
             ) 
          || (((fileext[1] == 'G' || fileext[1] == 'g')
               && (fileext[2] == 'G' || fileext[2] == 'g')
             ))) {                         
            num++;
            onlyFileNames++;
          }
        } 
//................................................................................
        if (TYPE == APP_AUD_PLAYER)        {
          //check MP3 File extension, then increase index
          if ((fileext[0] == 'M' || fileext[0] == 'm')
            && (fileext[1] == 'P' || fileext[1] == 'p')
            && (fileext[2] == '3' || fileext[2] == '3')) {
            num++;
            onlyFileNames++;
          }
          if ((fileext[0] == 'W' || fileext[0] == 'w')
            && (fileext[1] == 'A' || fileext[1] == 'a')
            && (fileext[2] == 'V' || fileext[2] == 'v')) {
            num++;
            onlyFileNames++;
          }          
        }
//................................................................................
        if (TYPE == APP_SIGNALGEN) {
          //check 1bit BMP IMAGES File extension, then increase index
          if (((fileext[0] == 'B' || fileext[0] == 'b')
               && (fileext[1] == 'M' || fileext[1] == 'm')
               && (fileext[2] == 'P' || fileext[2] == 'p'))
             ) {
            num++;
            onlyFileNames++;
          }
        }
//................................................................................ 
      }
    }
    loadedFileNames = num;
    file.close();
  }
  dirFile.close();  
#ifdef DEBUG
  Serial.println("--------------------------------------");
  Serial.print("Count of loaded File Names:");
  Serial.println(loadedFileNames);
#endif
  sortStrings(filename, loadedFileNames);

  //DRAW FILENAMES INTO BUFFER
  uint8_t CURSOR = 0;
  uint8_t PAGE = 0;
  bool NamesDisplayed = false;

  while (1) {
    vPortYield(); //we are in infinite loop must yield();
    vTaskDelay___(1);
    PAGE = CURSOR / FILESPERPAGE;
    if (!NamesDisplayed) {
      screenmemory_fillscreen(0); //black color
      set_font_XY(16, 12 );
      draw_string(MESSAGE, 0,192);
      set_font_XY(16, 36 );
      draw_string(PATH, 192);

      for (num = PAGE * FILESPERPAGE; num < ((PAGE + 1)*FILESPERPAGE) && num < loadedFileNames; num++) {
        set_font_XY(40, 60 + 20 * (num % FILESPERPAGE));

        if (filename[num][strlen(filename[num]) - 1] == '/') draw_string(filename[num], 23);
        else draw_string(filename[num], 255);
        if (DEBUG>3) {
          Serial.println(filename[num]);
          Serial.println(strlen(filename[num]));
        }  
        delay___(1);
      }
      NamesDisplayed = true;
    }

    //Draw Cursor
    set_font_XY(16, 60 + (20 * (CURSOR % FILESPERPAGE)));
    draw_string((char*)"->", 255);
    
    if (LCD_ENABLED) xSemaphoreGive(draw_mutex); //refresh LCD
    vTaskDelay___(200); //ms

    //PROCESS CURSOR SELECTION
    INPUTCLEAR();
    while (!INPUTREAD()) {
      INPUTCLEAR();
      vTaskDelay___(1);
    }; 

    //Empty Cursor
    set_font_XY(16, 60 + (20 * (CURSOR % FILESPERPAGE)));
    draw_string((char*)"  ", 0);

    if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
      INPUTCLEAR();    
      EXIT = 2;
      return MAINPATH;
    }

    if (INPUT_UP == 1) {
      if (CURSOR % FILESPERPAGE == 0) NamesDisplayed = false; //changed page
      if (CURSOR == 0 && loadedFileNames > 0) CURSOR = loadedFileNames - 1;
      else if (CURSOR > 0 && loadedFileNames > 0) CURSOR--;

      while(INPUTREAD()) {
        INPUTCLEAR();
      }       
    }
    if (INPUT_DOWN == 1) {
      if (CURSOR % FILESPERPAGE == FILESPERPAGE - 1 || CURSOR == loadedFileNames - 1) NamesDisplayed = false; //changed page
      if (CURSOR == loadedFileNames - 1 && loadedFileNames > 0) CURSOR = 0;
      else if (CURSOR < loadedFileNames - 1 && loadedFileNames > 0) CURSOR++;

      while(INPUTREAD()) {
        INPUTCLEAR();
      }  
    }
    if (INPUT_LEFT) {
      if (CURSOR > FILESPERPAGE - 1) CURSOR -= FILESPERPAGE;
      NamesDisplayed = false;
      while(INPUTREAD()) {
        INPUTCLEAR();
      }  
    }
    if (INPUT_RIGHT == 1) {
      if (CURSOR / FILESPERPAGE < loadedFileNames / FILESPERPAGE) CURSOR += FILESPERPAGE;
      if (CURSOR > loadedFileNames - 1) CURSOR = loadedFileNames - 1;
      NamesDisplayed = false;
      
      while(INPUTREAD()) {
        INPUTCLEAR();
      } 
    }
    if (INPUT_SELECT == 1) {
      //SWITCH SOURCE     
      while(INPUTREAD()) {
        INPUTCLEAR();
      } 
      delay___(50);

      if (POWER==APP_AUD_PLAYER || POWER==APP_SIGNALGEN) continue; /// DISABLE SELECT SWITCH !
               
      if (LOADED_ROM==1) LOADED_ROM=11; //FLASH LOAD
      else if (LOADED_ROM==11) LOADED_ROM=21; //USB STORAGE LOAD
      else if (LOADED_ROM==21) LOADED_ROM=1; //SD CARD LOAD

      EXIT = 1;
      return MAINPATH;
    }
    if (loadedFileNames > 0) { ///cannot select NOTHING!
      if ((INPUT_A == 1 || INPUT_B == 1) && INPUT_SELECT == 0) {
        dirFile.close(); 

        while(INPUTREAD()) {
          INPUTCLEAR();
        }  
        delay___(50);
   
        sprintf(MAINPATH, "%s%s", PATH, filename[CURSOR]);
        SELECTED_FILE_POS=CURSOR;                 
        sprintf(TRACKNAME,"%s",filename[CURSOR]); //load FILENAME
        return MAINPATH ; //START //A
      }
    }   
    if (((INPUT_START == 1 ) && INPUT_SELECT == 0)) {
      while(INPUTREAD()) {
        INPUTCLEAR();
      }       
      dirFile.close();
      delay___(25);

      sprintf(MAINPATH, "%s", PATH);
        
      if (strlen(MAINPATH) > 1) {
        MAINPATH[strlen(MAINPATH) - 1] = '\0';
        for (uint8_t strpos = strlen(MAINPATH) - 1; strpos > 0; strpos--) {
          if (MAINPATH[strpos] == '/') break;
          MAINPATH[strpos] = '\0';
        }
      }
      return MAINPATH;
    }

    if ((INPUT_SYSTEM==1 || (BTKEYBOARD[0x01]&&BTKEYBOARD[0x47]))) { ///"SYSTEM button" pressed or "ESC+ENTER" = EXIT
      EXIT=1;
      POWER=0; 
    }  
  }
  vTaskDelay___(300);
#endif
}
//################################################################################
char* Browse_MICROSD(uint16_t TYPE, char* PATH, char* MESSAGE) {
#if MICROSD
  SDCARD_BUSY=1;
  
#ifdef DEBUG
  Serial.print("microSD: Browsing in PATH:");
  Serial.println(PATH);
#endif

  if (PATH[strlen(PATH) - 1] != '/')
    if (strlen(PATH) > 1) {
      PATH[strlen(PATH) - 1] = '\0';
      for (uint8_t strpos = strlen(PATH) - 1; strpos > 0; strpos--) {
        if (PATH[strpos] == '/') break;
        PATH[strpos] = '\0';
      }
    }

  EXIT = 0;
  //................................................................................
  if (!filename[0]) {
  printf(">>> alloc filenames\n");
  if (PSRAM_BASE_SIZE > 0)
    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char*)ps_malloc(MAXFILENAME_LENGTH + 1); //temporary ps_malloc filenamebuffer
  else
    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char*)malloc(MAXFILENAME_LENGTH + 1); //temporary malloc filenamebuffer
  }  


  while (EXIT == 0 && PATH[strlen(PATH) - 1] == '/')  {
    PATH =  EXPLORE_MICROSD(TYPE, PATH, MESSAGE);
    Serial.println(PATH);
  }

    if (EXIT==0) LOADED_ROM=2; //SD CARD LOAD SELECTED

  if (filename[0]) {
    printf(">>> free filenames\n");

    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) {
      free(filename[tmp]); //free temporary malloc filenamebuffer
      filename[tmp]=NULL;
    }    
  }  
  SDCARD_BUSY=0;

  //................................................................................
  return PATH;

#else //NoMicroSD
  LOADED_ROM=11; //FLASH LOAD
#endif

}
//________________________________________________________________________________





//################################################################################
//IRAM_ATTR 
char* EXPLORE_USB(uint16_t TYPE, char* PATH, char* MESSAGE) {
#if USBHOST
  #if USBHOST_STORAGE
  if (USBhost_mass_storage == 2) {
    uint8_t num = 0;
    uint8_t loadedFileNames = 0;
//--------------------------------------------------------------------------------
    uint8_t LOADING_CHAR=0;

    screenmemory_fillscreen(0); //black color
    set_font_XY(16, 36 );
    draw_string((char*)"LOADING: ", 255);

    USBSTORAGE_BUSY=1;

    INPUTCLEAR();
    vTaskDelay___(500);

    while (USBSTORAGE_BUSY>0 && !partsready) { //
      vPortYield();
      vTaskDelay___(50);  //               

      EVENTS(); // !

      set_font_XY(16+80, 36 );
      if (LOADING_CHAR==0) draw_string((char*)"/", 255);
      if (LOADING_CHAR==1) draw_string((char*)"-", 255);
      if (LOADING_CHAR==2) draw_string((char*)"\\", 255);
      if (LOADING_CHAR==3) draw_string((char*)"|", 255);
      LOADING_CHAR++;
      if (LOADING_CHAR==4) LOADING_CHAR=0;

      INPUTREAD();
      if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
        INPUTCLEAR();
        EXIT = 2;
        return MAINPATH;
      }
      if (INPUT_SELECT == 1) { //SWITCH STORAGE
        //SWITCH SOURCE     
        while(INPUTREAD()) {
          INPUTCLEAR();
        } 
        delay___(50);

        if (LOADED_ROM==1) LOADED_ROM=11; //FLASH LOAD
        else if (LOADED_ROM==11) LOADED_ROM=21; //USB STORAGE LOAD 
        else if (LOADED_ROM==21) LOADED_ROM=1; //SD CARD LOAD

        EXIT = 1;
        return MAINPATH;
      }
    }
    vTaskDelay___(1000); ///needs 1000ms delay before LIST
       
    if (!SDCARD_BUSY && USBSTORAGE_BUSY==1 && usbon) {
      printf("EXPLORE_USB: !SDCARD_BUSY & usbon \n");
      USBSTORAGE_BUSY=2;
    
      if(fatready && Fats[0] != NULL) {
        printf("EXPLORE_USB: fatready \n");
//--------------------------------------------------------------------------------                        
        FRESULT rc; // Result code 
//--------------------------------------------------------------------------------  
        FATFS *fs = NULL;
        for (uint8_t zz = 0; zz < _VOLUMES; zz++) {
          if(Fats[zz]->volmap == 0) fs = Fats[zz]->ffs;
        } 
//--------------------------------------------------------------------------------
#if _USE_LFN
        char lfn[_MAX_LFN + 1];
        FILINFO My_File_Info_Object_x; // File information object 
        My_File_Info_Object_x.lfname = lfn;
#endif
        DIR My_Dir_Object_x; // Directory object 
        printf_P(PSTR("\r\nUSB: Open root directory.\r\n"));
        if (PATH[strlen(PATH)- 1]=='/') {
          PATH[strlen(PATH)- 1]='\0';
          printf("MOD PATH: %s\n",PATH);
        }
        rc = f_opendir(&My_Dir_Object_x, PATH); //without last slash
        if (PATH[strlen(PATH)-1]!='/') {
          uint8_t PATHPOS=strlen(PATH);
          PATH[PATHPOS]='/';
          PATH[PATHPOS+1]='\0';
          printf("REMOD PATH: %s\n",PATH);
        }
        if(rc>0) {
          if (rc==5) {
            printf("USB ERROR: PATH NOT FOUND: %s\n",PATH);
            screenmemory_fillscreen(0); //black color
            set_font_XY(32, 64 );
            draw_string((char*)"PATH NOT FOUND: ", 255);
            set_font_XY(32, 64  + 20);
            draw_string(PATH, 255);
            while (1) {} //HALT.      
          } else printf("USB ERROR: %d",rc);
          while(EXIT!=2) {
              INPUTCLEAR();
              delay___(1000);
              while(!INPUTREAD()) {
              INPUTCLEAR();
            } 
            delay___(50);
            if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
              INPUTCLEAR();    
              EXIT = 2;
              return MAINPATH;
            }
          }; //halt.
        }
        printf("\r\nDirectory listing...\r\n");
//--------------------------------------------------------------------------------
        //clear memory variables
        for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) memset (filename[tmp], 0, sizeof(filename[tmp]));

        fileext[0] = 0;
        fileext[1] = 0;
        fileext[2] = 0;
        fileext[3] = 0;

        num = 0;
        loadedFileNames = 0;

        //LOAD FILENAMES INTO MEMORY...
        if (rc) {
          Serial.print("PATH NOT FOUND: ");
          Serial.println(PATH);

          screenmemory_fillscreen(0); //black color
          set_font_XY(32, 64 );
          draw_string((char*)"PATH NOT FOUND: ", 255);
          set_font_XY(32, 64  + 20);
          draw_string(PATH, 255);
          ///while (1) {} //HALT.
//----------------------------------------
          INPUTCLEAR();
          delay___(1000);
          while(!INPUTREAD()) {
            INPUTCLEAR();
          } 
          delay___(50);
          if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
            INPUTCLEAR();    
            EXIT = 2;
            return MAINPATH;
          }

          if (LOADED_ROM==1) LOADED_ROM=11; //FLASH LOAD
          else if (LOADED_ROM==11) LOADED_ROM=21; //USB STORAGE LOAD
          else if (LOADED_ROM==21) LOADED_ROM=1; //SD CARD LOAD

          EXIT = 1;
          return MAINPATH;
//----------------------------------------    
        }
        while (num < MAXFILES ) {
          vPortYield(); 
//--------------------------------------------------------------------------------
#if _USE_LFN
          My_File_Info_Object_x.lfsize = 60;
#endif
          rc = f_readdir(&My_Dir_Object_x, &My_File_Info_Object_x); // Read a directory item 
          if(rc || !My_File_Info_Object_x.fname[0]) break; // Error or end of dir 

          if(My_File_Info_Object_x.fattrib & AM_DIR) {
            Serial.write('d');
          } else {
            Serial.write('-');
          }
          Serial.write('r');

          if(My_File_Info_Object_x.fattrib & AM_RDO) {
            Serial.write('-');
          } else {
            Serial.write('w');
          }
          if(My_File_Info_Object_x.fattrib & AM_HID) {
            Serial.write('h');
          } else {
            Serial.write('-');
          }

          if(My_File_Info_Object_x.fattrib & AM_SYS) {
            Serial.write('s');
          } else {
            Serial.write('-');
          }

          if(My_File_Info_Object_x.fattrib & AM_ARC) {
            Serial.write('a');
          } else {
            Serial.write('-');
          }
#if _USE_LFN
          if(*My_File_Info_Object_x.lfname) printf_P(PSTR(" %8lu  %s (%s)\r\n"), My_File_Info_Object_x.fsize, My_File_Info_Object_x.fname, My_File_Info_Object_x.lfname);
          else
#endif
          printf_P(PSTR(" %8lu  %s\r\n"), My_File_Info_Object_x.fsize, &(My_File_Info_Object_x.fname[0]));                                       
//--------------------------------------------------------------------------------
          // Skip hidden files.
          if(!(My_File_Info_Object_x.fattrib & AM_HID)) { 
            if(*My_File_Info_Object_x.lfname) strcpy(filename[num],My_File_Info_Object_x.lfname); ///
            else strcpy(filename[num],&(My_File_Info_Object_x.fname[0])); ///

            Serial.println(filename[num]);
            Serial.print(">>>num: ");
            Serial.println(num);
            Serial.print(">>>strlen: ");
            Serial.println(strlen(filename[num]));

            if(My_File_Info_Object_x.fattrib & AM_DIR) {
              if (strcmp(filename[num], ".")==0) continue;
              if (strcmp(filename[num], "..")==0) continue;

              sprintf(filename[num], "%s/", filename[num]);
              num++;

            } else {
              for (uint8_t i = strlen(filename[num]); i > 3; i--) {
                if (filename[num][i] != 0) {
                  fileext[3] = '\0';
                  fileext[2] = filename[num][i];
                  fileext[1] = filename[num][i - 1];
                  fileext[0] = filename[num][i - 2];
                  break;
                }
              }

              if (TYPE == APP_EMU_FAKE86) {
              
                //check DOS IMAGES File extension, then increase index
                if (((fileext[0] == 'B' || fileext[0] == 'b')
                     && (fileext[1] == 'I' || fileext[1] == 'i')
                     && (fileext[2] == 'N' || fileext[2] == 'n'))
                    ||
                    ((fileext[0] == 'I' || fileext[0] == 'i')
                     && (fileext[1] == 'M' || fileext[1] == 'm')
                     && (fileext[2] == 'G' || fileext[2] == 'g'))
                    ||
                    ((fileext[0] == 'V' || fileext[0] == 'v')
                     && (fileext[1] == 'F' || fileext[1] == 'f')
                     && (fileext[2] == 'D' || fileext[2] == 'd'))
                   ) {
                  num++;
                }
              }
              if (TYPE == APP_EMU_NES) {
              
                //check NES ROM IMAGES File extension, then increase index
                if (((fileext[0] == 'N' || fileext[0] == 'n')
                     && (fileext[1] == 'E' || fileext[1] == 'e')
                     && (fileext[2] == 'S' || fileext[2] == 's'))
                   ) {
                  num++;
                }
              }
              if (TYPE == APP_EMU_GBC) {
              
                //check GBC ROM IMAGES File extension, then increase index
                if ((((fileext[0] == 'G' || fileext[0] == 'g')
                     && (fileext[1] == 'B' || fileext[1] == 'b')
                     && (fileext[2] == 'C' || fileext[2] == 'c'))
                   )
                || (((fileext[1] == 'G' || fileext[1] == 'g')
                     && (fileext[2] == 'B' || fileext[2] == 'b')
                     ///&& (fileext[2] == 'C' || fileext[2] == 'c')
                   ))) {
                  num++;
                }
              } 

              if (TYPE == APP_EMU_SMS) {
                //check SMS ROM IMAGES File extension, then increase index
                if (((fileext[0] == 'S' || fileext[0] == 's')
                     && (fileext[1] == 'M' || fileext[1] == 'm')
                     && (fileext[2] == 'S' || fileext[2] == 's'))
                   ) {
                  num++;
                }
              }               
            }
          }
          loadedFileNames = num;
        }
//................................................................................
        if(rc) while(1) {} ///halt.
        DISK_IOCTL(fs->drv, CTRL_COMMIT, 0); ///like FILE CLOSE
//................................................................................
        USBSTORAGE_BUSY=1; 
        if (DEBUG>4) {
          Serial.println("--------------------------------------");
          Serial.print("Count of loaded File Names:");
          Serial.println(loadedFileNames);
        }
  
        sortStrings(filename, loadedFileNames);
  
        //DRAW FILENAMES INTO BUFFER
        uint8_t CURSOR = 0;
        uint8_t PAGE = 0;
        bool NamesDisplayed = false;
  
        while (1) {
          vPortYield(); //we are in infinite loop must yield();
          vTaskDelay___(1);

          PAGE = CURSOR / FILESPERPAGE;
          if (!NamesDisplayed) {
            screenmemory_fillscreen(0); //black color
            set_font_XY(16, 12 );
            draw_string(MESSAGE, 0,57);
            set_font_XY(16, 36 );
            draw_string(PATH, 57);

            for (num = PAGE * FILESPERPAGE; num < ((PAGE + 1)*FILESPERPAGE) && num < loadedFileNames; num++) {
              set_font_XY(40, 60 + 20 * (num % FILESPERPAGE));

              if (filename[num][strlen(filename[num]) - 1] == '/') draw_string(filename[num], 23);
              else draw_string(filename[num], 255);

              if (DEBUG>4) {
                Serial.println(filename[num]);
                Serial.println(strlen(filename[num]));
              }
              delay___(1);
            }
            NamesDisplayed = true;
          }

          //Draw Cursor
          set_font_XY(16, 60 + (20 * (CURSOR % FILESPERPAGE)));
          draw_string((char*)"->", 255);

          if (LCD_ENABLED) xSemaphoreGive(draw_mutex); //refresh LCD
          vTaskDelay___(200); //ms

          //PROCESS CURSOR SELECTION
          INPUTCLEAR();
          while (!INPUTREAD()) {
            INPUTCLEAR();
            vTaskDelay___(1);
          }; //READ input

          //Empty Cursor
          set_font_XY(16, 60 + (20 * (CURSOR % FILESPERPAGE)));
          draw_string((char*)"  ", 0);

          if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
            INPUTCLEAR();     
            EXIT = 2;
            return MAINPATH;
          }

          if (INPUT_UP == 1) {
            if (CURSOR % FILESPERPAGE == 0) NamesDisplayed = false; //changed page
            if (CURSOR == 0 && loadedFileNames > 0) CURSOR = loadedFileNames - 1;
            else if (CURSOR > 0 && loadedFileNames > 0) CURSOR--;

            while(INPUTREAD()) {
              INPUTCLEAR();
            }       
          }
          if (INPUT_DOWN == 1) {
            if (CURSOR % FILESPERPAGE == FILESPERPAGE - 1 || CURSOR == loadedFileNames - 1) NamesDisplayed = false; //changed page
            if (CURSOR == loadedFileNames - 1 && loadedFileNames > 0) CURSOR = 0;
            else if (CURSOR < loadedFileNames - 1 && loadedFileNames > 0) CURSOR++;

            while(INPUTREAD()) {
              INPUTCLEAR();
            }  
          }
          if (INPUT_LEFT) {
            if (CURSOR > FILESPERPAGE - 1) CURSOR -= FILESPERPAGE;
            NamesDisplayed = false;
            while(INPUTREAD()) {
              INPUTCLEAR();
            }  
          }
          if (INPUT_RIGHT == 1) {
            if (CURSOR / FILESPERPAGE < loadedFileNames / FILESPERPAGE) CURSOR += FILESPERPAGE;
            if (CURSOR > loadedFileNames - 1) CURSOR = loadedFileNames - 1;
            NamesDisplayed = false;

            while(INPUTREAD()) {
              INPUTCLEAR();
            } 
          }
          if (INPUT_SELECT == 1) {
            //SWITCH SOURCE     
            while(INPUTREAD()) {
              INPUTCLEAR();
            } 
            delay___(50);

            if (LOADED_ROM==1) LOADED_ROM=11; //FLASH LOAD
            else if (LOADED_ROM==11) LOADED_ROM=21; //USB STORAGE LOAD 
            else if (LOADED_ROM==21) LOADED_ROM=1; //SD CARD LOAD

            EXIT = 1;
            return MAINPATH;
          }
          if (loadedFileNames > 0) { ///cannot select NOTHING!
            if ((INPUT_A == 1 || INPUT_B == 1) && INPUT_SELECT == 0) {
            while(INPUTREAD()) {
              INPUTCLEAR();
            }  
              delay___(50);
              sprintf(MAINPATH, "%s%s", PATH, filename[CURSOR]);
              return MAINPATH ; //START //A
            }
          }

          if (((INPUT_START == 1 ) && INPUT_SELECT == 0)) {
            while(INPUTREAD()) {
              INPUTCLEAR();
            }  
            delay___(25);
            sprintf(MAINPATH, "%s", PATH);

            if (strlen(MAINPATH) > 3) {
              MAINPATH[strlen(MAINPATH) - 1] = '\0';
              for (uint8_t strpos = strlen(MAINPATH) - 1; strpos > 0; strpos--) {
                if (MAINPATH[strpos] == '/') break;
                MAINPATH[strpos] = '\0';
              }
            }
            printf(">>> NEW MAINPATH: %s\n",MAINPATH);
            return MAINPATH;
          } 
        }
      } else { EXIT=1; LOADED_ROM=1; //SD CARD LOAD 
      }
    } else { EXIT=1; LOADED_ROM=1; //SD CARD LOAD 
    }
  } else { EXIT=1; LOADED_ROM=1; //SD CARD LOAD 
  }
#endif
#endif
  return MAINPATH; ///
}

//################################################################################
char* Browse_USB(uint16_t TYPE, char* PATH, char* MESSAGE) {
#if USBHOST
#if USBHOST_STORAGE
  if (USBhost_mass_storage == 2) {

    if (DEBUG>4) {
      Serial.print("USB: Browsing in PATH:");
      Serial.println(PATH);
    }

    if (PATH[strlen(PATH) - 1] != '/')
      if (strlen(PATH) > 3) {
        PATH[strlen(PATH) - 1] = '\0';
        for (uint8_t strpos = strlen(PATH) - 1; strpos > 0; strpos--) {
          if (PATH[strpos] == '/') break;
          PATH[strpos] = '\0';
        }
      }

    EXIT = 0;
//................................................................................
    if (!filename[0]) {
      printf(">>> alloc filenames\n");   
      if (PSRAM_BASE_SIZE > 0) for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char*)ps_malloc(MAXFILENAME_LENGTH + 1); //temporary ps_malloc filenamebuffer
    else
      for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char*)malloc(MAXFILENAME_LENGTH + 1); //temporary malloc filenamebuffer
    }  
    while (EXIT == 0 && PATH[strlen(PATH) - 1] == '/')  {
      Serial.print("USB PATH is:");
      Serial.println(PATH);
      PATH =  EXPLORE_USB(TYPE, PATH, MESSAGE);
      Serial.println(PATH);
    }
    if (EXIT==0) LOADED_ROM=22; //USB STORAGE LOAD SELECTED

    if (filename[0]) {
      printf(">>> free filenames\n");
      for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) {
        free(filename[tmp]); //free temporary malloc filenamebuffer
        filename[tmp]=NULL;
      }  
    }  
  //................................................................................
  }
#else //NoUSB
  LOADED_ROM=1; //MICROSD LOAD
#endif
#else //NoUSB
  LOADED_ROM=1; //MICROSD LOAD
#endif
  return PATH;
}
//________________________________________________________________________________






//********************************************************************************
//FILE LOADING FUNCTION...
//********************************************************************************
//--------------------------------------------------------------------------------
///IRAM_ATTR
uint8_t* PSRAM_load_file(char * filename, uint32_t* len) {
  printf("*** PSRAM_load_file_MICROSD *** \n");  
#if MICROSD
  SDCARD_BUSY=1; 
  uint8_t* data;
  fp = SD.open(filename);
  if (DEBUG>1) {
    Serial.print("FILE SIZE: ");
    Serial.println(fp.size());
  }
  *len = fp.size();

  DRAWMODE = 1;

  uint16_t BLOCKCOUNT = (*len) / (1024 * 32);
  if (BLOCKCOUNT <= 0) BLOCKCOUNT = 1;
  uint16_t BLOCKSIZEPX = 240 / BLOCKCOUNT;

  if (PSRAM_BASE_SIZE > 0) {
    Serial.print("FREEPSRAM: ");
    Serial.println(ESP.getMaxAllocPsram()); //free PSRAM
    if (ESP.getMaxAllocPsram() < fp.size()) {
      Serial.println("File is too big, not enough PSRAM memory. Abort");
      fp.close();
      return (0);
    }
    data = (uint8_t *)ps_malloc(fp.size() + 1 );
    uint32_t i = 0;
    for (i = 0; i < fp.size(); i++) {
      ((uint8_t*)data)[i] = fp.read();
      if (i % 1024 == 0) {
        sprintf(TEXTBUFFER, " %d / %d kB", i / 1024, (*len / 1024));
        set_font_XY(8, 8);
        draw_string((char*)"Loaded:");
        draw_string(TEXTBUFFER);
        screenmemory_drawfillrectangle(((i / (1024 * 32)) - 0) * BLOCKSIZEPX, 24, BLOCKSIZEPX, 16, 192);
        if (i % 20480 == 0) {
          if (DEBUG>1) {
            Serial.print("█");
          }
        }
      }
    }
  }
  if (DEBUG>1) {
    printf("[ OK ]\n");
  }
  fp.close();
  SDCARD_BUSY=0;
  return data;
#endif  
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
uint8_t* PSRAM_load_file_silent(char * filename, uint32_t* len) {
  printf("*** PSRAM_load_file_MICROSD *** \n"); 
#if MICROSD
  SDCARD_BUSY=1;  
  uint8_t* data;
  fp = SD.open(filename);
  if (DEBUG>1) {
    Serial.print("FILE SIZE: ");
    Serial.println(fp.size());
  }
  *len = fp.size();
  if (PSRAM_BASE_SIZE > 0) {
    Serial.print("FREEPSRAM: ");
    Serial.println(ESP.getMaxAllocPsram()); //free PSRAM
    if (ESP.getMaxAllocPsram() < fp.size()) {
      Serial.println("File is too big, not enough PSRAM memory. Abort");
      fp.close();
      return (0);
    }
    data = (uint8_t *)ps_malloc(fp.size() + 1 );

    uint32_t i = 0;
    for (i = 0; i < fp.size(); i++) {
      ((uint8_t*)data)[i] = fp.read();      
    }
  }
  if (DEBUG>1) {
    printf("[ OK ]\n");
  }
  fp.close();
  SDCARD_BUSY=0;
  return data;
#endif  
}
//--------------------------------------------------------------------------------

//********************************************************************************
//HIMEM FILE LOADING FUNCTION...
//********************************************************************************
uint8_t *himem_data_buffer;

//--------------------------------------------------------------------------------
uint32_t HIMEM_load_file(char * filename, uint32_t* len) { ///loads file at HIMEM address 0...SIZE
  printf("*** HIMEM_load_file_MICROSD *** \n");

#if MICROSD
  SDCARD_BUSY=1;
  
  fp = SD.open(filename);
  if (DEBUG>1) {
    Serial.print("FILE SIZE: ");
    Serial.println(fp.size());
  }
  *len = fp.size();

  DRAWMODE = 1;

  uint16_t BLOCKCOUNT = (*len) / (1024 * 32);
  if (BLOCKCOUNT <= 0) BLOCKCOUNT = 1;
  uint16_t BLOCKSIZEPX = 240 / BLOCKCOUNT;

#if HIMEM
  Serial.print("FREEPSRAM_HIMEM allocated: ");
  Serial.println(PSRAM_HIMEM_SIZE); //free PSRAM
  if (PSRAM_HIMEM_SIZE < fp.size()) {
    Serial.println("File is too big, not enough PSRAM HIMEM memory. Abort");
    fp.close();
    return (0) ;
  }
  himem_data_buffer = (uint8_t *)malloc(ESP_HIMEM_BLKSZ);
  uint32_t tmp = 0;
  uint32_t Blocksize = ESP_HIMEM_BLKSZ;
  uint32_t i = 0;
  for (i = 0; i < *len; i++) {

    if (i % Blocksize == 0 && i > 0) {
      vPortYield(); ///

      Serial.print("himem_write ADDR: ");
      Serial.println(i - Blocksize);
      himem_write(i - Blocksize, Blocksize, himem_data_buffer);


      sprintf(TEXTBUFFER, " %d / %d kB", i / 1024, (*len / 1024));
      set_font_XY(8, 8);
      draw_string((char*)"Loaded:");
      draw_string(TEXTBUFFER);
      screenmemory_drawfillrectangle(((i / (1024 * 32)) - 0) * BLOCKSIZEPX, 24, BLOCKSIZEPX, 16, 57);
      if (i % (1024 * 4) == 0) {
        if (DEBUG>1) {
          Serial.print("█");
        }
      }
    }

    ((uint8_t*)himem_data_buffer)[i % Blocksize] = fp.read();
  }
  if (i >= (i - (i % Blocksize))) {
    Serial.print("himem_write ADDR final: ");
    Serial.println(i - (i % Blocksize) - Blocksize);
    himem_write(i - (i % Blocksize) - Blocksize, Blocksize, himem_data_buffer);
  }

  if (DEBUG>1) {
    printf("[ OK ]\n");
  }
  free(himem_data_buffer);
#endif
  fp.close();

  SDCARD_BUSY=0;
  return (*len); //return size
#endif
}
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
uint32_t HIMEM_load_flash(char * filename, uint32_t* len) { ///loads file at HIMEM address 0...SIZE
  printf("*** HIMEM_load_file_FLASH *** \n");
#if MICROSD  
//................................................................................
  uint16_t pos=0;
  while (pos<MAXFILES && pos<FLASH_FILES_COUNT) {
    if (strcmp(filename_flash[pos],filename)==0) break;
    pos++;  
  }

  switch (pos) {
#ifndef NCAT_NO_PSRAM
    case 0: romdata_flash = flash000;
      *len = sizeof(flash000);
      break;
    case 1: romdata_flash = flash001;
      *len = sizeof(flash001);
      break;
    case 2: romdata_flash = flash002;
      *len = sizeof(flash002);
      break;
    case 3: romdata_flash = flash003;
      *len = sizeof(flash003);
      break;
    case 4: romdata_flash = flash004;
      *len = sizeof(flash004);
      break;
    case 5: romdata_flash = flash005;
      *len = sizeof(flash005);
      break;
    case 6: romdata_flash = flash006;
      *len = sizeof(flash006);
      break;
    case 7: romdata_flash = flash007;
      *len = sizeof(flash007);
      break;
#endif      
    default:
      return 0;
      break;  
  }  
//................................................................................
  DRAWMODE = 1;
  uint16_t BLOCKCOUNT = (*len) / (1024 * 32);
  if (BLOCKCOUNT <= 0) BLOCKCOUNT = 1;
  uint16_t BLOCKSIZEPX = 240 / BLOCKCOUNT;

#if HIMEM
  Serial.print("FREEPSRAM_HIMEM allocated: ");
  Serial.println(PSRAM_HIMEM_SIZE); //free PSRAM
  if (PSRAM_HIMEM_SIZE < fp.size()) {
    Serial.println("File is too big, not enough PSRAM HIMEM memory. Abort");
    fp.close();
    return (0) ;
  }

  himem_data_buffer = (uint8_t *)malloc(ESP_HIMEM_BLKSZ);

  uint32_t tmp = 0;
  uint32_t Blocksize = ESP_HIMEM_BLKSZ;
  uint32_t i = 0;
  for (i = 0; i < *len; i++) {

    if (i % Blocksize == 0 && i > 0) {
      vPortYield(); ///

      Serial.print("himem_write ADDR: ");
      Serial.println(i - Blocksize);
      himem_write(i - Blocksize, Blocksize, himem_data_buffer);
      sprintf(TEXTBUFFER, " %d / %d kB", i / 1024, (*len / 1024));
      set_font_XY(8, 8);
      draw_string((char*)"Loaded:");
      draw_string(TEXTBUFFER);
      screenmemory_drawfillrectangle(((i / (1024 * 32)) - 0) * BLOCKSIZEPX, 24, BLOCKSIZEPX, 16, 57);
      if (i % (1024 * 4) == 0) {
        if (DEBUG>1) {
          Serial.print("█");
        }
      }
    }
//................................................................................
    ((uint8_t*)himem_data_buffer)[i % Blocksize] = romdata_flash[i];
//................................................................................
  }
  if (i >= (i - (i % Blocksize))) {
    Serial.print("himem_write ADDR final: ");
    Serial.println(i - (i % Blocksize) - Blocksize);
    himem_write(i - (i % Blocksize) - Blocksize, Blocksize, himem_data_buffer);
  }

  if (DEBUG>1) {
    printf("[ OK ]\n");
  }
  free(himem_data_buffer);
#endif
  return (*len); //return size
#endif  
}
//--------------------------------------------------------------------------------

//********************************************************************************
//IRAM_ATTR 
uint32_t HIMEM_load_usbstorage(char * filename, uint32_t* len) { ///loads file at HIMEM address 0...SIZE
  printf("*** HIMEM_load_file_USBSTORAGE *** \n");
 
#if USBHOST 
#if USBHOST_STORAGE
  if (USBhost_mass_storage == 2) {
//--------------------------------------------------------------------------------
    while (USBSTORAGE_BUSY!=1) {
      vTaskDelay___(1000);  ///
    }       
    if (!SDCARD_BUSY && USBSTORAGE_BUSY==1 && usbon) {
      printf("LOAD_USB: !SDCARD_BUSY & usbon \n");
      USBSTORAGE_BUSY=2;
     
      if(fatready && Fats[0] != NULL) {
        printf("LOAD_USB: fatready \n");
    
        FIL My_File_Object_x; /* File object */  
        FRESULT rc; // Result code 
        UINT bw, br; ///

        FATFS *fs = NULL;
        for (uint8_t zz = 0; zz < _VOLUMES; zz++) {
          if(Fats[zz]->volmap == 0) fs = Fats[zz]->ffs;
        } 
//--------------------------------------------------------------------------------
        uint8_t* data;

        rc = f_open(&My_File_Object_x, filename, FA_READ);

        if(rc) {
          printf("Usb Storage file load error...\n");
          while (1) {}; //halt.
        }

//................................................................................ 
        *len = My_File_Object_x.fsize;
        if (DEBUG>1) {
          Serial.print("USBSTORAGE FILE SIZE: ");
          Serial.println(*len);
        }
//................................................................................ 
        DRAWMODE = 1;

        uint16_t BLOCKCOUNT = (*len) / (1024 * 32);
        if (BLOCKCOUNT <= 0) BLOCKCOUNT = 1;
        uint16_t BLOCKSIZEPX = 240 / BLOCKCOUNT;

      #if HIMEM
        Serial.print("FREEPSRAM_HIMEM allocated: ");
        Serial.println(PSRAM_HIMEM_SIZE); //free PSRAM
        if (PSRAM_HIMEM_SIZE < *len) {
          Serial.println("File is too big, not enough PSRAM HIMEM memory. Abort");
          ///fp.close();
          rc = f_close(&My_File_Object_x);
          return (0) ;
        }

        himem_data_buffer = (uint8_t *)malloc(ESP_HIMEM_BLKSZ); //can not be ps_malloc! must be malloc!

        uint32_t tmp = 0;
        uint32_t Blocksize = ESP_HIMEM_BLKSZ;
        uint32_t i = 0;
        for (i = 0; i < *len; /*i++*/) { ///DO NOT INC i !!!   
          vPortYield(); /// !!!
          if (i % Blocksize == 0 && i > 0) {
            vTaskDelay___(5);
            Serial.print("himem_write ADDR: ");
            Serial.println(i - Blocksize);
            himem_write(i - Blocksize, Blocksize, himem_data_buffer);

            sprintf(TEXTBUFFER, " %d / %d kB", i / 1024, (*len / 1024));
            set_font_XY(8, 8);
            draw_string((char*)"Loaded:");
            draw_string(TEXTBUFFER);
            screenmemory_drawfillrectangle(((i / (1024 * 32)) - 0) * BLOCKSIZEPX, 24, BLOCKSIZEPX, 16, 57);
            if (i % (1024 * 4) == 0) {
              if (DEBUG>1) {
                Serial.print("█");
              }
            }
          }
//................................................................................ 
          rc = f_read(&My_File_Object_x, &himem_data_buffer[i%ESP_HIMEM_BLKSZ], 128, &br); /* Read a chunk of file */  ///reads 128bytes
          if(rc || !br) {
            printf("Error or end of file\n");
            break; /* Error or end of file */
          }
          i+=128; 
          printf("i:%d\n",i);
//................................................................................ 
        }
        if (i >= (i - (i % Blocksize))) {
          Serial.print("himem_write ADDR final: ");
          Serial.println(i - (i % Blocksize) - Blocksize);
          himem_write(i - (i % Blocksize) - Blocksize, Blocksize, himem_data_buffer);
        }
        if (DEBUG>1) {
          printf("[ OK ]\n");
        }
        free(himem_data_buffer);
#endif
//................................................................................
        if(rc) {
          f_close(&My_File_Object_x);
        }
        rc = f_close(&My_File_Object_x);
        if(rc) while(1) {} ///halt.
        DISK_IOCTL(fs->drv, CTRL_COMMIT, 0); ///like FILE CLOSE   
//................................................................................  
        USBSTORAGE_BUSY=0;
        return (*len); //return size
      }
//--------------------------------------------------------------------------------
    }
  }
#endif
#endif 
  return 0;
}

//********************************************************************************
//FLASH BROWSING FUNCTION...
//********************************************************************************

//################################################################################
char* EXPLORE_FLASH(uint16_t TYPE, char* PATH, char* MESSAGE) {   
  uint8_t num = 0;
  uint8_t loadedFileNames = 0;
  //clear memory variables
  for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) memset (filename[tmp], 0, sizeof(filename[tmp]));
  fileext[0] = 0;
  fileext[1] = 0;
  fileext[2] = 0;
  fileext[3] = 0;

  num = 0;
  loadedFileNames = 0;

  //LOAD FILENAMES INTO MEMORY...
  uint16_t num_flash=0;
  while (num < MAXFILES && num_flash<MAXFILES && num_flash<FLASH_FILES_COUNT) {
    strcpy(filename[num],filename_flash[num_flash]);
    Serial.println(filename[num]);
    for (uint8_t i = strlen(filename[num]); i > 3; i--) {
      if (filename[num][i] != 0) {
        fileext[3] = '\0';
        fileext[2] = filename[num][i];
        fileext[1] = filename[num][i - 1];
        fileext[0] = filename[num][i - 2];
        break;
      }
    }
    if (TYPE == APP_EMU_FAKE86) {
      //check DOS IMAGES File extension, then increase index
      if (((fileext[0] == 'B' || fileext[0] == 'b')
          && (fileext[1] == 'I' || fileext[1] == 'i')
          && (fileext[2] == 'N' || fileext[2] == 'n'))
         ||
         ((fileext[0] == 'I' || fileext[0] == 'i')
          && (fileext[1] == 'M' || fileext[1] == 'm')
          && (fileext[2] == 'G' || fileext[2] == 'g'))
         ||
         ((fileext[0] == 'V' || fileext[0] == 'v')
          && (fileext[1] == 'F' || fileext[1] == 'f')
          && (fileext[2] == 'D' || fileext[2] == 'd'))
        ) {
        num++;
      }
    }
    if (TYPE == APP_EMU_NES) {
      //check NES ROM IMAGES File extension, then increase index
      if (((fileext[0] == 'N' || fileext[0] == 'n')
        && (fileext[1] == 'E' || fileext[1] == 'e')
        && (fileext[2] == 'S' || fileext[2] == 's'))
      ) {
        num++;
      }
    }
    if (TYPE == APP_EMU_GBC) {
      //check GBC ROM IMAGES File extension, then increase index
      if (((fileext[0] == 'G' || fileext[0] == 'g')
        && (fileext[1] == 'B' || fileext[1] == 'b')
        && (fileext[2] == 'C' || fileext[2] == 'c'))
      ) {
        num++;
      }
    }        
    if (TYPE == APP_EMU_SMS) {
      //check SMS ROM IMAGES File extension, then increase index
      if (((fileext[0] == 'S' || fileext[0] == 's')
        && (fileext[1] == 'M' || fileext[1] == 'm')
        && (fileext[2] == 'S' || fileext[2] == 's'))
      ) {
        num++;
      }
    }        
    num_flash++;
  }
  loadedFileNames = num;
  if (DEBUG>1) {
    Serial.println("--------------------------------------");
    Serial.print("Count of loaded File Names:");
    Serial.println(loadedFileNames);
  }

  sortStrings(filename, loadedFileNames);

  //DRAW FILENAMES INTO BUFFER
  uint8_t CURSOR = 0;
  uint8_t PAGE = 0;
  bool NamesDisplayed = false;

  while (1) {
    vPortYield(); //we are in infinite loop must yield();
    vTaskDelay___(1);
    
    PAGE = CURSOR / FILESPERPAGE;
    if (!NamesDisplayed) {
      screenmemory_fillscreen(0); //black color
      set_font_XY(16, 12 );
      draw_string(MESSAGE, 0,23);
      set_font_XY(16, 36 );
      draw_string(PATH, 23);

      for (num = PAGE * FILESPERPAGE; num < ((PAGE + 1)*FILESPERPAGE) && num < loadedFileNames; num++) {
        set_font_XY(40, 60 + 20 * (num % FILESPERPAGE));
        ///draw_string(filename[num],48);

        if (filename[num][strlen(filename[num]) - 1] == '/') draw_string(filename[num], 23);
        else draw_string(filename[num], 255);
        if (DEBUG>2) {
          Serial.println(filename[num]);
          Serial.println(strlen(filename[num]));
        }
        delay___(1);
      }
      NamesDisplayed = true;
    }

    //Draw Cursor
    set_font_XY(16, 60 + (20 * (CURSOR % FILESPERPAGE)));
    draw_string((char*)"->", 255);
    
    if (LCD_ENABLED) xSemaphoreGive(draw_mutex); //refresh LCD
    vTaskDelay___(200); //ms

    //PROCESS CURSOR SELECTION

    INPUTCLEAR();
    while (!INPUTREAD()) {
      INPUTCLEAR();
      vTaskDelay___(1);
    }; 

    //Empty Cursor
    set_font_XY(16, 60 + (20 * (CURSOR % FILESPERPAGE)));
    draw_string((char*)"  ", 0);

    if ((INPUT_START == 1 && INPUT_SELECT == 1) || (INPUT_SYSTEM == 1)) {
      INPUTCLEAR();     
      EXIT = 2;
      return MAINPATH;
    }

    if (INPUT_UP == 1) {
      if (CURSOR % FILESPERPAGE == 0) NamesDisplayed = false; //changed page
      if (CURSOR == 0 && loadedFileNames > 0) CURSOR = loadedFileNames - 1;
      else if (CURSOR > 0 && loadedFileNames > 0) CURSOR--;

      while(INPUTREAD()) {
        INPUTCLEAR();
      }       
    }
    if (INPUT_DOWN == 1) {
      if (CURSOR % FILESPERPAGE == FILESPERPAGE - 1 || CURSOR == loadedFileNames - 1) NamesDisplayed = false; //changed page
      if (CURSOR == loadedFileNames - 1 && loadedFileNames > 0) CURSOR = 0;
      else if (CURSOR < loadedFileNames - 1 && loadedFileNames > 0) CURSOR++;

      while(INPUTREAD()) {
        INPUTCLEAR();
      }  
    }
    if (INPUT_LEFT) {
      if (CURSOR > FILESPERPAGE - 1) CURSOR -= FILESPERPAGE;
      NamesDisplayed = false;
      while(INPUTREAD()) {
        INPUTCLEAR();
      }  
    }
    if (INPUT_RIGHT == 1) {
      if (CURSOR / FILESPERPAGE < loadedFileNames / FILESPERPAGE) CURSOR += FILESPERPAGE;
      if (CURSOR > loadedFileNames - 1) CURSOR = loadedFileNames - 1;
      NamesDisplayed = false;
      
      while(INPUTREAD()) {
        INPUTCLEAR();
      } 
    }
    if (INPUT_SELECT == 1) { // SWITCH SOURCE DRIVE
      while(INPUTREAD()) {
        INPUTCLEAR();
      }  
      delay___(250);      
      if (LOADED_ROM==1) LOADED_ROM=11; //FLASH LOAD
      else if (LOADED_ROM==11) LOADED_ROM=21; //USB STORAGE LOAD
      else if (LOADED_ROM==21) LOADED_ROM=1; //SD CARD LOAD

      EXIT = 1;
      return MAINPATH;
    }
    if (loadedFileNames > 0) { // cannot select NOTHING!
      if ((INPUT_A == 1 || INPUT_B == 1) && INPUT_SELECT == 0) {
        vTaskDelay___(200); //ms

        while(INPUTREAD()) {
          INPUTCLEAR();
        }  
        sprintf(MAINPATH, "%s", filename[CURSOR]);
        return MAINPATH ; //START //A
      } 
      if (((INPUT_START == 1 ) && INPUT_SELECT == 0)) { // Can not DIRUP in FLASH
        while(INPUTREAD()) {
          INPUTCLEAR();
        }  
      }
    }   
  }
}
//################################################################################
char* Browse_FLASH(uint16_t TYPE, char* PATH, char* MESSAGE) {
  if (DEBUG>1) {
    Serial.print("FLASH: Browsing in PATH:");
    Serial.println(PATH);
  }
  if (PATH[strlen(PATH) - 1] != '/')
    if (strlen(PATH) > 1) {
      PATH[strlen(PATH) - 1] = '\0';
      for (uint8_t strpos = strlen(PATH) - 1; strpos > 0; strpos--) {
        if (PATH[strpos] == '/') break;
        PATH[strpos] = '\0';
      }
    }

  EXIT = 0;
//................................................................................
  if (!filename[0]) {
    printf(">>> alloc filenames\n");
    if (PSRAM_BASE_SIZE > 0)
      for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char*)ps_malloc(MAXFILENAME_LENGTH + 1); //temporary ps_malloc filenamebuffer
    else
      for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char*)malloc(MAXFILENAME_LENGTH + 1); //temporary malloc filenamebuffer
  }  
  while (EXIT == 0 && PATH[strlen(PATH) - 1] == '/')  {
    PATH =  EXPLORE_FLASH(TYPE, PATH, MESSAGE);
    Serial.println(PATH);
  }

  if (EXIT==0) LOADED_ROM=12; //FLASH LOAD SELECTED

  if (filename[0]) {
    printf(">>> free filenames\n");
    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) {
      free(filename[tmp]); //free temporary malloc filenamebuffer
      filename[tmp]=NULL;
    }    
  }  
//................................................................................
  return PATH;
}
//________________________________________________________________________________



//********************************************************************************
//FLASH LOADING FUNCTION...
//********************************************************************************
//--------------------------------------------------------------------------------
uint8_t* PSRAM_load_flash(char * filename, uint32_t* len) {
  printf("*** PSRAM_load_file_FLASH *** \n");
  uint8_t* data;
//................................................................................
  uint16_t pos=0;
  while (pos<MAXFILES && pos<FLASH_FILES_COUNT) {
    if (strcmp(filename_flash[pos],filename)==0) break;
    pos++;  
  }
  switch (pos) {
#ifndef NCAT_NO_PSRAM
    case 0: romdata_flash = flash000;
      *len = sizeof(flash000);
      break;
    case 1: romdata_flash = flash001;
      *len = sizeof(flash001);
      break;
    case 2: romdata_flash = flash002;
      *len = sizeof(flash002);
      break;
    case 3: romdata_flash = flash003;
      *len = sizeof(flash003);
      break;
    case 4: romdata_flash = flash004;
      *len = sizeof(flash004);
      break;
    case 5: romdata_flash = flash005;
      *len = sizeof(flash005);
      break;
    case 6: romdata_flash = flash006;
      *len = sizeof(flash006);
      break;
    case 7: romdata_flash = flash007;
      *len = sizeof(flash007);
      break;
#endif      
    default:
      return 0;
      break;  
  }  
//................................................................................
  if (DEBUG>1) {
    Serial.print("FLASH SIZE: ");
    Serial.println(*len);
  }
  DRAWMODE = 1;
  uint16_t BLOCKCOUNT = (*len) / (1024 * 32);
  if (BLOCKCOUNT <= 0) BLOCKCOUNT = 1;
  uint16_t BLOCKSIZEPX = 240 / BLOCKCOUNT;

  if (PSRAM_BASE_SIZE > 0) {
    Serial.print("FREEPSRAM: ");
    Serial.println(ESP.getMaxAllocPsram()); //free PSRAM
    if (ESP.getMaxAllocPsram() < *len) {
      Serial.println("File is too big, not enough PSRAM memory. Abort");
      return (0);
    }

    data = (uint8_t *)ps_malloc(*len + 1 );

#ifdef DEBUG
    Serial.println("ps_malloc done.");
#endif
    uint32_t i = 0;
    for (i = 0; i < *len; i++) {
      ((uint8_t*)data)[i] = romdata_flash[i];

      if (i % 1024 == 0) {
        ///vPortYield(); ///

        sprintf(TEXTBUFFER, " %d / %d kB", i / 1024, (*len / 1024));
        set_font_XY(8, 8);
        draw_string((char*)"Loaded:");
        draw_string(TEXTBUFFER);
        screenmemory_drawfillrectangle(((i / (1024 * 32)) - 0) * BLOCKSIZEPX, 24, BLOCKSIZEPX, 16, 23);
        if (i % 20480 == 0) {
          if (DEBUG>1) { 
            Serial.print("█");
          }
        }
      }
    }
  }
  if (DEBUG>1) {
    printf("[ OK ]\n");
  }
  return data;
}
//--------------------------------------------------------------------------------





//********************************************************************************
//FILE LOADING FUNCTION...
//********************************************************************************
//--------------------------------------------------------------------------------
//IRAM_ATTR 
uint8_t* PSRAM_load_usbstorage(char * filename, uint32_t* len) {
  printf("*** PSRAM_load_file_USBSTORAGE *** \n");
  
#if USBHOST 
#if USBHOST_STORAGE
  if (USBhost_mass_storage == 2) {
//--------------------------------------------------------------------------------
    while (USBSTORAGE_BUSY!=1) {
      vTaskDelay___(1000);  ///
    }       
    if (!SDCARD_BUSY && USBSTORAGE_BUSY==1 && usbon) {
      vPortYield(); ///
      printf("LOAD_USB: !SDCARD_BUSY & usbon \n");
      USBSTORAGE_BUSY=2;

      if(fatready && Fats[0] != NULL) {
        printf("LOAD_USB: fatready \n");

        FIL My_File_Object_x; /* File object */  
        FRESULT rc; // Result code 
        UINT bw, br, i;

        FATFS *fs = NULL;
        for (uint8_t zz = 0; zz < _VOLUMES; zz++) {
          if(Fats[zz]->volmap == 0) fs = Fats[zz]->ffs;
          vPortYield(); ///
        } 
//--------------------------------------------------------------------------------
        uint8_t* data;

        rc = f_open(&My_File_Object_x, filename, FA_READ);

        if(rc) {
          printf("Usb Storage file load error...\n");
          while (1) {}; //halt.
        }
//................................................................................ 
        *len = My_File_Object_x.fsize;
#ifdef DEBUG
        printf("USBSTORAGE FILE SIZE: %d\n",*len);
#endif
//................................................................................ 
        DRAWMODE = 1;

        uint16_t BLOCKCOUNT = (*len) / (1024 * 32);
        if (BLOCKCOUNT <= 0) BLOCKCOUNT = 1;
        uint16_t BLOCKSIZEPX = 240 / BLOCKCOUNT;

        if (PSRAM_BASE_SIZE > 0) {
          printf("FREEPSRAM: %d\n",ESP.getMaxAllocPsram());
          if (ESP.getMaxAllocPsram() < *len) {
            printf("File is too big, not enough PSRAM memory. Abort.\n");
            rc = f_close(&My_File_Object_x);
            return (0);
          }
          data = (uint8_t *)ps_malloc(*len);
          uint32_t i = 0;
          for (i = 0; i < *len; /*i++*/) {
            vPortYield(); ///       
//................................................................................ 
            rc = f_read(&My_File_Object_x, &data[i], 512, &br); /* Read a chunk of file */  ///reads 512bytes
            if(rc || !br) break; /* Error or end of file */
            i+=512; 
//................................................................................ 
            if (i % 1024 == 0) {
              sprintf(TEXTBUFFER, " %d / %d kB", i / 1024, (*len / 1024));
              set_font_XY(8, 8);
              draw_string((char*)"Loaded:");
              draw_string(TEXTBUFFER);
              screenmemory_drawfillrectangle(((i / (1024 * 32)) - 0) * BLOCKSIZEPX, 24, BLOCKSIZEPX, 16, 57);
              if (i % 20480 == 0) {
                if (DEBUG>1) {
                  printf("█");
                }
              }
            }
          }
        }
        if (DEBUG>1) {
          printf("[ OK ]\n");
        }
//................................................................................
        if(rc) {
          f_close(&My_File_Object_x);
        }
        rc = f_close(&My_File_Object_x);
//................................................................................
        if(rc) {
          printf("USB read ERROR\n");
          while(1) {} ///halt.
        }
        DISK_IOCTL(fs->drv, CTRL_COMMIT, 0); ///like FILE CLOSE
//................................................................................
        USBSTORAGE_BUSY=0;
        return data;
      }
    }
  }
#endif
#endif
  return (uint8_t*)NULL; 
}
//--------------------------------------------------------------------------------



//********************************************************************************
//
//********************************************************************************
uint32_t WALLPAPERDATA_SIZE=0;
uint8_t* WALLPAPER_DATA;

void MENU_TASK_START(int8_t STARTMENU) {
  printf("Starting MAIN MENU...");
  if (Extra_apps==2) MAX_MENU_ITEMS=10; 
#if JPG_DECODER
  if (Wallpaper==0) {
//--------------------------------------------------------------------------------
#if KEEP_WALLPAPER_IN_PSRAM    
    if (!WALLPAPER_DATA) WALLPAPER_DATA = PSRAM_load_file_silent((char*)"/wallpaper.jpg", &WALLPAPERDATA_SIZE); ///
    printf("WALLPAPERDATA_SIZE: %d\n",WALLPAPERDATA_SIZE);
    memset(&BACKGROUND_BUFFER[0],0,BACKGROUND_BUFFER_SIZE);     
    if (WALLPAPERDATA_SIZE) {      
      renderJPG_toBUFFER(&BACKGROUND_BUFFER[0], 0, 0, &WALLPAPER_DATA[0], WALLPAPERDATA_SIZE, JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale          
    }
#else   
    WALLPAPERDATA_SIZE=0;  
    WALLPAPER_DATA = PSRAM_load_file_silent("/wallpaper.jpg", &WALLPAPERDATA_SIZE); ///
    printf("WALLPAPERDATA_SIZE: %d\n",WALLPAPERDATA_SIZE);
    memset(&BACKGROUND_BUFFER[0],0,BACKGROUND_BUFFER_SIZE);
    if (WALLPAPERDATA_SIZE) {         
      renderJPG_toBUFFER(&BACKGROUND_BUFFER[0], 0, 0, &WALLPAPER_DATA[0], WALLPAPERDATA_SIZE, JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale     
      WALLPAPERDATA_SIZE=0;
      free(WALLPAPER_DATA); //free NES ROM data
    }
#endif    
//--------------------------------------------------------------------------------
  } else {
    if (Wallpaper==10) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_00, sizeof(JPG_WALLPAPER_00), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
    } else if (Wallpaper==11) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_01, sizeof(JPG_WALLPAPER_01), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
    } else if (Wallpaper==12) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_02, sizeof(JPG_WALLPAPER_02), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale    
    } else if (Wallpaper==13) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_03, sizeof(JPG_WALLPAPER_03), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
    } else if (Wallpaper==14) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_04, sizeof(JPG_WALLPAPER_04), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
    } else if (Wallpaper==15) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_05, sizeof(JPG_WALLPAPER_05), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
    } else if (Wallpaper==16) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_06, sizeof(JPG_WALLPAPER_06), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
    } else if (Wallpaper==17) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_07, sizeof(JPG_WALLPAPER_07), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
    } else if (Wallpaper==18) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_08, sizeof(JPG_WALLPAPER_08), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale      
    } else if (Wallpaper==19) {
      renderJPG_toBUFFER(BACKGROUND_BUFFER, 0, 0, JPG_WALLPAPER_09, sizeof(JPG_WALLPAPER_09), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale      
    } else {
    }
  }
#endif
#ifndef NCAT_NO_PSRAM
  //Render Status Bar
  renderTRANSPARENTRECT(BACKGROUND_BUFFER, 0, 0, 240, 36);
#endif
  if (MENU_ITEM==0) {
     TIME_COUNTER_BREAK=1; ///Init Clock Request
  }
  //render START ICON of MENU
  if (MENU_ITEM == 1) {    
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_NES_ICON, sizeof(PNG_NES_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("NES EMULATOR")*16))/ 2 , 104, "NES EMULATOR", font_Ishmeria8x16, 0xffff);    
#endif    
  }
  if (MENU_ITEM == 2) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_GBC_ICON, sizeof(PNG_GBC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("GBC EMULATOR")*16))/ 2 , 104, "GBC EMULATOR", font_Ishmeria8x16, 0xffff);
#endif 
  }
  if (MENU_ITEM == 3) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_SMS_ICON, sizeof(PNG_SMS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("SMS EMULATOR")*16))/ 2 , 104, "SMS EMULATOR", font_Ishmeria8x16, 0xffff);
#endif 
  }
  if (MENU_ITEM == 4) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_MP3_ICON, sizeof(PNG_MP3_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("MP3 PLAYER")*16))/ 2 , 104, "MP3 PLAYER", font_Ishmeria8x16, 0xffff);
#endif 
  }
  if (MENU_ITEM == 5) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_BTSPEAKER_ICON, sizeof(PNG_BTSPEAKER_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("BT SPEAKER")*16))/ 2 , 104, "BLUETOOTH SPEAKER", font_Ishmeria8x16, 0b1111100000000000);
#endif 
  }
  if (MENU_ITEM == 6) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_X86EMU_ICON, sizeof(PNG_X86EMU_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("FAKE86 EMULATOR")*16))/ 2 , 104, "FAKE86 EMULATOR", font_Ishmeria8x16, 0b1111100000000000);
#endif 
  }
  if (MENU_ITEM == 7) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_SETTINGS_ICON, sizeof(PNG_SETTINGS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("SETTINGS")*16))/ 2 , 104, "SETTINGS", font_Ishmeria8x16, 0xffff);
#endif 
  }
  if (MENU_ITEM == 8) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_KMSW_ICON, sizeof(PNG_KMSW_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("KM SWITCH")*16))/ 2 , 104, "KM SWITCH", font_Ishmeria8x16, 0b1111100000000000);   
#endif 
  }
  if (MENU_ITEM == 9) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_OSC_ICON, sizeof(PNG_OSC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("OSCILLOSCOPE")*16))/ 2 , 104, "OSCILLOSCOPE", font_Ishmeria8x16, 0b1111100000000000);
#endif 
  }
  if (MENU_ITEM == 10) {
#ifndef NCAT_NO_PSRAM
    renderPNG((240-176)/2, (240-160)/2, PNG_SIGGEN_ICON, sizeof(PNG_SIGGEN_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else
    draw_string_xy_transparent_double((240 - (strlen("SIG GENERATOR")*16))/ 2 , 104, "SIG GENERATOR", font_Ishmeria8x16, 0b1111100000000000);
#endif 
  }

  MENU_ACTIVE=1;   
  printf("[ OK ]\n");
}

//--------------------------------------------------------------------------------
int8_t MENU_TASK(int8_t STARTMENU) {
  if (MENU_ACTIVE==0) MENU_TASK_START(STARTMENU);
  if (MENU_ACTIVE!=1) return 0;
  INPUTCLEAR(); 
  if (MENU_ITEM==0) {
    if (POWER!=APP_EMU_CLOCK && TIME_COUNTER_BREAK==1) {
      POWER=APP_EMU_CLOCK;
      CLOCK_MODE=1;
    }
    DO_CLOCK_TASK();
  }
  UPDATE_STATUS_BAR();
  vPortYield();
  vTaskDelay___(1); ///we are in infinite loop so yielding
  INPUTREAD();   
  if (INDICATOR_MODE>0 && VOLUME_INDICATOR_TIMEOUT > 0 && VOLUME_INDICATOR_TIMEOUT + 3000 < millis___()) HIDE_VOLUME_INDICATOR();   
  else if (INDICATOR_MODE>0 && BACKLIGHT_INDICATOR_TIMEOUT > 0 && BACKLIGHT_INDICATOR_TIMEOUT + 3000 < millis___()) HIDE_BACKLIGHT_INDICATOR();
//--------------------------------------------------------------------------------
  if (INPUT_RIGHT == 1)  {
    if (MENU_ITEM==0) DO_DEINIT_CLOCK(); 
    if (INDICATOR_MODE==1) HIDE_VOLUME_INDICATOR();
    else if (INDICATOR_MODE==2) HIDE_BACKLIGHT_INDICATOR();
//................................................................................
    MENU_ITEM++;
    if (MENU_ITEM > MAX_MENU_ITEMS) MENU_ITEM = 0;
    if (MENU_ITEM == 1) {
#if PNG_DECODER
      animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_NES_ICON, sizeof(PNG_NES_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(-16, 0 + (240 - (strlen("NES EMULATOR")*16))/2, 240 , 104, "NES EMULATOR", 0xffff);
#endif        
    }
    if (MENU_ITEM == 2) {
#if PNG_DECODER
      animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_NES_ICON, sizeof(PNG_NES_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_GBC_ICON, sizeof(PNG_GBC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(-16, -240 , 0 + (240 - (strlen("NES EMULATOR")*16))/2, 104, "NES EMULATOR", 0xffff);
      animateTEXT(-16, 0 + (240 - (strlen("GBC EMULATOR")*16))/2, 240 , 104, "GBC EMULATOR", 0xffff);
#endif        
    }
    if (MENU_ITEM == 3) {
#if PNG_DECODER
      animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_GBC_ICON, sizeof(PNG_GBC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_SMS_ICON, sizeof(PNG_SMS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(-16, -240, 0 + (240 - (strlen("GBC EMULATOR")*16))/2, 104, "GBC EMULATOR", 0xffff);
      animateTEXT(-16, 0 + (240 - (strlen("SMS EMULATOR")*16))/2, 240, 104, "SMS EMULATOR", 0xffff);
#endif        
    }
    if (MENU_ITEM == 4) {
#if PNG_DECODER
      animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_SMS_ICON, sizeof(PNG_SMS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_MP3_ICON, sizeof(PNG_MP3_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(-16, -240, 0 + (240  - (strlen("SMS EMULATOR")*16))/2, 104, "SMS EMULATOR", 0xffff);
      animateTEXT(-16, +0 + (240 - (strlen("MP3 PLAYER")*16))/2, 240, 104, "MP3 PLAYER", 0xffff);
#endif        
    }
    if (MENU_ITEM == 5) {
#if PNG_DECODER
      animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_MP3_ICON, sizeof(PNG_MP3_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_BTSPEAKER_ICON, sizeof(PNG_BTSPEAKER_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(-16, -240, 0 + (240 - (strlen("MP3 PLAYER")*16))/2, 104, "MP3 PLAYER", 0xffff);
      ///animateTEXT(-16, +0 + (240 - (strlen("BT SPEAKER")*16))/2, 240, 104, "BT SPEAKER", 0xffff);
      MENU_ITEM++;
#endif        
    }
    if (MENU_ITEM == 6) {
#if PNG_DECODER
      animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_BTSPEAKER_ICON, sizeof(PNG_BTSPEAKER_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_X86EMU_ICON, sizeof(PNG_X86EMU_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      ///animateTEXT(-16, -240, 0 + (240 - (strlen("BT SPEAKER")*16))/2, 104, "BT SPEAKER", 0xffff);
      ///animateTEXT(-16, +0 + (240 - (strlen("FAKE86 EMULATOR")*16))/2, 240, 104, "FAKE86 EMULATOR", 0xffff);
      MENU_ITEM++;
#endif        
    }
    if (MENU_ITEM == 7) {
#if PNG_DECODER
      animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_X86EMU_ICON, sizeof(PNG_X86EMU_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_SETTINGS_ICON, sizeof(PNG_SETTINGS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else       
      ///animateTEXT(-16, -240, 0 +(240 - (strlen("FAKE86 EMULATOR")*16))/2, 104, "FAKE86 EMULATOR", 0xffff);
      animateTEXT(-16, 0 + (240 - (strlen("SETTINGS")*16))/2, 240, 104, "SETTINGS", 0xffff);
#endif        
    }
    if (Extra_apps==2) {
      if (MENU_ITEM == 8) {
#if PNG_DECODER
        animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_SETTINGS_ICON, sizeof(PNG_SETTINGS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
        animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_KMSW_ICON, sizeof(PNG_KMSW_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        animateTEXT(-16, -240, 0 + (240 - (strlen("SETTINGS")*16))/2, 104, "SETTINGS", 0xffff);
        ///animateTEXT(-16, 0 + (240 - (strlen("KM SWITCH")*16))/2, 240, 104, "KM SWITCH", 0xffff);
        MENU_ITEM=0;
#endif        
      }
      if (MENU_ITEM == 9) {
#if PNG_DECODER
        animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_KMSW_ICON, sizeof(PNG_KMSW_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
        animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_OSC_ICON, sizeof(PNG_OSC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        ///animateTEXT(-16, -240, 0 + (240 - (strlen("KM SWITCH")*16))/2, 104, "KM SWITCH", 0xffff);
        ///animateTEXT(-16, 0 + (240 - (strlen("OSCILLOSCOPE")*16))/2, 240, 104, "OSCILLOSCOPE", 0xffff);
#endif        
      }
      if (MENU_ITEM == 10) {
#if PNG_DECODER
        animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_OSC_ICON, sizeof(PNG_OSC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
        animatePNG(-16, +40, 240 - 88 + (240 - 176) / 2, (240 - 160) / 2, PNG_SIGGEN_ICON, sizeof(PNG_SIGGEN_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        ///animateTEXT(-16, -240, 0 + (240 - (strlen("OSCILLOSCOPE")*16))/2, 104, "OSCILLOSCOPE", 0xffff);
        ///animateTEXT(-16, +0 + (240 - (strlen("SIG. GENERATOR")*16))/2, 240, 104, "SIG. GENERATOR", 0xffff);
#endif        
      }
      if (MENU_ITEM == 0) {
#if PNG_DECODER
        animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_SIGGEN_ICON, sizeof(PNG_SIGGEN_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        ///animateTEXT(-16, -240 , (0 + (strlen("SIG. GENERATOR")*16))/2, 104, "SIG. GENERATOR", 0xffff);
#endif        
      }
    } else {       
      if (MENU_ITEM == 0) {
#if PNG_DECODER
        animatePNG(-16, -240 + 80, (240 - 176) / 2, (240 - 160) / 2, PNG_SETTINGS_ICON, sizeof(PNG_SETTINGS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        animateTEXT(-16, -240 + 0, (0 + (strlen("SETTINGS")*16))/2, 104, "SETTINGS", 0xffff);
#endif        
      }
    }
//................................................................................
    TIME_COUNTER_BREAK=1;
  }
//--------------------------------------------------------------------------------
  if (INPUT_LEFT == 1)  {
    if (MENU_ITEM==0) DO_DEINIT_CLOCK(); 
    if (INDICATOR_MODE==1) HIDE_VOLUME_INDICATOR();
    else if (INDICATOR_MODE==2) HIDE_BACKLIGHT_INDICATOR();
//................................................................................
    MENU_ITEM--;
    if (MENU_ITEM < 0) MENU_ITEM = MAX_MENU_ITEMS;
    if (Extra_apps==2) {
      if (MENU_ITEM == 10) {
#if PNG_DECODER
        animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_SIGGEN_ICON, sizeof(PNG_SIGGEN_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        ///animateTEXT(+16, (240 - (strlen("SIG. GENERATOR")*16))/2, -240, 200, "SIG. GENERATOR", 0xffff);
        MENU_ITEM=7; ///
#endif        
      }     
      if (MENU_ITEM == 9) {
#if PNG_DECODER
        animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_SIGGEN_ICON, sizeof(PNG_SIGGEN_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
        animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_OSC_ICON, sizeof(PNG_OSC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        ///animateTEXT(+16, 240 + (0 + (strlen("SIG. GENERATOR")*16))/2, (0 + (strlen("SIG. GENERATOR")*16))/2, 104, "SIG. GENERATOR", 0xffff);
        ///animateTEXT(+16, (240 - (strlen("OSCILLOSCOPE")*16))/2, -240, 104, "OSCILLOSCOPE", 0xffff);
#endif        
      }
      if (MENU_ITEM == 8) {
#if PNG_DECODER
        animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_OSC_ICON, sizeof(PNG_OSC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
        animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_KMSW_ICON, sizeof(PNG_KMSW_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        ///animateTEXT(+16, 240 + (240 - (strlen("OSCILLOSCOPE")*16))/2, (240 - (strlen("OSCILLOSCOPE")*16))/2, 104, "OSCILLOSCOPE", 0xffff);
        ///animateTEXT(+16, (240 - (strlen("KM SWITCH")*16))/2, -240, 104, "KM SWITCH", 0xffff);
#endif        
      }
      if (MENU_ITEM == 7) {
#if PNG_DECODER
        animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_KMSW_ICON, sizeof(PNG_KMSW_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
        animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_SETTINGS_ICON, sizeof(PNG_SETTINGS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        ///animateTEXT(+16, 240 + (240 - (strlen("KM SWITCH")*16))/2, (240 - (strlen("KM SWITCH")*16))/2, 104, "KM SWITCH", 0xffff);
        ///animateTEXT(+16, (240 - (strlen("SETTINGS")*16))/2, -240, 104, "SETTINGS", 0xffff);
#endif        
      }
    } else {
      if (MENU_ITEM == 7) {
#if PNG_DECODER
        animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_SETTINGS_ICON, sizeof(PNG_SETTINGS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
        animateTEXT(+16, (240 - (strlen("SETTINGS")*16))/2, -240, 104, "SETTINGS", 0xffff);
#endif        
      }
    }      
    if (MENU_ITEM == 6) {
#if PNG_DECODER
      animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_SETTINGS_ICON, sizeof(PNG_SETTINGS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_X86EMU_ICON, sizeof(PNG_X86EMU_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(+16, 240 + (240 - (strlen("SETTINGS")*16))/2, (240 - (strlen("SETTINGS")*16))/2, 104, "SETTINGS", 0xffff);
      ///animateTEXT(+16, (240 - (strlen("FAKE86 EMULATOR")*16))/2, -240, 104, "FAKE86 EMULATOR", 0xffff);
      MENU_ITEM--;
#endif        
    }
    if (MENU_ITEM == 5) {
#if PNG_DECODER
      animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_X86EMU_ICON, sizeof(PNG_X86EMU_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_BTSPEAKER_ICON, sizeof(PNG_BTSPEAKER_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      ///animateTEXT(+16, 240 + (240 - (strlen("FAKE86 EMULATOR")*16))/2, (240 - (strlen("FAKE86 EMULATOR")*16))/2, 104, "FAKE86 EMULATOR", 0xffff);
      ///animateTEXT(+16, (240 - (strlen("BT SPEAKER")*16))/2, -240, 104, "BT SPEAKER", 0xffff);
      MENU_ITEM--;
#endif        
    }
    if (MENU_ITEM == 4) {
#if PNG_DECODER
      animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_BTSPEAKER_ICON, sizeof(PNG_BTSPEAKER_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_MP3_ICON, sizeof(PNG_MP3_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      ///animateTEXT(+16, 240 + (240 - (strlen("BT SPEAKER")*16))/2, (240 - (strlen("BT SPEAKER")*16))/2, 104, "BT SPEAKER", 0xffff);
      animateTEXT(+16, (240 - (strlen("MP3 PLAYER")*16))/2, -240, 104, "MP3 PLAYER", 0xffff);
#endif        
    }
    if (MENU_ITEM == 3) {
#if PNG_DECODER
      animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_MP3_ICON, sizeof(PNG_MP3_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_SMS_ICON, sizeof(PNG_SMS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(+16, 240 + (240 - (strlen("MP3 PLAYER")*16))/2, (240 - (strlen("MP3 PLAYER")*16))/2, 104, "MP3 PLAYER", 0xffff);
      animateTEXT(+16, (240 - (strlen("SMS EMULATOR")*16))/2, -240, 104, "SMS EMULATOR", 0xffff);
#endif        
    }
    if (MENU_ITEM == 2) {
#if PNG_DECODER
      animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_SMS_ICON, sizeof(PNG_SMS_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_GBC_ICON, sizeof(PNG_GBC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(+16, 240 + (240 - (strlen("SMS EMULATOR")*16))/2, (240 - (strlen("SMS EMULATOR")*16))/2, 104, "SMS EMULATOR", 0xffff);
      animateTEXT(+16, (240 - (strlen("GBC EMULATOR")*16))/2, -240, 104, "GBC EMULATOR", 0xffff);
#endif        
    }
    if (MENU_ITEM == 1) {
#if PNG_DECODER
      animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_GBC_ICON, sizeof(PNG_GBC_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
      animatePNG(+16, +40 , ((240 - 176) / 2) - 240, (240 - 160) / 2, PNG_NES_ICON, sizeof(PNG_NES_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(+16, 240 + (240 - (strlen("GBC EMULATOR")*16))/2, (240 - (strlen("GBC EMULATOR")*16))/2, 104, "GBC EMULATOR", 0xffff);
      animateTEXT(+16, (240 - (strlen("NES EMULATOR")*16))/2, -240, 104, "NES EMULATOR", 0xffff);        
#endif        
    }
    if (MENU_ITEM == 0) {
#ifndef NCAT_NO_PSRAM
      animatePNG(+16, 240 + 0, (240 - 176) / 2 +16, (240 - 160) / 2, PNG_NES_ICON, sizeof(PNG_NES_ICON), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      animateTEXT(+16, 240 + (240 - (strlen("NES EMULATOR")*16))/2, (240 - (strlen("NES EMULATOR")*16))/2, 104, "NES EMULATOR", 0xffff);
#endif        
    }
    TIME_COUNTER_BREAK=1;
  }
//--------------------------------------------------------------------------------
  if (INPUT_UP == 1)  {
    if (INDICATOR_MODE==1) {   
      if (VOLUME < 255 - 8) VOLUME += 8; else VOLUME = 255;
      UPDATE_VOLUME_INDICATOR(VOLUME);
      TIME_COUNTER_BREAK=1;
    } else if (INDICATOR_MODE==2) {
      if (LCD_BACKLIGHT < 255 - 8) LCD_BACKLIGHT += 8; else LCD_BACKLIGHT = 255;
      UPDATE_BACKLIGHT_INDICATOR(LCD_BACKLIGHT);
      TIME_COUNTER_BREAK=1;        
    }    
  }
//--------------------------------------------------------------------------------
  if (INPUT_DOWN == 1)  {
    if (INDICATOR_MODE==1) {
      if (VOLUME > 8) VOLUME -= 8; else VOLUME = 0;
      UPDATE_VOLUME_INDICATOR(VOLUME);
      TIME_COUNTER_BREAK=1;
    } else if (INDICATOR_MODE==2) {
      if (LCD_BACKLIGHT > 8) LCD_BACKLIGHT -= 8; else LCD_BACKLIGHT= 0;
      UPDATE_BACKLIGHT_INDICATOR(LCD_BACKLIGHT);
      TIME_COUNTER_BREAK=1;        
    }      
  }
//--------------------------------------------------------------------------------
  if (INPUT_SELECT == 1) {
    if (INDICATOR_MODE==1) INDICATOR_MODE=2;
    else if (INDICATOR_MODE==2) INDICATOR_MODE=1;

    printf("INDICATOR_MODE: %d\n",INDICATOR_MODE);
    while(INPUTREAD()) {
      INPUTCLEAR();
    }
    TIME_COUNTER_BREAK=1;
  }
//--------------------------------------------------------------------------------
  if (INPUT_START == 1 || INPUT_A == 1 || INPUT_B == 1) {
    while(INPUTREAD()) {
      INPUTCLEAR();
    }
    MENU_ACTIVE=2;
  }
  return 0;
}
//********************************************************************************



//********************************************************************************
// SETTINGS FUNCTIONS
//********************************************************************************

char*  settings_name[]={
  (char*)"RESET SETTINGS",
  (char*)"Clock Enabled",
  (char*)"Sound Enabled",
  (char*)"Force Bluetooth",
  (char*)"Composite Enabled",
  (char*)"Wallpaper",
  (char*)"MicroSD Required",
  (char*)"USB Host Required",
  (char*)"Clock Color",
  (char*)"InGame Menu Enabled",
  (char*)"Overclock - WARNING!",
  (char*)"LCD rotation - WARNING!",
  (char*)"Extra applications",
  (char*)"Bluetooth Security",
  (char*)"Multiplayer Devices",
  (char*)"USB joystick",
  (char*)"USB keyboard+mouse",
  (char*)"USB mass storage",
  (char*)"USB bluetooth",
  (char*)"LCD off time",
};
uint16_t settings_name_COUNT=sizeof(settings_name)/sizeof(const char*);

// default settings are overwrited by EEPROM
uint8_t settings_set[]={ //0 = EMPTY, 1=DISABLED, 2=ENABLED, 10..19 =NUMBER (10=0 .. 19=9)
  EEPROM_RESET_SETTINGS,  //0=not SETTINGS RESET, 1=request SETTINGS RESET
  2,
  2,
  0,
  2,
  10,
  0,
  0,
  10,
  2,
  10,
  10,
  1,
  10,
  2,
  1,
  2,
  2,
  1,
  14,
};
uint8_t settings_set_allowed[]={  // 0= can not set, 1=EMPTY or ENABLED, 2=EMPTY or DISABLED, 3=ENABLED or DISABLED, 4=DISABLED, EMPTY, ENABLED 1X=NUMBER 0..X
  1,
  3,
  3,  
  4,
  3,
  19,
  4,
  4,
  19,
  3,
  14,
  13,
  3,
  14,
  3,
  3,
  3,
  3,
  3,
  19,
};
//--------------------------------------------------------------------------------
void EEPROM_save_settings() {
  EEPROM.begin(EEPROM_SIZE);
  for (uint8_t tmp=0;tmp<sizeof(settings_set);tmp++) {
     EEPROM.write(tmp, settings_set[tmp]);    
  }  
  EEPROM.end();
}
//--------------------------------------------------------------------------------
void EEPROM_reset_settings() {
  EEPROM.end();
  settings_set[0]=0; 
  EEPROM_save_settings(); 
  ESP.restart(); //reboot
}
//--------------------------------------------------------------------------------
void EEPROM_load_settings() {
  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(0)!=0 || settings_set[0]==1) EEPROM_reset_settings();   
  for (uint8_t tmp=0;tmp<sizeof(settings_set);tmp++) {
     settings_set[tmp] = EEPROM.read(tmp); 
  }
  EEPROM.end();
}
//--------------------------------------------------------------------------------
uint8_t REQUIRE_RESTART() {
  screenmemory_fillscreen_static(0);  
#if JPG_DECODER  
  renderJPG_toBUFFER(NULL,(240 - 128) / 2, (240 - 128) / 2 - 32, JPG_WARNINGICON, sizeof(JPG_WARNINGICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
#endif 
  set_font_XY(16, 200 );
  draw_string_static((char*)"REQUIRED RESTART. A=RESTART.", 0xffff);
  INPUTCLEAR();
  delay___(1000);
  while (!INPUTREAD()) {}
  if (INPUT_A) ESP.restart(); 
  return 0;
}

uint8_t SETTINGS_POSITION=0;
uint8_t SETTINGS_POSITION_PAGE=0;
uint8_t SETTINGS_CHANGED=0;
//--------------------------------------------------------------------------------
void Draw_Setting_Menu() {
  RESTORE_BACKGROUND(0,36, 240, 204);

  SETTINGS_POSITION_PAGE=(SETTINGS_POSITION-1)/5;
  printf("SETTINGS_POSITION_PAGE: %d\n",SETTINGS_POSITION_PAGE);
  printf("SETTINGS_POSITION: %d\n",SETTINGS_POSITION);
  
  uint8_t Settings_Set=0;
  char settings_num='0';
  set_font_XY(64, 52 );
  if (SETTINGS_POSITION_PAGE*5+0<settings_name_COUNT) {
    Settings_Set=settings_set[SETTINGS_POSITION_PAGE*5+0];
    if (Settings_Set==0) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 48, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      draw_string_xy_transparent(24, 48, "[ ]", font_Ishmeria8x16, 0xffff);
#endif      
    } else if (Settings_Set==1) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 48, PNG_CHECKBOX_DISABLED, sizeof(PNG_CHECKBOX_DISABLED), PNGOUTPUT_BUFFER, 0, 0x0000);     
#else 
      draw_string_xy_transparent(24, 48, "[X]", font_Ishmeria8x16, 0b1111100000000000);
#endif      
    } else if (Settings_Set==2) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 48, PNG_CHECKBOX_ENABLED, sizeof(PNG_CHECKBOX_ENABLED), PNGOUTPUT_BUFFER, 0, 0x0000);      
#else 
      draw_string_xy_transparent(24, 48, "[V]", font_Ishmeria8x16, 0b0000011111000000);
#endif      
    } else if (Settings_Set>=10 && Settings_Set<20) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 48, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
      settings_num='0'+(Settings_Set-10);
      draw_char_xy_static_transparent(24+8, 48+4,settings_num,font_Ishmeria8x16, 0x0000); 
#else 
      settings_num='0'+(Settings_Set-10);
      char settings_num_text[]="[ ]";
      settings_num_text[1]=settings_num;
      draw_string_xy_transparent(24, 48, settings_num_text, font_Ishmeria8x16, 0xffff);
#endif      
    }
    if ((SETTINGS_POSITION-1)%5==0) {
      draw_string_static(settings_name[SETTINGS_POSITION_PAGE*5+0], 0x0000,0x07C0);
    } else {
      draw_string_xy_transparent(64+2, 52+2, settings_name[SETTINGS_POSITION_PAGE*5+0],font_Ishmeria8x16, 0x0000);    
      draw_string_xy_transparent(64, 52, settings_name[SETTINGS_POSITION_PAGE*5+0],font_Ishmeria8x16, 0xffff);    
    }
  }
  set_font_XY(64, 52+36 );
  if (SETTINGS_POSITION_PAGE*5+1<settings_name_COUNT) {
    Settings_Set=settings_set[SETTINGS_POSITION_PAGE*5+1];
    if (Settings_Set==0) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 84, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      draw_string_xy_transparent(24, 84, "[ ]", font_Ishmeria8x16, 0xffff);
#endif      
    } else if (Settings_Set==1) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 84, PNG_CHECKBOX_DISABLED, sizeof(PNG_CHECKBOX_DISABLED), PNGOUTPUT_BUFFER, 0, 0x0000);     
#else 
      draw_string_xy_transparent(24, 84, "[X]", font_Ishmeria8x16, 0b1111100000000000);
#endif      
    } else if (Settings_Set==2) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 84, PNG_CHECKBOX_ENABLED, sizeof(PNG_CHECKBOX_ENABLED), PNGOUTPUT_BUFFER, 0, 0x0000);      
#else 
      draw_string_xy_transparent(24, 84, "[V]", font_Ishmeria8x16, 0b0000011111000000);
#endif      
    } else if (Settings_Set>=10 && Settings_Set<20) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 84, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
      settings_num='0'+(Settings_Set-10);
      draw_char_xy_static_transparent(24+8, 84+4,settings_num,font_Ishmeria8x16, 0x0000); 
#else 
      settings_num='0'+(Settings_Set-10);
      char settings_num_text[]="[ ]";
      settings_num_text[1]=settings_num;
      draw_string_xy_transparent(24, 84, settings_num_text, font_Ishmeria8x16, 0xffff);
#endif      
    }
    if ((SETTINGS_POSITION-1)%5==1) {
      draw_string_static(settings_name[SETTINGS_POSITION_PAGE*5+1], 0x0000,0x07C0);
    } else {
      draw_string_xy_transparent(64+2, 52+36+2, settings_name[SETTINGS_POSITION_PAGE*5+1],font_Ishmeria8x16, 0x0000);    
      draw_string_xy_transparent(64, 52+36, settings_name[SETTINGS_POSITION_PAGE*5+1],font_Ishmeria8x16, 0xffff);    
    }
  }
  set_font_XY(64, 52+72 );
  if (SETTINGS_POSITION_PAGE*5+2<settings_name_COUNT) {
    Settings_Set=settings_set[SETTINGS_POSITION_PAGE*5+2];
    if (Settings_Set==0) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 120, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      draw_string_xy_transparent(24, 120, "[ ]", font_Ishmeria8x16, 0xffff);
#endif      
    } else if (Settings_Set==1) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 120, PNG_CHECKBOX_DISABLED, sizeof(PNG_CHECKBOX_DISABLED), PNGOUTPUT_BUFFER, 0, 0x0000);     
#else 
      draw_string_xy_transparent(24, 120, "[X]", font_Ishmeria8x16, 0b1111100000000000);
#endif      
    } else if (Settings_Set==2) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 120, PNG_CHECKBOX_ENABLED, sizeof(PNG_CHECKBOX_ENABLED), PNGOUTPUT_BUFFER, 0, 0x0000);      
#else 
      draw_string_xy_transparent(24, 120, "[V]", font_Ishmeria8x16, 0b0000011111000000);
#endif      
    } else if (Settings_Set>=10 && Settings_Set<20) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 120, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
      settings_num='0'+(Settings_Set-10);
      draw_char_xy_static_transparent(24+8, 120+4,settings_num,font_Ishmeria8x16, 0x0000); 
#else 
      settings_num='0'+(Settings_Set-10);
      char settings_num_text[]="[ ]";
      settings_num_text[1]=settings_num;
      draw_string_xy_transparent(24, 120, settings_num_text, font_Ishmeria8x16, 0xffff);
#endif      
    }
    if ((SETTINGS_POSITION-1)%5==2) {
      draw_string_static(settings_name[SETTINGS_POSITION_PAGE*5+2], 0x0000,0x07C0);
    } else {
      draw_string_xy_transparent(64+2, 52+72+2, settings_name[SETTINGS_POSITION_PAGE*5+2],font_Ishmeria8x16, 0x0000);    
      draw_string_xy_transparent(64, 52+72, settings_name[SETTINGS_POSITION_PAGE*5+2],font_Ishmeria8x16, 0xffff);    
    }
  }
  set_font_XY(64, 52+108 );
  if (SETTINGS_POSITION_PAGE*5+3<settings_name_COUNT) {
    Settings_Set=settings_set[SETTINGS_POSITION_PAGE*5+3];
    if (Settings_Set==0) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 156, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      draw_string_xy_transparent(24, 156, "[ ]", font_Ishmeria8x16, 0xffff);
#endif      
    } else if (Settings_Set==1) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 156, PNG_CHECKBOX_DISABLED, sizeof(PNG_CHECKBOX_DISABLED), PNGOUTPUT_BUFFER, 0, 0x0000);     
#else 
      draw_string_xy_transparent(24, 156, "[X]", font_Ishmeria8x16, 0b1111100000000000);
#endif      
    } else if (Settings_Set==2) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 156, PNG_CHECKBOX_ENABLED, sizeof(PNG_CHECKBOX_ENABLED), PNGOUTPUT_BUFFER, 0, 0x0000);      
#else 
      draw_string_xy_transparent(24, 156, "[V]", font_Ishmeria8x16, 0b0000011111000000);
#endif      
    } else if (Settings_Set>=10 && Settings_Set<20) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 156, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
      settings_num='0'+(Settings_Set-10);
      draw_char_xy_static_transparent(24+8, 156+4,settings_num,font_Ishmeria8x16, 0x0000); 
#else 
      settings_num='0'+(Settings_Set-10);
      char settings_num_text[]="[ ]";
      settings_num_text[1]=settings_num;
      draw_string_xy_transparent(24, 156, settings_num_text, font_Ishmeria8x16, 0xffff);
#endif      
    }
    if ((SETTINGS_POSITION-1)%5==3) {
      draw_string_static(settings_name[SETTINGS_POSITION_PAGE*5+3], 0x0000,0x07C0);
    } else {
      draw_string_xy_transparent(64+2, 52+108+2, settings_name[SETTINGS_POSITION_PAGE*5+3],font_Ishmeria8x16, 0x0000);    
      draw_string_xy_transparent(64, 52+108, settings_name[SETTINGS_POSITION_PAGE*5+3],font_Ishmeria8x16, 0xffff);    
    }
  }
  set_font_XY(64, 52+144 );
  if (SETTINGS_POSITION_PAGE*5+4<settings_name_COUNT) {
    Settings_Set=settings_set[SETTINGS_POSITION_PAGE*5+4];
    if (Settings_Set==0) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 192, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
#else 
      draw_string_xy_transparent(24, 192, "[ ]", font_Ishmeria8x16, 0xffff);
#endif      
    } else if (Settings_Set==1) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 192, PNG_CHECKBOX_DISABLED, sizeof(PNG_CHECKBOX_DISABLED), PNGOUTPUT_BUFFER, 0, 0x0000);     
#else 
      draw_string_xy_transparent(24, 192, "[X]", font_Ishmeria8x16, 0b1111100000000000);
#endif      
    } else if (Settings_Set==2) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 192, PNG_CHECKBOX_ENABLED, sizeof(PNG_CHECKBOX_ENABLED), PNGOUTPUT_BUFFER, 0, 0x0000);      
#else 
      draw_string_xy_transparent(24, 192, "[V]", font_Ishmeria8x16, 0b0000011111000000);
#endif      
    } else if (Settings_Set>=10 && Settings_Set<20) {
#ifndef NCAT_NO_PSRAM
      renderPNG(24, 192, PNG_CHECKBOX_EMPTY, sizeof(PNG_CHECKBOX_EMPTY), PNGOUTPUT_BUFFER, 0, 0x0000);
      settings_num='0'+(Settings_Set-10);
      draw_char_xy_static_transparent(24+8, 192+4,settings_num,font_Ishmeria8x16, 0x0000); 
#else 
      settings_num='0'+(Settings_Set-10);
      char settings_num_text[]="[ ]";
      settings_num_text[1]=settings_num;
      draw_string_xy_transparent(24, 192, settings_num_text, font_Ishmeria8x16, 0xffff);
#endif      
    }
    if ((SETTINGS_POSITION-1)%5==4) {
      draw_string_static(settings_name[SETTINGS_POSITION_PAGE*5+4], 0x0000,0x07C0);
    } else {
      draw_string_xy_transparent(64+2, 52+144+2, settings_name[SETTINGS_POSITION_PAGE*5+4],font_Ishmeria8x16, 0x0000);    
      draw_string_xy_transparent(64, 52+144, settings_name[SETTINGS_POSITION_PAGE*5+4],font_Ishmeria8x16, 0xffff);    
    }
  }
}
//--------------------------------------------------------------------------------
void Do_Settings_Menu() {
  if (SETTINGS_POSITION==0) {
    SETTINGS_POSITION=1;
    Draw_Setting_Menu();
  }
  
  INPUTCLEAR();
  if (INPUTREAD()) {  

    if (INPUT_UP ) {
      if (SETTINGS_POSITION>1) SETTINGS_POSITION--;
      else SETTINGS_POSITION=settings_name_COUNT;
    }
    
    if (INPUT_DOWN) {
      if (SETTINGS_POSITION<settings_name_COUNT) SETTINGS_POSITION++;
      else SETTINGS_POSITION=1;
    }

    if (INPUT_A) {
      SETTINGS_CHANGED=1;
      
      if (settings_set_allowed[SETTINGS_POSITION-1]==0) {} // 0=Can not be set
      else if (settings_set_allowed[SETTINGS_POSITION-1]==1) { // 1=EMPTY or ENABLED
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=2;
        else if (settings_set[SETTINGS_POSITION-1]==2) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==2) { // 2=EMPTY or DISABLED
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=1;
        else if (settings_set[SETTINGS_POSITION-1]==1) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==3) { // 3=ENABLED or DISABLED
        if (settings_set[SETTINGS_POSITION-1]==2) settings_set[SETTINGS_POSITION-1]=1;
        else if (settings_set[SETTINGS_POSITION-1]==1) settings_set[SETTINGS_POSITION-1]=2;
        else settings_set[SETTINGS_POSITION-1]=1;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==4) { // 4=DISABLED, EMPTY, ENABLED
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=2;
        else if (settings_set[SETTINGS_POSITION-1]==2) settings_set[SETTINGS_POSITION-1]=1;
        else if (settings_set[SETTINGS_POSITION-1]==1) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==10) { // 5=NUMBER 0..1
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=10;
        else if (settings_set[SETTINGS_POSITION-1]==10) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==11) { // 5=NUMBER 0..1
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=10;
        else if (settings_set[SETTINGS_POSITION-1]==10) settings_set[SETTINGS_POSITION-1]=11;
        else if (settings_set[SETTINGS_POSITION-1]==11) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==12) { // 5=NUMBER 0..2
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=10;
        else if (settings_set[SETTINGS_POSITION-1]==10) settings_set[SETTINGS_POSITION-1]=11;
        else if (settings_set[SETTINGS_POSITION-1]==11) settings_set[SETTINGS_POSITION-1]=12;
        else if (settings_set[SETTINGS_POSITION-1]==12) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==13) { // 5=NUMBER 0..3
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=10;
        else if (settings_set[SETTINGS_POSITION-1]==10) settings_set[SETTINGS_POSITION-1]=11;
        else if (settings_set[SETTINGS_POSITION-1]==11) settings_set[SETTINGS_POSITION-1]=12;
        else if (settings_set[SETTINGS_POSITION-1]==12) settings_set[SETTINGS_POSITION-1]=13;
        else if (settings_set[SETTINGS_POSITION-1]==13) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==14) { // 5=NUMBER 0..4
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=10;
        else if (settings_set[SETTINGS_POSITION-1]==10) settings_set[SETTINGS_POSITION-1]=11;
        else if (settings_set[SETTINGS_POSITION-1]==11) settings_set[SETTINGS_POSITION-1]=12;
        else if (settings_set[SETTINGS_POSITION-1]==12) settings_set[SETTINGS_POSITION-1]=13;
        else if (settings_set[SETTINGS_POSITION-1]==13) settings_set[SETTINGS_POSITION-1]=14;
        else if (settings_set[SETTINGS_POSITION-1]==14) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==15) { // 5=NUMBER 0..5
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=10;
        else if (settings_set[SETTINGS_POSITION-1]==10) settings_set[SETTINGS_POSITION-1]=11;
        else if (settings_set[SETTINGS_POSITION-1]==11) settings_set[SETTINGS_POSITION-1]=12;
        else if (settings_set[SETTINGS_POSITION-1]==12) settings_set[SETTINGS_POSITION-1]=13;
        else if (settings_set[SETTINGS_POSITION-1]==13) settings_set[SETTINGS_POSITION-1]=14;
        else if (settings_set[SETTINGS_POSITION-1]==14) settings_set[SETTINGS_POSITION-1]=15;
        else if (settings_set[SETTINGS_POSITION-1]==15) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      else if (settings_set_allowed[SETTINGS_POSITION-1]==19) { // 5=NUMBER 0..9
        if (settings_set[SETTINGS_POSITION-1]==0) settings_set[SETTINGS_POSITION-1]=10;
        else if (settings_set[SETTINGS_POSITION-1]==10) settings_set[SETTINGS_POSITION-1]=11;
        else if (settings_set[SETTINGS_POSITION-1]==11) settings_set[SETTINGS_POSITION-1]=12;
        else if (settings_set[SETTINGS_POSITION-1]==12) settings_set[SETTINGS_POSITION-1]=13;
        else if (settings_set[SETTINGS_POSITION-1]==13) settings_set[SETTINGS_POSITION-1]=14;
        else if (settings_set[SETTINGS_POSITION-1]==14) settings_set[SETTINGS_POSITION-1]=15;
        else if (settings_set[SETTINGS_POSITION-1]==15) settings_set[SETTINGS_POSITION-1]=16;
        else if (settings_set[SETTINGS_POSITION-1]==16) settings_set[SETTINGS_POSITION-1]=17;
        else if (settings_set[SETTINGS_POSITION-1]==17) settings_set[SETTINGS_POSITION-1]=18;
        else if (settings_set[SETTINGS_POSITION-1]==18) settings_set[SETTINGS_POSITION-1]=19;
        else if (settings_set[SETTINGS_POSITION-1]==19) settings_set[SETTINGS_POSITION-1]=0;
        else settings_set[SETTINGS_POSITION-1]=0;
      }
      EEPROM_save_settings();
    }
    Draw_Setting_Menu();
    vTaskDelay___(250);  //250ms
    if ((INPUT_START && INPUT_SELECT) || (INPUT_SYSTEM)) POWER=0; 
  }  
}            

//================================================================================
#define WINMENU_CURSOR "->"
#define WINMENU_TEXT0 "RESUME"
#define WINMENU_TEXT1 "STATE SAVE"
#define WINMENU_TEXT2 "STATE LOAD"
#define WINMENU_TEXT3 "EXIT"


#define WINMENU_ITEMSCOUNT 4
uint8_t WINMENU_POSITION = 1;

char* STATESAVE_FILENAME;

int NES_state_save(char* filename);
int NES_state_load(char* filename);
int8_t GBC_savestate(char * filename);
int8_t GBC_loadstate(char * filename);
void SMS_load_state(char* filename);
void SMS_save_state(char* filename);

uint8_t WM_fontcolor=0xff;
uint8_t WM_fontbgcolor=0x00;

//********************************************************************************
// WINDOW MENU FUNCTIONS
//********************************************************************************
void do_window_menu() {
  DRAWREADY = 1;
  WINMENU_POSITION = 1;
#ifndef NCAT_NO_PSRAM
  STATESAVE_FILENAME=(char*)ps_malloc(256); ///allocate memory 
#else
  STATESAVE_FILENAME=(char*)malloc(256); ///allocate memory 
#endif  
  memset(STATESAVE_FILENAME, '\0', sizeof(STATESAVE_FILENAME));
  uint16_t SCREEN_MENU_XOFFSET=240;
#if VGA_OUTPUT
  SCREEN_MENU_XOFFSET=SCREEN_WIDTH;
  if (DRAWMODE == 3) SCREEN_MENU_XOFFSET=240; 
#endif
  if (DRAWMODE == 1 || DRAWMODE == 4) {
    WM_fontcolor=0xff;
    WM_fontbgcolor=0b10100100;
    screenmemory_drawfillrectangle((SCREEN_MENU_XOFFSET - 128) / 2, (240 - 112) / 2 , 128, 112, 0b10100100 ); //default PAL //GGRR0BB0
    screenmemory_drawrectangle((SCREEN_MENU_XOFFSET - 128) / 2, (240 - 112) / 2 , 128, 112, 0b11101101 ); //GGRR0BB0
  } else if (DRAWMODE == 2) {
    WM_fontcolor=0x20;
    WM_fontbgcolor=0x00;
    screenmemory_drawfillrectangle((SCREEN_MENU_XOFFSET - 120) / 2, (240 - 112) / 2 , 128, 112, 0x00 ); //NES PAL   
    screenmemory_drawrectangle((SCREEN_MENU_XOFFSET - 120) / 2, (240 - 112) / 2 , 128, 112, 0x10 );
  } else if (DRAWMODE == 3) {
    WM_fontcolor=0x1f;
    WM_fontbgcolor=0x1e;
    screenmemory_drawfillrectangle((SCREEN_MENU_XOFFSET - 128) / 2, (240 - 112) / 2 , 128, 112, WM_fontbgcolor ); //SMS PAL
    screenmemory_drawrectangle((SCREEN_MENU_XOFFSET - 128) / 2, (240 - 112) / 2 , 128, 112, WM_fontbgcolor );
  }
  draw_string_xy((SCREEN_MENU_XOFFSET - 120) / 2 + 8, (240 - 112) / 2 + 16 + (WINMENU_POSITION - 1) * 16, WINMENU_CURSOR , DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  draw_string_xy((SCREEN_MENU_XOFFSET - 120) / 2 + 32, (240 - 112) / 2 + 16 + 0, WINMENU_TEXT0 , DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  draw_string_xy((SCREEN_MENU_XOFFSET - 120) / 2 + 32, (240 - 112) / 2 + 24 + 16, WINMENU_TEXT1 , DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  draw_string_xy((SCREEN_MENU_XOFFSET - 120) / 2 + 32, (240 - 112) / 2 + 32 + 32, WINMENU_TEXT2 , DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  draw_string_xy((SCREEN_MENU_XOFFSET - 120) / 2 + 32, (240 - 112) / 2 + 40 + 48, WINMENU_TEXT3 , DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
  DRAWREADY = 1;
  while (WINMENU_POSITION != 255) {
    INPUTCLEAR();
    if (INPUTREAD()) {
      INPUTREPORT();
      if (INPUT_UP == 1) {
        if (WINMENU_POSITION > 1) WINMENU_POSITION--;
        else WINMENU_POSITION = WINMENU_ITEMSCOUNT;
      }
      if (INPUT_DOWN == 1) {
        if (WINMENU_POSITION < WINMENU_ITEMSCOUNT) WINMENU_POSITION++;
        else WINMENU_POSITION = 1;
      }
      for (uint8_t tmp = 1; tmp < WINMENU_ITEMSCOUNT + 1; tmp++) {
        if (tmp == WINMENU_POSITION)
          draw_string_xy((SCREEN_MENU_XOFFSET - 120) / 2 + 8, (240 - 112) / 2 + 16 + (tmp - 1) * (16 + 8), WINMENU_CURSOR , DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
        else
          draw_string_xy((SCREEN_MENU_XOFFSET - 120) / 2 + 8, (240 - 112) / 2 + 16 + (tmp - 1) * (16 + 8), "  " , DisplayFontSet, WM_fontcolor, WM_fontbgcolor);
      }
      if (INPUT_A || INPUT_B) {
        INPUTCLEAR();
        if (WINMENU_POSITION == 1) {
          //RESUME to GAME
          WINMENU_POSITION = 255;
        } else if (WINMENU_POSITION == 2) {
#if MICROSD
          //STATE SAVE
          if (POWER == APP_EMU_NES) {
            strcpy(STATESAVE_FILENAME,"/NES/");
            strcpy(&STATESAVE_FILENAME[5],TRACKNAME);         
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-4]='.';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-3]='S';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-2]='A';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-1]='V';
            printf("SAVE STATE FILENAME: %s\n", STATESAVE_FILENAME);
#if NES_EMULATOR
            NES_state_save(STATESAVE_FILENAME);
#endif
          } else if (POWER == APP_EMU_GBC) {           
            strcpy(STATESAVE_FILENAME,"/GBC/");
            strcpy(&STATESAVE_FILENAME[5],TRACKNAME);                                  
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-4]='.';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-3]='S';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-2]='A';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-1]='V';
            printf("SAVE STATE FILENAME: %s\n", STATESAVE_FILENAME);
#if GBC_EMULATOR
            GBC_savestate(STATESAVE_FILENAME);
#endif
          } else if (POWER == APP_EMU_SMS) {
            strcpy(STATESAVE_FILENAME,"/SMS/");
            strcpy(&STATESAVE_FILENAME[5],TRACKNAME);         
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-4]='.';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-3]='S';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-2]='A';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-1]='V';
            printf("SAVE STATE FILENAME: %s\n", STATESAVE_FILENAME);
#if SMS_EMULATOR
            SMS_save_state(STATESAVE_FILENAME);
#endif            
          }
#endif 
          WINMENU_POSITION = 255;       
        } else if (WINMENU_POSITION == 3) {
          //LOAD STATE
#if MICROSD
          if (POWER == APP_EMU_NES) {
            strcpy(STATESAVE_FILENAME,"/NES/");
            strcpy(&STATESAVE_FILENAME[5],TRACKNAME);         
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-4]='.';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-3]='S';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-2]='A';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-1]='V';
            printf("LOAD STATE FILENAME: %s\n", STATESAVE_FILENAME);
#if NES_EMULATOR
            NES_state_load(STATESAVE_FILENAME);
#endif
          } else if (POWER == APP_EMU_GBC) {
            strcpy(STATESAVE_FILENAME,"/GBC/");
            strcpy(&STATESAVE_FILENAME[5],TRACKNAME);         
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-4]='.';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-3]='S';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-2]='A';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-1]='V';
            printf("LOAD STATE FILENAME: %s\n", STATESAVE_FILENAME);
#if GBC_EMULATOR
            GBC_loadstate(STATESAVE_FILENAME);
#endif
          } else if (POWER == APP_EMU_SMS) {
            strcpy(STATESAVE_FILENAME,"/SMS/");
            strcpy(&STATESAVE_FILENAME[5],TRACKNAME);         
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-4]='.';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-3]='S';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-2]='A';
            STATESAVE_FILENAME[strlen(STATESAVE_FILENAME)-1]='V';
            printf("LOAD STATE FILENAME: %s\n", STATESAVE_FILENAME);
#if SMS_EMULATOR
            SMS_load_state(STATESAVE_FILENAME);
#endif            
          }  
#endif
             WINMENU_POSITION = 255;
        } else if (WINMENU_POSITION == 4) {
          //EXIT GAME
          POWER = 0;
          WINMENU_POSITION = 255;
        }
      }
    }
    DRAWREADY = 1;
    xSemaphoreGive(draw_mutex); //refresh SOUND
    vPortYield(); ///
    vTaskDelay___(120); //200ms
  }
  free(STATESAVE_FILENAME);
}


//--------------------------------------------------------------------------------
uint32_t getfilesize_SD(char* FILENAME) {
#if MICROSD
  fp = SD.open(FILENAME);
#ifdef DEBUG
  Serial.print("FILE SIZE: ");
  Serial.println(fp.size());
#endif
  uint32_t FILESIZE=fp.size();
  fp.close();
  return FILESIZE; 
#else 
  return 0;
#endif  
}
//--------------------------------------------------------------------------------
