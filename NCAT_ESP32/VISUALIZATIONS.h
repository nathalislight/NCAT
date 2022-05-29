//FFT_library
#include "src/arduinoFFT/src/arduinoFFT.h"                  // Standard Arduino FFT library

//Visualizations
arduinoFFT FFT = arduinoFFT();


//***************************************************************************************
//   AUDIO VISUALYZATION of PLAYER & BLUETOOTH SPEAKER
//***************************************************************************************
#define SAMPLES 128
uint16_t value;
byte peak[] = {0, 0, 0, 0, 0, 0, 0};
///double vReal[SAMPLES];
///double vImag[SAMPLES];
double* vReal;
double* vImag;

//***************************************************************************************
uint16_t COLORIX;
#define max_bar_height  140
#define bar_height  196
#define bar_width  24
#define bar_spacing  10
#define bars_xoffset  8


uint16_t amp[7];
//***************************************************************************************
void displayBand(int band, int dsize) {
  int dmax = max_bar_height; //max bar height
  if (dsize > dmax) dsize = dmax;
  for (int s = dsize - 1; s <= dmax; s = s + 1) {
    for (uint16_t bar_tmp = 0; bar_tmp < bar_width; bar_tmp++)
      screenmemory_drawpixel((bar_width + bar_spacing)*band + bar_tmp + 1 + bars_xoffset, bar_height - s, 0);
  }
  

  for (int s = dsize; s > 0 ; s = s - 1) {

    if (POWER == APP_AUD_PLAYER) {
      if (s > 120 ) COLORIX = 0b00111000; //RED
      else if (s > 100 && s <= 120) COLORIX = 0b11111000; //YELLOW
      else COLORIX = 0b11000000; //GREEN
    } else if (POWER == APP_AUD_BTSPKER) {
      if (s > 120 ) COLORIX = 0b00111000; //RED
      else if (s > 100 && s <= 120) COLORIX = 0b00111111; //PURPLE
      else COLORIX = 0b00000111; //BLUE

    }
    
    for (uint16_t bar_tmp = 0; bar_tmp < bar_width; bar_tmp++) {
      screenmemory_drawpixel((bar_width + bar_spacing)*band + bar_tmp + 1 + bars_xoffset, bar_height - s + 1, COLORIX);
    }


  
  }
  if (dsize > peak[band]) {
    peak[band] = dsize;
  }
}
//**************************************************************************************

///extern uint8_t BTSPKR_data[128]; ///NCX:
extern uint8_t* BTSPKR_data; ///128bytes

void  visualyze() {
  
  vPortYield(); ///
  if (visualyze_READY == 0) return;
  visualyze_READY = 0;

#ifndef NCAT_NO_PSRAM
  if (!vReal) vReal = (double*)ps_malloc(SAMPLES * sizeof(double));
  if (!vImag) vImag = (double*)ps_malloc(SAMPLES * sizeof(double));
#else
  if (!vReal) vReal = (double*)malloc(SAMPLES * sizeof(double));
  if (!vImag) vImag = (double*)malloc(SAMPLES * sizeof(double));
#endif

  for (int i = 0; i < 0 + SAMPLES; i++) {

    if (POWER == APP_AUD_PLAYER) {
#if MICROSD
      if (audio.m_curSample < 1152 - SAMPLES)
        value = ((int16_t*)audio.m_outBuff)[2 * i + audio.m_curSample * 2];
      else
        value = ((int16_t*)audio.m_outBuff)[2 * i + (1152 - audio.m_curSample) * 2  ];
      vReal[i] = value / 512 * audio.m_vol / 100;
#endif      

    } else if (POWER == APP_AUD_BTSPKER) {
#if BLUETOOTH
      value = ((uint16_t*)btstack_audio_buffer)[i]; ///in "btstack_audio_esp32.c"
#endif      
      if (value > 250) value = 0;
      if (value < 6) value = 0;

      vReal[i] = value / 16;
    }
    vImag[i] = 0;
  }

  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  for (int i = 0; i < 7; i++) {
    amp[i] = 0;  //reset amp bar values
  }

  for (int i = 1; i < (SAMPLES / 2); i++) { // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    if (i > 2 && i <= 3 & (uint16_t)vReal[i] > amp[0] )  amp[0] = (uint16_t)vReal[i]; //125Hz
    if (i > 3 && i <= 7 & (uint16_t)vReal[i] > amp[1] )  amp[1] = (uint16_t)vReal[i]; //250Hz
    if (i > 7 && i <= 12 & (uint16_t)vReal[i] > amp[2] )  amp[2] = (uint16_t)vReal[i]; //500Hz
    if (i > 12 && i <= 24 & (uint16_t)vReal[i] > amp[3] )  amp[3] = (uint16_t)vReal[i]; //1kHz
    if (i > 24 && i <= 30 & (uint16_t)vReal[i] > amp[4] )  amp[4] = (uint16_t)vReal[i]; //2kHz
    if (i > 30 && i <= 45 & (uint16_t)vReal[i] > amp[5] )  amp[5] = (uint16_t)vReal[i]; //4kHz
    if (i > 45 && i <= 64 & (uint16_t)vReal[i] > amp[6] )  amp[6] = (uint16_t)vReal[i]; //8kHz

  }

  if (POWER == APP_AUD_PLAYER)  {
    //correct bar ratio
    amp[6] *= 2;
    amp[5] *= 2;
    amp[4] *= 1.5;
    amp[3] *= 1.0;
    amp[2] *= 0.50;
    amp[1] *= 0.25;
    amp[0] *= 0.25;
  } else if (POWER == APP_AUD_BTSPKER)  {
    //correct bar ratio
    amp[6] *= 0.50;
    amp[5] *= 0.75;
    amp[4] *= 1.0;
    amp[3] *= 1.0;
    amp[2] *= 1.0;
    amp[1] *= 1.0;
    amp[0] *= 1.0;
  }
  ///DRAW BARS
  for (uint16_t tmp = 0; tmp < 7; tmp++) displayBand(tmp, amp[tmp]);

  //DRAW PEAKS
  for (byte band = 0; band <= 6; band++) {
    for (uint16_t bar_tmp = 0; bar_tmp < bar_width; bar_tmp++)
      screenmemory_drawpixel((bar_width + bar_spacing)*band + bar_tmp + 1 + bars_xoffset, bar_height - peak[band], 255);
  }

  for (byte band = 0; band <= 6; band++) {
    if (peak[band] > 0) peak[band] -= 1; // Decay the peak
  }


  if (POWER == APP_AUD_PLAYER)
    if (DRAWREADY==1) {
      xSemaphoreGive(draw_mutex); //refresh LCD
    }

  visualyze_READY = 1;
}
//********************************************************************************
uint16_t PLAYINGFILE = 0;
uint16_t TOTALFILES = 0;

//________________________________________________________________________________

//NEXT TRACK in Player...

char* PREVNEXT(char* PATH, uint8_t POSITION) {
#if MICROSD  
  SDCARD_BUSY=1;
  
  if (PATH[strlen(PATH) - 1] != '/')
    if (strlen(PATH) > 1) {
      PATH[strlen(PATH) - 1] = '\0';
      for (uint8_t strpos = strlen(PATH) - 1; strpos > 0; strpos--) {
        if (PATH[strpos] == '/') break;
        PATH[strpos] = '\0';
      }
    }

//................................................................................
  if (!filename[0]) {
  printf(">>> alloc filenames\n");
  if (PSRAM_BASE_SIZE > 0)
    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char*)ps_malloc(MAXFILENAME_LENGTH + 1); //temporary ps_malloc filenamebuffer
  else
    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) filename[tmp] = (char*)malloc(MAXFILENAME_LENGTH + 1); //temporary malloc filenamebuffer
  }  
//................................................................................

  uint8_t num = 0;
  //LOAD FILENAMES INTO MEMORY...

  num = 0;
  if (!dirFile.open(PATH, O_READ)) {
    while (1) {};
  }
  while (num < MAXFILES && file.openNext(&dirFile, O_READ)) {

    // Skip directories and hidden files.
    if (!file.isSubDir() && !file.isHidden()) {

      for (uint8_t i = sizeof(filename[num]); i > 3; i--) filename[num][i] = 0;

      file.getName(filename[num], MAXFILENAME_LENGTH);

      if (file.isSubDir()) {
        ///sprintf(filename[num], "%s/", filename[num]);
        ///num++;
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
      }

      //check MP3 File extension, then increase index
      if ((fileext[0] == 'M' || fileext[0] == 'm')
          && (fileext[1] == 'P' || fileext[1] == 'p')
          && (fileext[2] == '3' || fileext[2] == '3')) {
        num++;
      }
      if ((fileext[0] == 'W' || fileext[0] == 'w')
          && (fileext[1] == 'A' || fileext[1] == 'a')
          && (fileext[2] == 'V' || fileext[2] == 'v')) {
        num++;
      }
    }
    loadedFileNames = num;
    file.close();
  }
  dirFile.close();
  if (DEBUG) {
    Serial.println("--------------------------------------");
    Serial.print("Count of loaded File Names:");
    Serial.println(loadedFileNames);
  }

  sortStrings(filename, loadedFileNames);

  sprintf(MAINPATH, "%s%s", PATH, filename[POSITION]);
  if (DEBUG) Serial.println(MAINPATH);

  sprintf(TRACKNAME, "%s", filename[POSITION]);
//................................................................................
  if (filename[0]) {
    printf(">>> free filenames\n");

    for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) {
      free(filename[tmp]); //free temporary malloc filenamebuffer
      filename[tmp]=NULL;
    }    
  }  
  SDCARD_BUSY=0;
//................................................................................
#endif
  return MAINPATH ; //START //A
}
//________________________________________________________________________________
