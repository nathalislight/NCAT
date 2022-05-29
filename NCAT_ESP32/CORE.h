uint32_t ON_SECONDS=0;               //TOTAL ON SCEONDS
uint32_t LAST_ACTIVITY_SECONDS=0;    //LAST ACTIVITY SECONDS TIME
uint8_t  BACKLIGHT_PIN_LAST_VALUE=0; //0..255      
uint16_t BACKLIGHT_OFF_TIME=0;       // seconds

#define BATERY_READ_INTERVAL 5000 //5000ms


//██████████████████████████████████████████████████
//█  MAIN CORE TASK
//██████████████████████████████████████████████████

uint32_t eventCore1_counter = 0;
uint32_t BLUETOOTH_UPDATE_Counter = 0;

uint32_t FRAME_COUNTER=0;

void mainCore0(void * arg) {
  //--------------------------------------------------------------------------------
#if(COMPOSITE_VIDEO_ISR_CORE == 0 && COMPOSITE_VIDEO == 1) //Composite on CORE 0 not recommending...
  if (Composite_Enabled == 2) { //0==NOT SET, 1==DISABLED, 2=ENABLED
    printf("Initializing COMPOSITE VIDEO...");
    COMPOSITE_video_init(COMPOSITE_VIDEO_BYTE_WIDTH, adj_32bit, 1); // start the A/V pump on app on CORE0 //experimental.
    printf("[ OK ]\n");
  }
#endif

  INIT_SOUND();

  //--------------------------------------------------------------------------------
  //starting Wire on Core 0 !!!
#if CLOCK
#ifdef NCAT_BOARD
  if (Clock_Enabled == 2) {
    printf("DS3231 init on WIRE...");
    Wire.begin(I2C_SDA, I2C_SCL);
    DS3231_init(DS3231_CONTROL_INTCN);
    printf("[ OK ]\n");
  }
#endif
#ifdef NCAT_LITE
  printf("DS1307 init on WIRE...");
  Wire.begin(I2C_SDA, I2C_SCL);    
  Wire.setClock(100000);
  if (!ds1307rtc.begin()) {
    printf("[fail]\n");
  } else {
    printf("[ OK ]\n");
  }  
  ds1307rtc.setSquareWave(SquareWaveDisable);
#endif
#endif
//--------------------------------------------------------------------------------

  //CALCULATE SET LCD BACKLIGHT OFF TIME
  if (LCD_off_time==11) BACKLIGHT_OFF_TIME=5; 
  else if (LCD_off_time==12) BACKLIGHT_OFF_TIME=10; 
  else if (LCD_off_time==13) BACKLIGHT_OFF_TIME=30; 
  else if (LCD_off_time==14) BACKLIGHT_OFF_TIME=60; 
  else if (LCD_off_time==15) BACKLIGHT_OFF_TIME=120; 
  else if (LCD_off_time==16) BACKLIGHT_OFF_TIME=300;  //5 min.
  else if (LCD_off_time==17) BACKLIGHT_OFF_TIME=600;  //10 min.
  else if (LCD_off_time==18) BACKLIGHT_OFF_TIME=1800; //30 min.
  else if (LCD_off_time==19) BACKLIGHT_OFF_TIME=3600; //1 hour 
  else BACKLIGHT_OFF_TIME=0; //NEVER
//--------------------------------------------------------------------------------

//█████████████████████████████████████████████████
  while (1) {
    vPortYield(); ///
    vTaskDelay___(1); ///
//--------------------------------------------------------------------------------
#if DEBUG > 3
    if (INPUTREAD()) {
      INPUTREPORT();
    }
#endif

    EVENTS();

    if (MENU_ACTIVE == 0 || MENU_ACTIVE == 1) {
      MENU_TASK(MENU_ITEM);
    }
    if (MENU_ACTIVE < 0) MENU_ACTIVE = 0; //MENU TASK Activation Request
    if (MENU_ACTIVE == 2) {
      MENU_ACTIVE = -1;

//█████████████████████████████████████████████████
//█████████████████████████████████████████████████
//█████████████████████████████████████████████████
      EXIT = 0;
      POWER = 0;
      BATTERY_METER_ACTIVE = 0; //disable battery meter for now...
      AUDIOREADY=0; 

      DO_DEINIT_CLOCK();
           
      switch (MENU_ITEM) {
      case 0:
        printf("================================================================================\n");
        printf("Starting WATCHES MENU\n");
        printf("================================================================================\n");
        MEMORY_STATUS();

        DO_SETUP_CLOCK();

        MENU_ACTIVE = 1; //not need menu reactivation in this case
        break;
//████████████████████████████████████████████████
      case 1:
        printf("================================================================================\n");
        printf("Starting NES EMULATOR\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
#if NES_EMULATOR
        DRAWMODE = 1;
        set_font(font_Ishmeria8x16); //default font
        screenmemory_fillscreen(0); //black color       
//--------------------------------------------------------------------------------
        LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
        while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
          vPortYield();       
          if (LOADED_ROM == 1) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, (char * )"/NES/"); ///must be allocated 256 Bytes
              Browse_MICROSD(APP_EMU_NES, MAINPATH, (char * )"microSD: Select NES ROM image");
          }
          if (LOADED_ROM == 11) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, (char * )"/"); ///must be allocated 256 Bytes
              Browse_FLASH(APP_EMU_NES, MAINPATH, (char * )"FLASH: Select NES ROM image");
          }
          if (LOADED_ROM == 21) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, (char * )"0:/NES/"); ///must be allocated 256 Bytes
              Browse_USB(APP_EMU_NES, MAINPATH, (char * )"USB: Select NES ROM image");
          }
        }
//--------------------------------------------------------------------------------
        if (EXIT == 2) break; ///

        if (LOADED_ROM == 2) LOADED_ROM = 3;
        else if (LOADED_ROM == 12) LOADED_ROM = 13;
        else if (LOADED_ROM == 22) LOADED_ROM = 23;

        screenmemory_fillscreen(0); //black color

        if (DEBUG) Serial.print("> SELECTED NES: ");
        if (DEBUG) Serial.println(MAINPATH);

        delay___(300);        
//--------------------------------------------------------------------------------
#ifndef NCAT_NO_PSRAM
        while (LOADED_ROM != 4 && LOADED_ROM != 14 && LOADED_ROM != 24) {
          vPortYield();        
          if (LOADED_ROM == 3) {
            screenmemory_fillscreen(0); //black color
            DATA = PSRAM_load_file(MAINPATH, & DATA_SIZE);
            LOADED_ROM = 4;
          }
          if (LOADED_ROM == 13) {
            screenmemory_fillscreen(0); //black color
            DATA = PSRAM_load_flash(MAINPATH, & DATA_SIZE);
            LOADED_ROM = 14;
          }
          if (LOADED_ROM == 23) {
            screenmemory_fillscreen(0); //black color
            DATA = PSRAM_load_usbstorage(MAINPATH, & DATA_SIZE);
            LOADED_ROM = 24;
          }
        }
#else
        DATA = FLASH_load_sdcard(MAINPATH, &DATA_SIZE); 
#endif  
        screenmemory_fillscreen(0); //black color

        delay___(300);
//--------------------------------------------------------------------------------
#ifndef NCAT_NO_PSRAM
        null_page = (uint8_t * ) ps_malloc(NES6502_BANKSIZE); ///4kB memory
        audio_frame[0] = (int16_t * ) ps_malloc(DEFAULT_FRAGSIZE * 4); ///
        audio_frame[1] = (int16_t * ) ps_malloc(DEFAULT_FRAGSIZE * 4); ///
#else
        null_page = (uint8_t * ) malloc(NES6502_BANKSIZE); ///4kB memory
        audio_frame[0] = (int16_t * ) malloc(DEFAULT_FRAGSIZE * 4); ///
        audio_frame[1] = (int16_t * ) malloc(DEFAULT_FRAGSIZE * 4); ///
#endif
#if PWMAUDIO_ENABLED
        AUDIO_SET_PARAMS(DEFAULT_SAMPLERATE, 16, 1, 0);
#else    
        AUDIO_SET_PARAMS(DEFAULT_SAMPLERATE, 16, 1, 0);
#endif
        init_sound(); //START AUDIO
        
        //NES machine CREATE
        if (NULL == NESmachine) NESmachine = nes_create();
        if (NULL == NESmachine) {
          if (DEBUG) Serial.println("Failed to create NES instance.");
          while (1) {} //FREEZE
        }
        if (DEBUG) Serial.println("NES Inserting cartridge.");      
//--------------------------------------------------------------------------------
        if (NULL == NESmachine -> rominfo) 
        NESmachine -> rominfo = (rominfo_t * ) malloc(sizeof(rominfo_t));
        if (NULL == NESmachine -> rominfo) goto rom_load_fail;
        
        memset(NESmachine -> rominfo, 0, sizeof(rominfo_t));
        
        // Get the header and stick it into rominfo struct
        if (rom_getheader( & DATA, NESmachine -> rominfo)) goto rom_load_fail;
        
        // Make sure we really support the mapper
        if (false == mmc_peek(NESmachine -> rominfo -> mapper_number)) {
          if (DEBUG) {
            Serial.print("Mapper not yet implemented:");
            Serial.println(NESmachine -> rominfo -> mapper_number);
          }
          goto rom_load_fail;
        }
        
        MEMORY_STATUS();
        
        // iNES format doesn't tell us if we need SRAM, so we have to always allocate it -- bleh!* UNIF, TAKE ME AWAY!  AAAAAAAAAA!!!
        if (rom_allocsram( & NESmachine -> rominfo)) goto rom_load_fail; ///NEED FOR example: SMB2-LostLevels
        rom_loadtrainer( & DATA, NESmachine -> rominfo);
        if (rom_loadrom( & DATA, NESmachine -> rominfo)) goto rom_load_fail;        
        goto rom_load_success;        
//................................................................................
      rom_load_fail:
        if (DEBUG) {
          Serial.println("NES ROMLOAD FAIL.");
        }                
        goto nes_exit; // continue to menu.     
//................................................................................
      rom_load_success:
        if (DEBUG) {
          Serial.println("NES ROMLOAD SUCCESS.");
        }
        // map cart's SRAM to CPU $6000-$7FFF
        if (NESmachine -> rominfo -> sram) {
          NESmachine -> cpu -> mem_page[6] = NESmachine -> rominfo -> sram;
          NESmachine -> cpu -> mem_page[7] = NESmachine -> rominfo -> sram + 0x1000;
        }
        // mapper
        NESmachine -> mmc = mmc_create(NESmachine -> rominfo);
        if (NULL == NESmachine -> mmc) goto inscart_fail;
        // if there's VRAM, let the PPU know
        if (NULL != NESmachine -> rominfo -> vram) NESmachine -> ppu -> vram_present = true;
        if (SOUND_ENABLED) apu_setext(NESmachine -> apu, NESmachine -> mmc -> intf -> sound_ext);
        if (SOUND_ENABLED) osd_setsound(NESmachine -> apu -> process);
        build_address_handlers(NESmachine);
        nes_setcontext(NESmachine);

        //NES RESET:
        nes_reset();
        goto inscart_success;
//................................................................................
      inscart_fail:
        if (DEBUG) {
          Serial.println("Insert Cartridge Fail!");
        }
        goto nes_exit; // continue to menu.
//................................................................................
      inscart_success:
        if (DEBUG) {
            Serial.println("NES Insert Cartridge OK.");
        }
        #if I2SAUDIO_ENABLED
            i2s_start((i2s_port_t) AUDIO_I2SPORT);
        #endif
        POWER = APP_EMU_NES;   
        DRAWMODE = 2;
        SETPAL(APP_EMU_NES);

        //NES START!
        AUDIOREADY=1;              
//--------------------------------------------------------------------------------
        while (POWER == APP_EMU_NES) { //NES EMULATION LOOP
          tickcnt = micros___();
          INPUTCLEAR();                    
          if (INPUTREAD()) {
              //INPUTREPORT();
          }
#if VGA_OUTPUT
          FRAME_COUNTER++;
          if (FRAME_COUNTER%2 && VGA_READY == 0) VGA_READY = 1;
          nes_renderframe(true);
#else          
          nes_renderframe(true);
#endif                                       
          tickcnt = micros___() - tickcnt;

          if (AUDIOREADY == 0) AUDIOREADY = 1;          

          if (LCD_ENABLED) if (MULTITASK_MODE == 1) xSemaphoreGive(draw_mutex); //refresh LCD         
          //if (DRAWREADY == 0) DRAWREADY = 1;
                    
          if (tickcnt < 1000000 / NES_REFRESH_RATE) vTaskDelay___(1000 / NES_REFRESH_RATE - tickcnt / 1000); //delay before next frame
          if (((INPUT_START == 1 && INPUT_SELECT == 1) || INPUT_SYSTEM == 1) && !INPUT_A && !INPUT_B) { //EXIT NES EMULATOR CALL
            if (InGame_Menu == 2) {
              vTaskDelay___(100);
              do_window_menu();
              INPUTCLEAR();
            } else {
              AUDIOREADY=0;
              POWER = 0; //exit
            }
          }
//----------------------------------------------------------------------------------------
        }
        DRAWMODE=0;       
      nes_exit:
        POWER = 0;
        delay___(500);
        osd_stopsound(); ///must be delayed because crash...
        AUDIO_SILENCE();

        free(audio_frame[0]);
        free(audio_frame[1]);
        free(null_page); ///

        if (NESmachine -> rominfo -> sram != NULL) free(NESmachine -> rominfo -> sram); ///free SRAM
                                        
#ifndef NCAT_NO_PSRAM
        free(DATA); //free NES ROM data
#endif
        screenmemory_fillscreen_static(0);
#endif
        break;
//████████████████████████████████████████████████
      case 2:
        printf("================================================================================\n");
        printf("Starting GBC EMULATOR\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
#if GBC_EMULATOR
        DRAWMODE = 1;
//--------------------------------------------------------------------------------
#if SOUND_ENABLED
        AUDIO_SET_PARAMS(GBC_SAMPLERATE, 16, 1 , 0);
        //GBC audio using 8 buffers (...changed to 2 used buffers)
#ifndef NCAT_NO_PSRAM
        audio_frame[0] = (int16_t * ) ps_malloc(GBC_FRAGSIZE * 4); ///
        audio_frame[1] = (int16_t * ) ps_malloc(GBC_FRAGSIZE * 4); ///
        audio_frame[2] = (int16_t * ) ps_malloc(GBC_FRAGSIZE * 4); ///
        audio_frame[3] = (int16_t * ) ps_malloc(GBC_FRAGSIZE * 4); ///
        audio_frame[4] = (int16_t * ) ps_malloc(GBC_FRAGSIZE * 4); ///
        audio_frame[5] = (int16_t * ) ps_malloc(GBC_FRAGSIZE * 4); ///
        audio_frame[6] = (int16_t * ) ps_malloc(GBC_FRAGSIZE * 4); ///
        audio_frame[7] = (int16_t * ) ps_malloc(GBC_FRAGSIZE * 4); ///
#else
        audio_frame[0] = (int16_t * ) malloc(GBC_FRAGSIZE * 4); ///
        audio_frame[1] = (int16_t * ) malloc(GBC_FRAGSIZE * 4); ///
#endif                        
        gbc_I2S_enabled = 1;
#endif
        screenmemory_fillscreen(63); //black color

        sprintf(MAINPATH, "/GBC/");

        LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
//--------------------------------------------------------------------------------
        while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
          vPortYield();
          if (LOADED_ROM == 1) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "/GBC/"); ///must be allocated 256 Bytes
            Browse_MICROSD(APP_EMU_GBC, MAINPATH, (char * )"microSD: Select GBC ROM image");
          }
          if (LOADED_ROM == 11) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "/"); ///must be allocated 256 Bytes
            Browse_FLASH(APP_EMU_GBC, MAINPATH, (char * )"FLASH: Select GBC ROM image");
          }
          if (LOADED_ROM == 21) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "0:/GBC/"); ///must be allocated 256 Bytes
            Browse_USB(APP_EMU_GBC, MAINPATH, (char * )"USB: Select GBC ROM image");
          }
        }
        delay___(300);

        if (EXIT == 2) break; ///

        if (LOADED_ROM == 2) LOADED_ROM = 3;
        else if (LOADED_ROM == 12) LOADED_ROM = 13;
        else if (LOADED_ROM == 22) LOADED_ROM = 23;

        screenmemory_fillscreen(0); //black color

        if (DEBUG) Serial.print("> SELECTED GBC: ");
        if (DEBUG) Serial.println(MAINPATH);
        delay___(300);
//--------------------------------------------------------------------------------
#ifndef NCAT_NO_PSRAM
        while (LOADED_ROM != 4 && LOADED_ROM != 14 && LOADED_ROM != 24) {
          vPortYield();
          if (LOADED_ROM == 3) {
            screenmemory_fillscreen(0); //black color
            uint32_t FILESIZE = getfilesize_SD(MAINPATH);
            if (FILESIZE <= 2097152) { //<2MB GBC ROM FILE loaded in PSRAM
              DATA = PSRAM_load_file(MAINPATH, & DATA_SIZE);
              GBC_IN_HIMEM = 0;
              LOADED_ROM = 4;
            } else if (FILESIZE > 2097152 && FILESIZE <= 4194304) { // 2MB..4MB GBC ROM FILE loaded in HIMEM
              HIMEM_load_file(MAINPATH, & DATA_SIZE);
              GBC_IN_HIMEM = 1;
              for (uint8_t tmp=0;tmp<64;tmp++) GBC_HIMEM_BANK[tmp]=(uint8_t*)ps_malloc(16384); //1MB psram of ROMBANKS for HIMEM 4MB ROMs              
              LOADED_ROM = 4;
            } else {
              EXIT = 2;
              break;
            }
          }
          if (LOADED_ROM == 13) {
            screenmemory_fillscreen(0); //black color
            DATA = PSRAM_load_flash(MAINPATH, & DATA_SIZE);
            LOADED_ROM = 14;
          }
          if (LOADED_ROM == 23) {
            screenmemory_fillscreen(0); //black color
            DATA = PSRAM_load_usbstorage(MAINPATH, & DATA_SIZE);
            LOADED_ROM = 24;
          }
        }
#else
        DATA = FLASH_load_sdcard(MAINPATH, &DATA_SIZE); 
        GBC_IN_HIMEM = 0;
        LOADED_ROM = 4;                
#endif                
        delay(300);
//--------------------------------------------------------------------------------
        screenmemory_fillscreen(0); //black color GBC pal
        SETPAL(0);

        AUDIO_SET_PARAMS(AUDIO_HZ, 16, 1, 0);

        POWER = APP_EMU_GBC;
        
        DRAWREADY = 1; 
        AUDIOREADY = 1;

        MEMORY_STATUS();
        gb_hw_rambanks[0]=(byte*)malloc(4096);
        gb_hw_rambanks[1]=(byte*)malloc(4096);
        gb_hw_rambanks[2]=(byte*)malloc(4096);
        gb_hw_rambanks[3]=(byte*)malloc(4096);
        gb_hw_rambanks[4]=(byte*)malloc(4096);
        gb_hw_rambanks[5]=(byte*)malloc(4096);
        gb_hw_rambanks[6]=(byte*)malloc(4096);
        gb_hw_rambanks[7]=(byte*)malloc(4096);
        gb_lcd_vbank[0]=(byte*)malloc(8192 *2);
        gb_lcd_vbank[1]=&gb_lcd_vbank[0][8192];
        MEMORY_STATUS();

        gnuboy_load_rom((const char*)DATA);

        //vid_preinit();
        vid_init();
        pcm_init();

        gnuboy_init();
        gnuboy_reset(1);

        MEMORY_STATUS();

        DRAWMODE = 4;
        printf("GBC Starting emulator.\n");
        DRAWREADY = 1; 
        AUDIOREADY = 1;
//--------------------------------------------------------------------------------
        gbc_frame_time = micros___();

        lcd.out.enabled = 1;
        while (POWER == APP_EMU_GBC) {

          vPortYield(); 
          //delay___(1); //needs for disable bluetooth crash on CORE0? Maybe...
                        
          cpu_emulate(2280);
          while (R_LY > 0 && R_LY < 144) cpu_emulate(lcd.cycles);

          rtc_tick();
          sound_mix();  

          if (AUDIOREADY == 0) AUDIOREADY = 1;
   
          ev_poll();
                        
          if (AUDIO_TIMER + 32 <= millis___()) { /// fix if audio wasn't processed until 64ms...                          
            printf(">>> SOUNDMIX UNDERLOAD %d ms \n",(int)(millis___()-AUDIO_TIMER));
            //AUDIOTask();
            //sound_mix();
          }  

          printf("%d",  lcd.out.enabled );
          if (framenum % 60 == 0) {
            printf("\n");
          }                        
          framenum++;

          deltatime = micros___() - gbc_frame_time;                      
//----------------------------------------------------------------------------------------
          if (skipped >= 3) { //minimally 20fps required...                       
            skipped = 0;
            gbc_frame_time = micros___();
            lcd.out.enabled = 1;           
          } else if ((skipped + 1) * 16743 < deltatime) {
            skipped++;
            lcd.out.enabled = 0;
          } else {           
            if ((((skipped + 1) * 16743) - deltatime) > 0) delayMicroseconds___((((skipped + 1) * 16743) - deltatime));            
            skipped = 0;
            gbc_frame_time = micros___();            
            lcd.out.enabled = 1;            
            if (MULTITASK_MODE == 1) xSemaphoreGive(draw_mutex);
          }
//----------------------------------------------------------------------------------------                   
          if (!(R_LCDC & 0x80)) cpu_emulate(32832);
          while (R_LY > 0) // wait for next frame 
          cpu_emulate(lcd.cycles);                  
        }
//----------------------------------------------------------------------------------------
        AUDIOREADY=0;

        delay___(500); //we are using multitasking, waiting for sound_mix finish
                    
        printf("GBC Terminated emulator.\n");

        free(gb_hw_rambanks[7]);
        free(gb_hw_rambanks[6]);
        free(gb_hw_rambanks[5]);
        free(gb_hw_rambanks[4]);
        free(gb_hw_rambanks[3]);
        free(gb_hw_rambanks[2]);
        free(gb_hw_rambanks[1]);
        free(gb_hw_rambanks[0]);
        
        free(gb_lcd_vbank[0]);
        gb_lcd_vbank[1]=NULL;

        free(audioBuffer);

        gnuboy_free_rom();            

        AUDIO_SILENCE();
        
        free(audio_frame[0]);
        free(audio_frame[1]);
        free(audio_frame[2]);
        free(audio_frame[3]);
        free(audio_frame[4]);
        free(audio_frame[5]);
        free(audio_frame[6]);
        free(audio_frame[7]);

        SETPAL(0);
        if (GBC_IN_HIMEM == 0) {
#ifndef NCAT_NO_PSRAM
          free(DATA);
#endif
        } else {
          for (uint8_t tmp=0;tmp<64;tmp++) {
            free(GBC_HIMEM_BANK[tmp]);
            GBC_HIMEM_BANK_SELECTED[tmp]=0;
          }
          if (GBC_HIMEM_BANK_BUFFER_ALLOCATED) {
            free(GBC_HIMEM_BANK_BUFFER);
            GBC_HIMEM_BANK_BUFFER_ALLOCATED=0;
          }
          if (GBC_HIMEM_BANK0_BUFFER_LOADED) {      
            free(GBC_HIMEM_BANK0_BUFFER);  
            GBC_HIMEM_BANK0_BUFFER_LOADED=0;                    
          }  
        }                                     
        DRAWMODE = 0;
        delay___(100); ///small delay is required when switching DRAWMODE
        screenmemory_fillscreen_static(0);
#endif
        break;
//████████████████████████████████████████████████
      case 3:
        printf("================================================================================\n");
        printf("Starting SMS EMULATOR\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
#if SMS_EMULATOR
        DRAWMODE = 1;
        set_font(font_Ishmeria8x16); //default font
        screenmemory_fillscreen(0); //black color
//--------------------------------------------------------------------------------
        LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
        while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
          vPortYield();

          if (LOADED_ROM == 1) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "/SMS/"); ///must be allocated 256 Bytes
            Browse_MICROSD(APP_EMU_SMS, MAINPATH, (char * )"microSD: Select SMS ROM image");
          }
          if (LOADED_ROM == 11) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "/"); ///must be allocated 256 Bytes
            Browse_FLASH(APP_EMU_SMS, MAINPATH, (char * )"FLASH: Select SMS ROM image");
          }
          if (LOADED_ROM == 21) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "0:/SMS/"); ///must be allocated 256 Bytes
            Browse_USB(APP_EMU_SMS, MAINPATH, (char * )"USB: Select SMS ROM image");
          }
        }
        delay___(300);
//--------------------------------------------------------------------------------
        if (EXIT == 2) break; ///
        if (LOADED_ROM == 2) LOADED_ROM = 3;
        else if (LOADED_ROM == 12) LOADED_ROM = 13;
        else if (LOADED_ROM == 22) LOADED_ROM = 23;
        screenmemory_fillscreen(0); //black color
        if (DEBUG) Serial.print("> SELECTED SMS: ");
        if (DEBUG) Serial.println(MAINPATH);
        delay___(300);
//--------------------------------------------------------------------------------
#ifndef NCAT_NO_PSRAM
        while (LOADED_ROM != 4 && LOADED_ROM != 14 && LOADED_ROM != 24) {               
          vPortYield();
          if (LOADED_ROM == 3) {
            screenmemory_fillscreen(0); //black color
            cart.rom = PSRAM_load_file(MAINPATH, & cart.size);
            LOADED_ROM = 4;
          }
          if (LOADED_ROM == 13) {
            screenmemory_fillscreen(0); //black color
            cart.rom = PSRAM_load_flash(MAINPATH, & cart.size);
            LOADED_ROM = 14;
          }
          if (LOADED_ROM == 23) {
            screenmemory_fillscreen(0); //black color
            cart.rom = PSRAM_load_usbstorage(MAINPATH, & cart.size);
            LOADED_ROM = 24;
          }
        }
#else
        cart.rom = FLASH_load_sdcard(MAINPATH, &cart.size); 
#endif
        delay___(300);

        AUDIO_SET_PARAMS(SNDRATE, 16, 2, 0);
           
        if (DEBUG) Serial.print("> SELECTED SMS: ");
        if (DEBUG) Serial.println(MAINPATH);

        screenmemory_fillscreen(0); //black color
        delay___(300);
        cart.pages = (cart.size / 0x4000);
        cart.type = TYPE_SMS; // default sms settings

        // console type detection // SMS Header is located at 0x7ff0
        if ((cart.size > 0x7000) && (!memcmp( & cart.rom[0x7ff0], "TMR SEGA", 8))) {
        uint8_t region = (cart.rom[0x7fff] & 0xf0) >> 4;
        switch (region) {
          case 5:
            cart.type = TYPE_GG;
            break;
          case 6:
          case 7:
            cart.type = TYPE_GG;
            break;
          case 3:
            cart.type = TYPE_SMS;
            break;
          default:
            cart.type = TYPE_SMS;
            break;
          }
        }
//--------------------------------------------------------------------------------
        MEMORY_STATUS();
#if I2SAUDIO_ENABLED
        i2s_start((i2s_port_t) AUDIO_I2SPORT);
#endif
        POWER = APP_EMU_SMS;
//--------------------------------------------------------------------------------      
        SZ = (UINT8 * ) malloc(256 * sizeof(UINT8));
        SZ_BIT = (UINT8 * ) malloc(256 * sizeof(UINT8));
        SZP = (UINT8 * ) malloc(256 * sizeof(UINT8));
        SZHV_inc = (UINT8 * ) malloc(256 * sizeof(UINT8));
        SZHV_dec = (UINT8 * ) malloc(256 * sizeof(UINT8));

        int frameno, frameTgt;
        int lastTickCnt, tickCnt;
        int didFrame;
        sms.use_fm = 0;
        sms.country = TYPE_OVERSEAS;

        sms.ram = (unsigned char * ) malloc(0x2000);
#ifndef NCAT_NO_PSRAM
        vdp_vram = (uint8 * ) ps_malloc(0x4000);
#else
        vdp_vram = (uint8 * ) malloc(0x4000);
#endif

        cachePtr = (int16 * ) malloc(512 * 4 * sizeof(int16)); ///can not be ps_malloc!
        cacheStore = (uint8 * ) malloc(CACHEDTILES * 64);
        cacheStoreUsed = (uint8 * ) malloc(CACHEDTILES);

        //for (uint32_t tmp=0;tmp<512*4;tmp++) cachePtr[tmp]=0; ///clear cachePtr
        for (uint32_t tmp = 0; tmp < CACHEDTILES * 64; tmp++) cacheStore[tmp] = 0; ///clear cacheStore
        for (uint32_t tmp = 0; tmp < CACHEDTILES; tmp++) cacheStoreUsed[tmp] = 0; ///clear cacheStoreUsed

#ifndef NCAT_NO_PSRAM
        appIramData.sram = (unsigned char * ) ps_malloc(0x8000); ///
#else
        appIramData.sram = (unsigned char * ) malloc(0x8000); ///
#endif
        //appIramData.videodata = (unsigned char*) malloc(256*192  + 1); /// PS malloc cases slowering draw, malloc causes overflow

        for (uint32_t tmp = 0; tmp < 240; tmp++) memset(SCREENMEMORY_LINE[tmp], 22, 256);
        for (uint32_t tmp = 0; tmp < 192; tmp++) appIramData.videodata[tmp] = SCREENMEMORY_LINE[tmp + ((240 - 192) / 2)] - 8; //link SCREENMEMORY_LINE

        if (!appIramData.sram) Serial.println("appIramData.sram malloc Failed!");
        if (!appIramData.videodata) Serial.println("appIramData.videodata malloc Failed!");

        sms.dummy = appIramData.videodata[0]; //A normal cart shouldn't access this memory ever. Point it to vram just in case.
        sms.sram = appIramData.sram;
        //bitmap.data=appIramData.videodata;
        for (uint32_t tmp = 0; tmp < 192; tmp++) bitmap.data[tmp] = appIramData.videodata[tmp];

        bitmap.width = 256;
        bitmap.height = 192;
        bitmap.pitch = 256;
        bitmap.depth = 8;

        Serial.println("SMS EMU system init");

        emu_system_init(SNDRATE);

        init_sound_();
        printf("Sound buffer: %d samples, enabled=%d.\n", snd_.bufsize, snd_.enabled);
        lastTickCnt = 0;
        didFrame = 0;

        Serial.println("SMS EMU system reset");

        system_reset(); ////

        printf("Loop Start\n");
//--------------------------------------------------------------------------------
        DRAWMODE = 3;
        bitmap.pal.update=1; ///
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
        while (POWER == APP_EMU_SMS) {
          //tickCnt would be 100tick/sec, but because we're running at double the clock speed without informing
          //the RTOS, it's 1000tick/sec
          for (frameno = 0; frameno < SMS_FPS; frameno++) {
            if (POWER == 0) break;
            tickCnt = xTaskGetTickCount___();
            if (tickCnt == lastTickCnt) tickCnt++;
            frameTgt = ((tickCnt - lastTickCnt) * SMS_FPS) / TIMEMS___(Tickrate);
            frameTgt += didFrame; //Try do diffuse frames a bit... otherwise, because of the low
            //granularity of the FreeRTOS tick variable, the drawn frames will be 'clumped together'.
            if (frameTgt <= frameno) {
              INPUTCLEAR();
              INPUTREAD();
//--------------------------------------------------------------------------------
              //Convert JOY/KEY buttons to SMS buttons
              int smsButtons;
              int smsSystem;
              smsButtons = 0;
              smsSystem = 0;
              if (INPUT_UP) smsButtons |= _INPUT_UP;
              if (INPUT_DOWN) smsButtons |= _INPUT_DOWN;
              if (INPUT_LEFT) smsButtons |= _INPUT_LEFT;
              if (INPUT_RIGHT) smsButtons |= _INPUT_RIGHT;
              if (INPUT_A) smsButtons |= _INPUT_BUTTON1;
              if (INPUT_B) smsButtons |= _INPUT_BUTTON2;
              if (INPUT_START) smsSystem |= _INPUT_START;
              if (INPUT_SELECT) smsSystem |= _INPUT_PAUSE;
        
              input.pad[0] = smsButtons;
              input.system = smsSystem;
        
              if ((INPUT_START && INPUT_SELECT) || (INPUT_SYSTEM)) {
                if (InGame_Menu == 2) {
//--------------------------------------------------------------------------------
                  //change some colors in palette for InGame Menu
                  adj_32bit[0x1e] = 0x80808000;
                  adj_32bit[0x1f] = 0xffffff00;
                  bitmap.pal.color[0x1e][0] = 0x7f; //R
                  bitmap.pal.color[0x1e][1] = 0x7f; //G
                  bitmap.pal.color[0x1e][2] = 0x7F; //B
                  bitmap.pal.color[0x1f][0] = 0xff; //R
                  bitmap.pal.color[0x1f][1] = 0xff; //G
                  bitmap.pal.color[0x1f][2] = 0xff; //B
        
                  do_window_menu();
                  INPUTCLEAR();
                  vTaskDelay___(100);
        
                  // Restore palette
                  for (uint8_t i = 0; i < PALETTE_SIZE; i += 1) palette_sync(i);
                  bitmap.pal.update = 1; //restore palette
//--------------------------------------------------------------------------------
                  if (POWER == 0) break; /// !!!
                } else {
                  POWER = 0; //exit smsemu
                }
              }
//--------------------------------------------------------------------------------
              sms_frame(0);       
              if (LCD_ENABLED) xSemaphoreGive(draw_mutex); //refresh LCD
              vPortYield(); ///
              didFrame = 3;
              printf("1");
            } else {
              sms_frame(1);
              if (didFrame != 0) didFrame--;
              printf("0");
            }
            if (OVERCLOCK)
              if ((xTaskGetTickCount___() - tickCnt) < 1000 / SMS_FPS) delay((1000 / SMS_FPS) - (xTaskGetTickCount___() - tickCnt)); //delay before next frame
            if (AUDIOREADY == 0) AUDIOREADY = 1;
            //xSemaphoreGive(audio_mutex); //refresh SOUND
            vPortYield(); ///
//----------------------------------------------------------------------------------------
          }
          tickCnt = xTaskGetTickCount___();
          if (tickCnt == lastTickCnt) tickCnt++;
          printf("\nfps=%d ", (SMS_FPS * TIMEMS___(Tickrate)) / (tickCnt - lastTickCnt));
          lastTickCnt = tickCnt;
        }
        AUDIOREADY=0;
        DRAWMODE = 0;
        POWER = 0;
        MEMORY_STATUS();

        printf("Terminating SMS emulator...\n");

        AUDIO_SILENCE();

        free(audio_frame[0]);
        free(audio_frame[1]);

        free(snd_.buffer[0]);
        free(snd_.buffer[1]);

        free(appIramData.sram);

        if (sms.ram) free(sms.ram);
        free(vdp_vram);

        free(cachePtr);
        free(cacheStore);
        free(cacheStoreUsed);
#ifndef NCAT_NO_PSRAM                
        free(cart.rom); ///Free SMS ROM Data
#endif
        free(SZ);
        free(SZ_BIT);
        free(SZP);
        free(SZHV_inc);
        free(SZHV_dec);

        DRAWMODE = 0;
        SETPAL(0);
        screenmemory_fillscreen_static(0);
#endif
        MEMORY_STATUS();
        break;
//████████████████████████████████████████████████
      case 4:
        printf("================================================================================\n");
        printf("Starting AUDIO PLAYER APPLICATION\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
#if MICROSD
        DRAWMODE = 1;
#if I2S_ENABLED
        player_I2S_enabled = true;
#else
        player_I2S_enabled = false;
#endif
        screenmemory_fillscreen(0); //black color
        sprintf(MAINPATH, "/AUDIO/");
        //malloc FILENAME array
        for (uint16_t tmp = 0; tmp < MAXFILES; tmp++) {
#ifndef NCAT_NO_PSRAM
          filename[tmp] = (char * ) ps_calloc(MAXFILENAME_LENGTH, sizeof(char));
#else 
          filename[tmp] = (char * ) calloc(MAXFILENAME_LENGTH, sizeof(char));
#endif                 
        }
        
#ifndef NCAT_NO_PSRAM
        audio.m_outBuff = (int16_t * ) ps_malloc(2048 * 2 * 2); //malloc 16bit * 2*2048
#else 
        audio.m_outBuff = (int16_t * ) malloc(2048 * 2 * 2); //malloc 16bit * 2*2048
#endif                        
        audio.setPinout(I2S_bck_io_num, I2S_ws_io_num, I2S_data_out_num);
        audio.setVolume(VOLUME * 100 / 1163); // 0..255 => 0...21        
//................................................................................
        while (!EXIT) {
//--------------------------------------------------------------------------------
          LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
          while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
            vPortYield();          
            if (LOADED_ROM == 1) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, "/AUDIO/"); ///must be allocated 256 Bytes
              Browse_MICROSD(APP_AUD_PLAYER, MAINPATH, (char * ) "microSD: Select AUDIO file");
            }  
          }
          PLAYINGFILE = SELECTED_FILE_POS; ///play selected file           
          TOTALFILES = onlyFileNames; //without DIRS!
//--------------------------------------------------------------------------------
          if (EXIT) break;
          delay___(300);
            
          Serial.println(MAINPATH);
          Serial.print(PLAYINGFILE + 1);
          Serial.print('/');
          Serial.println(TOTALFILES);
          Serial.println(MAINPATH);
          
          audio.connecttoFS(SD, MAINPATH);
          
          screenmemory_fillscreen(0); //black color
          set_font_XY(16, 12);
          sprintf(TEXTBUFFER, "%d/%d", PLAYINGFILE + 1, TOTALFILES);
          draw_string(TEXTBUFFER, 255);
          
          set_font_XY(16, 240 - 32);
          draw_string(TRACKNAME, 255);
          
          set_font_XY(16, 12 + 16);
          sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH ", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
          draw_string(TEXTBUFFER, 255);
          
          uint32_t prevtime = millis___();
          
          POWER = APP_AUD_PLAYER; //Playing Flag
          PAUSED = false;
          
          visualyze_READY = 1;

          uint32_t AUDIO_TIME_COUNTER = millis___();
//................................................................................
          while (POWER == APP_AUD_PLAYER) {
            audio.loop();
            if (AUDIO_TIME_COUNTER + 40 < millis___()) { //40ms refresh enough? yes...
              uint16_t h = 0;
              uint8_t m = 0;
              uint8_t s = 0;
              secondsToHMS(audio.getAudioCurrentTime(), h, m, s);
              set_font_XY(240 - 160, 12);
              sprintf(TEXTBUFFER, "%02d:%02d:%02d / ", h, m, s);
              draw_string(TEXTBUFFER, 0xff);
              secondsToHMS(audio.getAudioFileDuration(), h, m, s);
              sprintf(TEXTBUFFER, "%02d:%02d:%02d", h, m, s);
              draw_string(TEXTBUFFER, 0xff);

              if (DRAWREADY == 0 && POWER == APP_AUD_PLAYER && visualyze_READY == 1) {
                DRAWREADY = 1;
                visualyze(); //visualyze when playing audio... but not in CORE 0
                //xSemaphoreGive(draw_mutex);               
                if (AUDIOREADY == 0) AUDIOREADY = 1;
              }
              if (INPUTREAD()) {
                //INPUTREPORT();
              }
              AUDIO_TIME_COUNTER = millis___();
            }
            if (INPUT_A || INPUT_B || INPUT_SELECT || INPUT_START || INPUT_UP || INPUT_DOWN || INPUT_LEFT || INPUT_RIGHT) delay___(1);
            //PAUSE
            if ((INPUT_SELECT == 0 && INPUT_START == 1) || INPUT_A == 1) {
              audio.pauseResume();
              if (PAUSED == false) PAUSED = true;
              else PAUSED = false;
              AUDIO_SILENCE();
              delay(200);
              INPUTCLEAR();
            }
            //VOLUME+
            if ((INPUT_UP == 1) && prevtime + 200 < millis___()) {
              if (VOLUME <= 255 - 8) VOLUME += 8;
              else VOLUME = 255;
              audio.setVolume(VOLUME * 100 / 1163); // 0...21
              set_font_XY(16, 12 + 16);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              prevtime = millis___();
              INPUTCLEAR();
            }
            //VOLUME-
            if ((INPUT_DOWN == 1) && prevtime + 200 < millis___()) {
              if (VOLUME >= 8) VOLUME -= 8;
              else VOLUME = 0;
              audio.setVolume(VOLUME * 100 / 1163); // 0...21
              set_font_XY(16, 12 + 16);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              prevtime = millis___();
              INPUTCLEAR();
            }
            //PLAY NEXT FILE AFTER END
            if (PLAYINGFILE < TOTALFILES - 1 && !audio.isRunning() && PAUSED == false) {
              if (PLAYINGFILE < TOTALFILES - 1) PLAYINGFILE++;
              AUDIO_SILENCE();
              PREVNEXT(MAINPATH, PLAYINGFILE);
              if (DEBUG) {
                  Serial.print(PLAYINGFILE + 1);
                  Serial.print('/');
                  Serial.println(TOTALFILES);
                  Serial.println(MAINPATH);
              }
              audio.stopSong();
              audio.connecttoFS(SD, MAINPATH);
              screenmemory_fillscreen(0); //black color
              set_font_XY(16, 12);
              sprintf(TEXTBUFFER, "%d/%d", PLAYINGFILE + 1, TOTALFILES);
              draw_string(TEXTBUFFER, 255);
              set_font_XY(16, 240 - 24);
              draw_string(TRACKNAME, 255);
              set_font_XY(16, 12 + 16);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              delay(300);
            }
            //NEXT TRACK
            if (INPUT_RIGHT == 1) {
              if (PLAYINGFILE < TOTALFILES - 1) PLAYINGFILE++;
              AUDIO_SILENCE();
              PREVNEXT(MAINPATH, PLAYINGFILE);
              if (DEBUG) {
                Serial.print(PLAYINGFILE + 1);
                Serial.print('/');
                Serial.println(TOTALFILES);
                Serial.println(MAINPATH);
              }
              audio.stopSong();
              audio.connecttoFS(SD, MAINPATH);
              screenmemory_fillscreen(0); //black color
              set_font_XY(16, 12);
              sprintf(TEXTBUFFER, "%d/%d", PLAYINGFILE + 1, TOTALFILES);
              draw_string(TEXTBUFFER, 255);
              set_font_XY(16, 240 - 24);
              draw_string(TRACKNAME, 255);
              set_font_XY(16, 12 + 16);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              INPUTCLEAR();
              delay(200);
            }
            //PREV TRACK
            if (INPUT_LEFT == 1) {
              if (PLAYINGFILE > 0) PLAYINGFILE--;
              AUDIO_SILENCE();                  
              PREVNEXT(MAINPATH, PLAYINGFILE);
              if (DEBUG) {
                Serial.print(PLAYINGFILE + 1);
                Serial.print('/');
                Serial.println(TOTALFILES);
                Serial.println(MAINPATH);
              }
              audio.stopSong();
              audio.connecttoFS(SD, MAINPATH);
              delay(200);
              screenmemory_fillscreen(0); //black color
              set_font_XY(16, 12);
              sprintf(TEXTBUFFER, "%d/%d", PLAYINGFILE + 1, TOTALFILES);
              draw_string(TEXTBUFFER, 255);
              set_font_XY(16, 240 - 24);
              draw_string(TRACKNAME, 255);
              set_font_XY(16, 12 + 16);
              sprintf(TEXTBUFFER, "VOL:%d%% %dHz %dbit %dCH", VOLUME * 100 / 256, audio.getSampleRate(), audio.getBitsPerSample(), audio.getChannels());
              draw_string(TEXTBUFFER, 255);
              INPUTCLEAR();
            }
            //EXIT
            if ((INPUT_SELECT == 1 && INPUT_START == 1) || (INPUT_SYSTEM == 1)) {
              audio.stopSong();
              AUDIO_SILENCE();
              delay(200);
              INPUTCLEAR(); ///
              POWER = 0;
              EXIT = true;
            }
          }
        }
//................................................................................
        printf("Exiting Player application.\n");
        POWER = 0;
        audio.Uninstall();
        screenmemory_fillscreen(0); //black color
        delay(200);
#endif
        delay(200);

        break;
//████████████████████████████████████████████████
      case 5:
        printf("================================================================================\n");
        printf("Starting BLUETOOTH A2DP TASK\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
                
#if BLUETOOTH
        printf("Force_Bluetooth: %d, BLUETOOTH_APP: %d, BLUETOOTH_CONNECTIONS: %d\n", Force_Bluetooth, BLUETOOTH_APP, BLUETOOTH_CONNECTIONS);

        if (a2dp_and_avrcp_inited==0) {
          a2dp_and_avrcp_setup();
          a2dp_and_avrcp_inited=1;
        }
        
        DRAWMODE = 1;
        
        INPUTCLEAR();
        vTaskDelay___(200);
        
        screenmemory_fillscreen(0); //black color
        
        while (BLUETOOTH_APP < 0) {
          Force_Bluetooth = 2; ///BLUETOOTH not INITED
          vTaskDelay___(1000);
        }
        
        if (BLUETOOTH_APP == 0) {
          START_BT();
          vTaskDelay___(1000);
        }

        //STOP BLUETOOTH SCAN IS REQUIRED FOR PERFORMANCE...
        gap_inquiry_stop(); //top scan devices
        BLUETOOTH_APP = 3;
        hci_set_master_slave_policy(HCI_ROLE_SLAVE);
        
        POWER = APP_AUD_BTSPKER;

        MEMORY_STATUS();
        while (POWER == APP_AUD_BTSPKER) {
//--------------------------------------------------------------------------------
          //CORE 0 PWM audio PWM_SAMPRATE_REQUEST:
#if PWMAUDIO_ENABLED
          if (PWM_SAMPRATE_REQUEST) {
            //THIS IS AND MUST BE CALLED FROM CORE0 !!!
            pwm_audio_deinit();
            pac.duty_resolution = LEDC_TIMER_8_BIT;
            pac.gpio_num_left = PWMAUDIO_CONFIG_LEFT_CHANNEL_GPIO;
            pac.ledc_channel_left = LEDC_CHANNEL_0;
            pac.gpio_num_right = PWMAUDIO_CONFIG_RIGHT_CHANNEL_GPIO;
            pac.ledc_channel_right = LEDC_CHANNEL_1;
            pac.ledc_timer_sel = LEDC_TIMER_0;
            pac.tg_num = TIMER_GROUP_0; ///must be differt
            pac.timer_num = TIMER_0;
            pac.ringbuf_len = PWM_RINGBUFFER_SIZE; ///
            pwm_audio_init( & pac);                       
            pwm_audio_set_param(PWM_SAMPRATE_REQUEST, (ledc_timer_bit_t) 16, 2); ///NEEDS CORRECTION
            pwm_audio_start();
            pwm_audio_set_volume(0.99);
            PWM_SAMPRATE_REQUEST = 0;
          }
#endif
//--------------------------------------------------------------------------------
          visualyze();
              
          SYSTEM_TASK();
          xSemaphoreGive(draw_mutex); ///refresh LCD
          INPUTCLEAR();
          if (INPUTREAD()) {
            if ((INPUT_START && INPUT_SELECT) || (INPUT_SYSTEM)) POWER = 0; //exit smsemu
          }
        }
        AUDIO_SILENCE();

        a2dp_sink_disconnect(a2dp_cid); 

        hci_set_master_slave_policy(HCI_ROLE_MASTER);

        gap_inquiry_start(INQUIRY_INTERVAL);
        BLUETOOTH_APP = 2;
        screenmemory_fillscreen(0); //black color
#endif
        break;
//█████████████████████████████████████████████████
      case 6:
        printf("================================================================================\n");
        printf("Starting FAKE86 EMULATOR\n");
        printf("================================================================================\n");
        MEMORY_STATUS();

        set_font(font_Ishmeria8x16); //default font

        DRAWMODE = 1;
#if FAKE86_EMULATOR

        DISKETTE_DRIVE_A_INSERTED = 0;
        DISKETTE_DRIVE_B_INSERTED = 0;
//--------------------------------------------------------------------------------
        LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
        while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
          vPortYield();
          if (LOADED_ROM == 1) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "/DOS/"); ///must be allocated 256 Bytes
            Browse_MICROSD(APP_EMU_FAKE86, MAINPATH, (char * )"microSD: SELECT IMAGE (A:) DRIVE");
          }
          if (LOADED_ROM == 11) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "/"); ///must be allocated 256 Bytes
            Browse_FLASH(APP_EMU_FAKE86, MAINPATH, (char * )"FLASH: SELECT IMAGE (A:) DRIVE");
          }
          if (LOADED_ROM == 21) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "0:/DOS/"); ///must be allocated 256 Bytes
            Browse_USB(APP_EMU_FAKE86, MAINPATH, (char * )"USB: SELECT IMAGE (A:) DRIVE");
          }
          //printf("\n\n>>>>>>> LOADED_ROM: %d EXIT: %d \n\n ",LOADED_ROM,EXIT);
        }
//--------------------------------------------------------------------------------
        strcpy(DATAPATH1, MAINPATH); //dest ,src

        if (LOADED_ROM == 2) LOADED_ROM = 3;
        else if (LOADED_ROM == 12) LOADED_ROM = 13;
        else if (LOADED_ROM == 22) LOADED_ROM = 23;

        while (DISKETTE_DRIVE_A_INSERTED == 0 && LOADED_ROM != 4 && LOADED_ROM != 14 && LOADED_ROM != 24) {
          vPortYield();
          DISKETTE_DRIVE_A_INSERTED = insertdisk(0, DATAPATH1); //FLOPPY A in PSRAM ... 1.44MB max.! return 1 == success, return 0 == fail.
        }
//--------------------------------------------------------------------------------
        if (EXIT == 2) break; ///
        LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST (FOR HIMEM)
//--------------------------------------------------------------------------------
        if (PSRAM_HIMEM_SIZE > 0) {
          LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
          while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
            vPortYield();
            if (LOADED_ROM == 1) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, "/DOS/"); ///must be allocated 256 Bytes
              Browse_MICROSD(APP_EMU_FAKE86, MAINPATH, (char * )"microSD: SELECT IMAGE (B:) DRIVE");
            }
            if (LOADED_ROM == 11) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, "/"); ///must be allocated 256 Bytes
              Browse_FLASH(APP_EMU_FAKE86, MAINPATH, (char * )"FLASH: SELECT IMAGE (B:) DRIVE");
            }
            if (LOADED_ROM == 21) {
              screenmemory_fillscreen(0); //black color
              sprintf(MAINPATH, "0:/DOS/"); ///must be allocated 256 Bytes
              Browse_USB(APP_EMU_FAKE86, MAINPATH, (char * )"USB: SELECT IMAGE (B:) DRIVE");
            }
          }
//--------------------------------------------------------------------------------
          if (PSRAM_HIMEM_SIZE > 0) {
            strcpy(DATAPATH2, MAINPATH); //dest, src          
            if (LOADED_ROM == 2) LOADED_ROM = 3;
            else if (LOADED_ROM == 12) LOADED_ROM = 13;
            else if (LOADED_ROM == 22) LOADED_ROM = 23;
        
            while (DISKETTE_DRIVE_B_INSERTED == 0 && LOADED_ROM != 4 && LOADED_ROM != 14 && LOADED_ROM != 24) {
              vPortYield();
              DISKETTE_DRIVE_B_INSERTED = insertdisk(1, DATAPATH2); //FLOPPY B in HIMEM ... 2.88MB max.! return 1 == success, return 0 == fail.
            }
          }
        }

        if (EXIT == 2) break; ///
        if (DEBUG>2) {
          printf("> SELECTED IMAGE 1: %s\n", DATAPATH1);
          printf("> SELECTED IMAGE 2: %s\n", DATAPATH2);
        }
        delay___(300);
        
        set_font(font_CP437_4x8); //DOS FONT
//================================================================================      
        HIMEM_READBUFFER = (uint8_t * ) malloc(512);      //512 bytes sector = can not be PSRAM!!!
        Serial.print("FAKE86 INIT MEMORY:");
        RAM = (uint8_t * ) ps_malloc(RAM_MEM_SIZE);       //1MB (MAIN MEMORY)
        VRAM = (uint8_t * ) ps_malloc(VRAM_SIZE + 8192);  //250kB (VIDEO MEMORY) + 8kB overflow ... I dont know where...
        portram = (uint8_t * ) ps_malloc(PORTS_MEM_SIZE); //64kB (PORTS)
        portcallback_ = (portcallback_t * ) ps_malloc(sizeof(portcallback_t)); ///512kB (PORTS CB)
        Serial.print("portcallback_t size:");
        Serial.println(sizeof(portcallback_t));
        memset(RAM, 0, RAM_MEM_SIZE);
        memset(VRAM, 0, VRAM_SIZE);
        memset(portram, 0, PORTS_MEM_SIZE);
        memset(portcallback_, 0, sizeof(portcallback_t));
        VGA_SC = (uint16_t * ) malloc(0x100 * sizeof(uint16_t));
        VGA_CRTC = (uint16_t * ) malloc(0x100 * sizeof(uint16_t));
        VGA_ATTR = (uint16_t * ) malloc(0x100 * sizeof(uint16_t));
        VGA_GC = (uint16_t * ) malloc(0x100 * sizeof(uint16_t));

        memset(VGA_SC, 0, 0x100 * sizeof(uint16_t));
        memset(VGA_CRTC, 0, 0x100 * sizeof(uint16_t));
        memset(VGA_ATTR, 0, 0x100 * sizeof(uint16_t));
        memset(VGA_GC, 0, 0x100 * sizeof(uint16_t));

        if (NULL == blaster) blaster = (blaster_s * ) ps_malloc(sizeof(blaster_s));
        palettecga = (uint32_t * ) malloc(16 * sizeof(uint32_t));
        palettevga = (uint32_t * ) malloc(256 * sizeof(uint32_t));

        printf("[ OK ]\n");
//================================================================================    
        printf("FAKE86 INIT HARDWARE...");

        loadrom(0xFE000UL, (uint8_t * )"pcxtbios", 1); ///8kB
        loadrom(0xF6000UL, (uint8_t * )"rombasic", 0); ///32kB
        loadrom(0xC0000UL, (uint8_t * )"videorom", 1); ///32kB

        POWER = APP_EMU_FAKE86;

        reset86();
        fake86_inithardware();

        bootdrive = 0; // floppy A=0, floppy B=0, ROMBASIC=255

        printf("[ OK ]\n");
//================================================================================
        printf("FAKE86 START:\n");

        screenmemory_fillscreen(0); //black color

        static uint32_t cursorprevtick, cursorcurtick;
        cursorprevtick = millis___();

        keyboardcounter_repeat = millis___();
        keyboardcounter_pause = millis___();
//--------------------------------------------------------------------------------
        PS2KEYBOARD[0x1f]=0; //fix force pressed INSERT KEY //I don know why is pressed?
        FAKE86_KEYBOARD();
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
        while (POWER == APP_EMU_FAKE86) {

          if (OVERCLOCK > 0) {
            exec86(100000);
          } else {
            exec86(10000);
          }

          INPUTCLEAR();
          if (INPUTREAD()) {
            if (!USBKEYBOARD[0x01] && !BTKEYBOARD[0x01] && (INPUT_START && INPUT_SELECT) || (INPUT_SYSTEM)) POWER = 0; //exit fake86
          }
          if (scrmodechange) doscrmodechange();
          //BLINK CURSOR IN TEXTMODE
          cursorcurtick = millis___();
          if ((cursorcurtick - cursorprevtick) >= 300) {
            if (vidmode == 0 || vidmode == 1 || vidmode == 2 || vidmode == 3 || vidmode == 7 || vidmode == 0x82) {
              //updatedscreen = 1;
              cursorvisible = ~cursorvisible & 1;
              cursorprevtick = cursorcurtick;
              uint16_t x_cursor = cursx * 4;
              uint16_t y_cursor = cursy * 8;
              if (cursorvisible) draw_char_xy(x_cursor, y_cursor, '_', DisplayFontSet, 63, 0);
              else draw_char_xy(x_cursor, y_cursor, ' ', DisplayFontSet, 63, 0);
            }
          }
          xSemaphoreGive(draw_mutex); //DRAW function + refresh LCD
          vPortYield(); ///

          if ((INPUTREAD()) && (INPUT_SYSTEM == 1 || (BTKEYBOARD[0x01] && BTKEYBOARD[0x47]))) POWER = 0;
        }
        vTaskDelay___(1000);
        DRAWMODE = 0;
        screenmemory_fillscreen_static(0);

        free(HIMEM_READBUFFER);

        free(RAM);
        free(VRAM);
        free(portram);
        free(portcallback_);


        if (blaster) free(blaster);
        if (palettecga) free(palettecga);
        if (palettevga) free(palettevga);
        if (VGA_SC) free(VGA_SC);
        if (VGA_CRTC) free(VGA_CRTC);
        if (VGA_ATTR) free(VGA_ATTR);
        if (VGA_GC) free(VGA_GC);

        ejectdisk(0);
        set_font(font_Ishmeria8x16); //default font
#endif
        MEMORY_STATUS();
        break;
//█████████████████████████████████████████████████
      case 7:
        printf("================================================================================\n");
        printf("Starting SETTINGS\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
        delay(300);
        BATTERY_METER_ACTIVE = 1;
#if SETTINGS_OPTIONS
        while (INPUTREAD()) {
          vTaskDelay___(1);
        };
        INPUTCLEAR();
        SETTINGS_POSITION = 0;
        EEPROM_load_settings();
        //screenmemory_fillscreen_static(0);
        POWER = APP_SETTINGS;
        while (POWER == APP_SETTINGS) {
          Do_Settings_Menu();
          UPDATE_STATUS_BAR();
        }
        if (SETTINGS_CHANGED) REQUIRE_RESTART();
#ifdef NCAT_NO_PSRAM
        screenmemory_fillscreen_static(0);
#endif
        
#endif
        break;
//████████████████████████████████████████████████
      case 8:
        printf("================================================================================\n");
        printf("Starting KM SWITCH\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
    
        DRAWMODE = 0; ///!
        POWER = APP_KMSWITCH;
    
        screenmemory_fillscreen_static(0);
        draw_char_xy_transparent_double((240 - 64) / 2, (240 - 128) / 2, '0', font_7SEGMENT, 0xffff); //64x128
        while (POWER == APP_KMSWITCH) {
          if (SCROLLLOCK_millis > 0 && SCROLLLOCK_millis + 500 < millis()) { //wait 0.5 second to switch scrolllock
            SCROLLLOCK_millis = 0; //reset SCROLLLOCK call
    
            //Switch ScrollLock 1..6
            if (SCROLLLOCK == 0 && KM_max_devices > 0) SCROLLLOCK = 1;
            else if (SCROLLLOCK == 1 && KM_max_devices > 1) SCROLLLOCK = 2;
            else if (SCROLLLOCK == 2 && KM_max_devices > 2) SCROLLLOCK = 3;
            else if (SCROLLLOCK == 3 && KM_max_devices > 3) SCROLLLOCK = 4;
            else if (SCROLLLOCK == 4 && KM_max_devices > 4) SCROLLLOCK = 5;
            else if (SCROLLLOCK == 5 && KM_max_devices > 5) SCROLLLOCK = 6;
            else SCROLLLOCK = 1;
            printf("SCROLLLOCK KMSWITCH: %d\n", SCROLLLOCK);
            DRAW_KM_INFO();
          }
          if (SCROLLLOCK == 0) {
            vTaskDelay___(500);
            if (SCROLLLOCK == 0 && INPUTREAD()) {
                INPUTREPORT();
                if ((INPUT_START && INPUT_SELECT) || (INPUT_SYSTEM)) POWER = 0; //exit KM switch
            }
            vTaskDelay___(100);
          }
        }
        break;
//████████████████████████████████████████████████
      case 9:
        printf("================================================================================\n");
        printf("Starting OSCILLOSCOPE\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
#if COMPOSITE_VIDEO
#if OSCILLOSCOPE
        for (uint32_t tmp = 0; tmp < BUFFERS_COUNT; tmp++) dma_buff[tmp] = (uint16_t * ) heap_caps_malloc((NUM_SAMPLES) * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);

        DRAWMODE = 1; ///!
        POWER = APP_OSCILLOSCOPE;

        //adc1_config_width(ADC_WIDTH_BIT_12);
        //adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); 

        rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 0); ///21.4772727272Mhz
        I2S0.sample_rate_conf.tx_bck_div_num = 2;
        I2S0.sample_rate_conf.rx_bck_div_num = 2; //2x scale time base

        REINIT_ADC();

        adc_i2s_mode_init(ADC_UNIT_1, ADC_CHANNEL_0);
        adc_power_on();
        delay___(250);
        I2S0.conf.rx_start = 1; /// start DMA ADC
//--------------------------------------------------------------------------------
        while (POWER == APP_OSCILLOSCOPE) {
          if (INPUT_START == 1 && INPUT_SELECT == 1) { //EXIT
            INPUT_START = 0;
            INPUT_SELECT = 0;
            EXIT = true;
            POWER=0;
            delay(250);
          }
          if (INPUT_A == 1) { /// PAUSE
            delay(250);
            INPUTCLEAR();
            while (!INPUTREAD()) {
              delay(10);
            }
            INPUTCLEAR();
          }
          if (INPUT_START == 1) {
            if (spritecolor == 0b00011000) spritecolor = 0b11100000; //green to red
            else if (spritecolor == 0b11100000) spritecolor = 0b00000111; //red to blue
            else if (spritecolor == 0b00000111) spritecolor = 0b11111000; //blue to yellow
            else if (spritecolor == 0b11111000) spritecolor = 0b00011000; //yellow to green
            else spritecolor = 0b00011000;
          }
          if (INPUT_B == 1) { /// SHOW/HIDE info
            if (digital_wave_option == 0) digital_wave_option = 1;
            else if (digital_wave_option == 1) digital_wave_option = 2;
            else if (digital_wave_option == 2) digital_wave_option = 0;
            else digital_wave_option = 0;
          }
          if (INPUT_SELECT == 1) { /// SHOW/HIDE info
            if (info == true) info = false;
            else info = true;
          }
          if (INPUT_LEFT == 1) {
            switch (TIMEBASE) {
              case 10000:
                TIMEBASE = 5000;
                break;
              case 5000:
                TIMEBASE = 2000;
                break;
              case 2000:
                TIMEBASE = 1000;
                break;
              case 1000:
                TIMEBASE = 500;
                break;
              case 500:
                TIMEBASE = 200;
                break;
              case 200:
                TIMEBASE = 100;
                break;
              case 100:
                TIMEBASE = 50;
                break;
              case 50:
                TIMEBASE = 20;
                break;
              case 20:
                TIMEBASE = 10;
                break;
              case 10:
                TIMEBASE = 5;
                break;
              case 5:
                TIMEBASE = 2;
                break;
              case 2:
                TIMEBASE = 1;
                break;
              case 1:
                TIMEBASE = 1; //minimum us/div
                break;
              default:
                TIMEBASE = 10000;
            }
            timebase_switched = true;
          }
          if (INPUT_RIGHT == 1) {
            switch (TIMEBASE) {
              case 1:
                TIMEBASE = 2;
                break;
              case 2:
                TIMEBASE = 5;
                break;
              case 5:
                TIMEBASE = 10;
                break;
              case 10:
                TIMEBASE = 20;
                break;
              case 20:
                TIMEBASE = 50;
                break;
              case 50:
                TIMEBASE = 100;
                break;
              case 100:
                TIMEBASE = 200;
                break;
              case 200:
                TIMEBASE = 500;
                break;
              case 500:
                TIMEBASE = 1000;
                break;
              case 1000:
                //TIMEBASE = 2000;
                TIMEBASE = 1000; //stop here (disabled more than 1ms )
                break;
              case 2000:
                TIMEBASE = 5000;
                break;
              case 5000:
                TIMEBASE = 10000;
                break;
              case 10000:
                TIMEBASE = 10000; //maximum us/div
                break;
              default:
                TIMEBASE = 2;
            }
            timebase_switched = true;
          }

          INPUTCLEAR();
//--------------------------------------------------------------------------------
          peak_mean(BUFF_SIZE, & max_v, & min_v, & mean);
//--------------------------------------------------------------------------------
          //if analog mode OR auto mode and wave recognized as analog
          bool digital_data = false;
          if (digital_wave_option == 1) {
            trigger_freq_analog(sample_rate, mean, max_v, min_v, & freq, & period, & trigger0, & trigger1);
          } else if (digital_wave_option == 0) {
            digital_data = digital_analog(max_v, min_v);
            if (!digital_data) {
              trigger_freq_analog(sample_rate, mean, max_v, min_v, & freq, & period, & trigger0, & trigger1);
            } else {
              trigger_freq_digital(sample_rate, mean, max_v, min_v, & freq, & period, & trigger0);
            }
          } else {
            trigger_freq_digital(sample_rate, mean, max_v, min_v, & freq, & period, & trigger0);
          }
//--------------------------------------------------------------------------------
          //Clear Screen
          screenmemory_fillscreen(0);

          //Draw Grid
          draw_grid();
//--------------------------------------------------------------------------------
          //DEBUG DATA
          if (DEBUG>2) {
            Serial.print("digital=1_analog=0: ");
            Serial.println(digital_analog(max_v, min_v));
            Serial.print("mean:");
            Serial.println(mean);
            Serial.print("max_v:");
            Serial.println(max_v);
            Serial.print("min_v:");
            Serial.println(min_v);
            Serial.print("freq:");
            Serial.println(freq);
            Serial.print("period:");
            Serial.println(period);
            Serial.print("trigger0:");
            Serial.println(trigger0);
          }
//--------------------------------------------------------------------------------
          //adjust TIMEBASE:
          if (timebase_switched == true) {
            I2S0.in_link.start = 0;
            switch (TIMEBASE) {             
              case 1:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 0); ///21.4772727272Mhz
                I2S0.sample_rate_conf.tx_bck_div_num = 2;
                I2S0.sample_rate_conf.rx_bck_div_num = 1; //2x scale time base
                break;           
              case 2:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 0); ///21.4772727272Mhz
                I2S0.sample_rate_conf.tx_bck_div_num = 2;
                I2S0.sample_rate_conf.rx_bck_div_num = 2; //2x scale time base
                break;
              case 5:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 0); ///21.4772727272Mhz
                I2S0.sample_rate_conf.tx_bck_div_num = 2;
                I2S0.sample_rate_conf.rx_bck_div_num = 2; //2x scale time base
                break;
              case 10:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 2; //4x scale time base
                break;
              case 20:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 2; //4x scale time base
                break;
              case 50:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 2; //4x scale time base
                break;
              case 100:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);              
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 4; //8x scale time base
                break;
              case 200:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 4; //8x scale time base
                break;
              case 500:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);             
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 4; //8x scale time base
                break;
              case 1000:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);            
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 4; //8x scale time base
                break;
              case 2000:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 4; //8x scale time base
                break;
              case 5000:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 8; //20x scale time base
                break;
              case 10000:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);             
                I2S0.sample_rate_conf.tx_bck_div_num = 1;
                I2S0.sample_rate_conf.rx_bck_div_num = 8; //20x scale time base
                break;
              default:
                rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2);             
                I2S0.sample_rate_conf.tx_bck_div_num = 2;
                I2S0.sample_rate_conf.rx_bck_div_num = 2; //2x scale time base
            }
            timebase_switched = false;
            if (DEBUG) Serial.println("timebase_switched-------------------------------------");
            I2S0.in_link.start = 1;
          }
//--------------------------------------------------------------------------------
          //calculate freq:
          switch (TIMEBASE) {
            case 1:
              rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 0); ///21.4772727272Mhz               
              freq = (float) freq * (float)(2);                
              break;
            case 2:
              rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 0); ///21.4772727272Mhz               
              break;
            case 5:
              rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 0); ///21.4772727272Mhz                
              break;
            case 10:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq * (float) 4.29554; //analog wave
              else freq = (float) freq / (float)(2.14777);
              break;
            case 20:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq * (float) 4.29554; //analog wave
              else freq = (float) freq / (float)(2.14777);
              break;
            case 50:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq * (float) 4.29554; //analog wave
              else freq = (float) freq / (float)(2.14777);
              break;
            case 100:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq * (float) 2.14777; //analog wave
              else freq = (float) freq / (float)(4.29554);
              break;
            case 200:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq * (float) 2.14777; //analog wave
              else freq = (float) freq / (float)(4.29554);
              break;
            case 500:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq * (float) 2.14777; //analog wave
              else freq = (float) freq / (float)(4.29554);
              break;
            case 1000:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq * (float) 2.14777; //analog wave
              else freq = (float) freq / (float)(4.29554);
              break;
            case 2000:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq * (float) 2.14777; //analog wave
              else freq = (float) freq / (float)(4.29554);
              break;
            case 5000:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq / (float) 2.14777; //analog wave
              else freq = (float) freq / (float) 8;
              break;
            case 10000:
              if ((digital_wave_option == 0 && digital_data == 0) || (digital_wave_option == 1)) freq = (float) freq / (float) 2.14777; //analog wave
              else freq = (float) freq / (float) 8;
              break;
            default:
              I2S0.sample_rate_conf.rx_bck_div_num = 2; //1x scale time base
          }            
//--------------------------------------------------------------------------------           
          float px_h_offset = (float)(TFT_WIDTH * ((1 - h_scale) / 2)); // vertical offset from scale/2 %        
          uint16_t n_data = 0, o_data = to_scale(dma_buff[0][0]);
            
          for (uint32_t i = 1; i < 240; i++) {
            uint32_t index = 0;
            
            switch (TIMEBASE) {
              case 2:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 1.1135 / 1.3333); ///waveform is triggered.... 2.0us/DIV
                break;
              case 5:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 2.8636 / 1.3333); ///waveform is triggered.... 5.0us/DIV
                break;
              case 10:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 5.7272 / 2 / 1.3333); ///waveform is triggered.... 10.0us/DIV
                break;
              case 20:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 1.1135 * 5 / 1.3333); ///waveform is triggered.... 10x2.0us/DIV
                break;
              case 50:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 2.7920 * 5 / 1.3333); ///waveform is triggered.... 10x5.0us/DIV
                break;
              case 100:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 5.5840 * 2.5 / 1.3333); ///waveform is triggered.... 10x10.0us/DIV
                break;
              case 200:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 11.1680 * 2.5 / 1.3333); ///waveform is triggered.... 2.0us/DIV
                break;
              case 500:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 27.9200 * 2.5 / 1.3333); ///waveform is triggered.... 5.0us/DIV
                break;
              case 1000:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 55.8400 * 2.5 / 1.3333); ///waveform is triggered.... 10.0us/DIV
                break;
              case 2000:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 111.6800 * 2.5 / 1.3333); ///waveform is triggered.... 2.0us/DIV
                break;
              case 5000:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 279.2000 / 2 * 5.0 / 1.3333); ///waveform is triggered.... 5.0us/DIV
                break;
              case 10000:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 558.4000 / 2 * 5.0 / 1.3333); ///waveform is triggered.... 10.0us/DIV
                break;
              default:
                index = px_h_offset + trigger0 + (uint32_t)((i + 1) * 0.944 / 1.3333); ///waveform is triggered.... 2.0us/DIV
            }
//--------------------------------------------------------------------------------
            //draw sprite:
            if (index < BUFF_SIZE - 240) {
              n_data = to_scale(dma_buff[index / NUM_SAMPLES][index % NUM_SAMPLES]);
              screenmemory_line(i - 1, o_data, i, n_data, spritecolor); 
              screenmemory_line(i, o_data + 1, i + 1, n_data - 1, spritecolor);
              o_data = n_data;
            }
          }
//--------------------------------------------------------------------------------
          if (info) {
            screenmemory_drawrectangle(shift + 12, 20, 240 - shift - 20, 36, 0xff);
        
            set_font_XY(shift + 15, 20 + 5);
            sprintf(TEXTBUFFER, "P-P:%.3fV", (float)((to_voltage(max_v) - to_voltage(min_v))));
            draw_string(TEXTBUFFER, 0xff);
        
            if (freq < 1000) sprintf(TEXTBUFFER, "%.3fHz", (float) freq);
            else if (freq < 1000000) sprintf(TEXTBUFFER, "%.1fkHz", (float)(freq / 1000));
            else sprintf(TEXTBUFFER, "----");
        
            //very high frequency on low time base
            if ((TIMEBASE >= 5000 && digital_wave_option == 1 && freq > 200) ||
                (TIMEBASE >= 5000 && digital_wave_option == 0 && digital_data == 0 && freq > 200)) sprintf(TEXTBUFFER, "----");
        
            //very low frequency on high time base
            if ((TIMEBASE <= 10 && digital_wave_option == 1 && freq < 2000) ||
                (TIMEBASE <= 10 && digital_wave_option == 0 && digital_data == 0 && freq < 2000)) sprintf(TEXTBUFFER, "----");
            set_font_XY(shift + 15, 20 + 20);
            draw_string(TEXTBUFFER, 0xff);
        
            if (mean > 1.0) sprintf(TEXTBUFFER, "Avg:%.2fV", (float) mean);
            else sprintf(TEXTBUFFER, "Avg:%.2fdmV", (float)(mean * 1000.0));
        
            if (digital_wave_option == 0 && digital_data == 0) sprintf(TEXTBUFFER, "AUTO:AN."); //AUTO:ANALOG
            else if (digital_wave_option == 0 && digital_data == 1) sprintf(TEXTBUFFER, "AUTO:DIG."); //AUTO:DIGITAL
            else if (digital_wave_option == 1) sprintf(TEXTBUFFER, "ANALOG"); //ANALOG
            else if (digital_wave_option == 2) sprintf(TEXTBUFFER, "DIGITAL"); //DIGITAL
            else sprintf(TEXTBUFFER, "UNKNOWN"); //DIGITAL
            set_font_XY(0 + 24, 24);
            draw_string(TEXTBUFFER, 0xff);

            if (TIMEBASE < 1000) sprintf(TEXTBUFFER, "T:%.0fus", (float)(TIMEBASE));
            else if (TIMEBASE < 1000000) sprintf(TEXTBUFFER, "T:%.0fms", (float)(TIMEBASE / 1000));
            else sprintf(TEXTBUFFER, "T:----us");
            set_font_XY(0 + 24, 256 - 24 - 32);
            draw_string(TEXTBUFFER, 0xff);
          }
        
          if (INPUTREAD()) {
            INPUTREPORT();
            if ((INPUT_START && INPUT_SELECT) || (INPUT_SYSTEM)) POWER = 0; //exit
          }         
          vTaskDelay___(100);
        }
        I2S0.conf.rx_start = 0; /// stop DMA ADC
        I2S0.in_link.start = 0;
         
        for (uint32_t tmp = 0; tmp < BUFFERS_COUNT; tmp++) heap_caps_free(dma_buff[tmp]);
#endif                
#endif         
        delay___(300);

        break;
//████████████████████████████████████████████████
      case 10:
        printf("================================================================================\n");
        printf("Starting SIGNAL GENERATOR\n");
        printf("================================================================================\n");
        MEMORY_STATUS();
#if COMPOSITE_VIDEO   
#if SIGNAL_GENERATOR 
        DRAWMODE = 1;
//--------------------------------------------------------------------------------
        LOADED_ROM = 1; // STARTS BROWSE SD CARD FIRST
        while (EXIT != 2 && LOADED_ROM != 2 && LOADED_ROM != 12 && LOADED_ROM != 22) {
          vPortYield(); ///
          if (LOADED_ROM == 1) {
            screenmemory_fillscreen(0); //black color
            sprintf(MAINPATH, "/GEN/"); ///must be allocated 256 Bytes
            Browse_MICROSD(APP_SIGNALGEN, MAINPATH, (char*)"microSD: Select BMP file");
          }
        }
//--------------------------------------------------------------------------------
        if (EXIT == 2) break; ///
        if (LOADED_ROM == 2) LOADED_ROM = 3;
        screenmemory_fillscreen(0); //black color
        if (DEBUG) printf("> SELECTED BMP: ");
        if (DEBUG) printf("%s\n",MAINPATH);
        delay___(300);
//--------------------------------------------------------------------------------
        while (LOADED_ROM != 4 && LOADED_ROM != 14 && LOADED_ROM != 24) {
          vPortYield();
          if (LOADED_ROM == 3) {
            screenmemory_fillscreen(0); //black color
            DATA = PSRAM_load_file(MAINPATH, & DATA_SIZE);
            LOADED_ROM = 4;
          }
        }
        delay___(300);
//--------------------------------------------------------------------------------
        int Width;
        int Height;
        uint8_t * DECODED_DATA;
        DECODED_DATA = decode_1bit_bmp(DATA, & Width, & Height, NULL, NULL); //
        if (Width!=255 || Height!=255) {
          printf("Bitmap size must be 255x255 pixels! Aborting.");
          break;
        }

        screenmemory_fillscreen(0); //black color

        DRAWMODE = 0;
        screenmemory_fillscreen_static(0); //black color
        printf("1bit BMP decoded Width: %d, Height: %d \n", Width, Height);
        uint8_t * CALCWAVE;
        CALCWAVE = (uint8_t * ) malloc(256 * sizeof(uint8_t));
        SIGWAVE = (uint16_t * ) malloc(512 * sizeof(uint16_t));
        memset(SIGWAVE, 0, 512 * sizeof(uint16_t));

        for (uint32_t XPOS = 0; XPOS < Width && XPOS < 256; XPOS += 8) {
          CALCWAVE[(XPOS + 0)] = ((DECODED_DATA[0 * (Width / 8) + (XPOS / 8)] >> 0) & 1);
          CALCWAVE[(XPOS + 1)] = ((DECODED_DATA[0 * (Width / 8) + (XPOS / 8)] >> 1) & 1);
          CALCWAVE[(XPOS + 2)] = ((DECODED_DATA[0 * (Width / 8) + (XPOS / 8)] >> 2) & 1);
          CALCWAVE[(XPOS + 3)] = ((DECODED_DATA[0 * (Width / 8) + (XPOS / 8)] >> 3) & 1);
          CALCWAVE[(XPOS + 4)] = ((DECODED_DATA[0 * (Width / 8) + (XPOS / 8)] >> 4) & 1);
          CALCWAVE[(XPOS + 5)] = ((DECODED_DATA[0 * (Width / 8) + (XPOS / 8)] >> 5) & 1);
          CALCWAVE[(XPOS + 6)] = ((DECODED_DATA[0 * (Width / 8) + (XPOS / 8)] >> 6) & 1);
          CALCWAVE[(XPOS + 7)] = ((DECODED_DATA[0 * (Width / 8) + (XPOS / 8)] >> 7) & 1);
        }
        for (uint32_t YPOS = 0; YPOS < Height; YPOS++) {
          for (uint32_t XPOS = 0; XPOS < Width; XPOS += 8) {
            if ((SIGWAVE[XPOS * 2 + 0] == 0) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 0) & 1) != CALCWAVE[(XPOS + 0)]) {
              CALCWAVE[(XPOS + 0)] = ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 0) & 1);
              SIGWAVE[XPOS * 2 + 0] = YPOS;
              SIGWAVE[XPOS * 2 + 1] = YPOS;
            }
            if ((SIGWAVE[XPOS * 2 + 2] == 0) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 1) & 1) != CALCWAVE[(XPOS + 1)]) {
              CALCWAVE[(XPOS + 1)] = ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 1) & 1);
              SIGWAVE[XPOS * 2 + 2] = YPOS;
              SIGWAVE[XPOS * 2 + 3] = YPOS;
            }
            if ((SIGWAVE[XPOS * 2 + 4] == 0) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 2) & 1) != CALCWAVE[(XPOS + 2)]) {
              CALCWAVE[(XPOS + 2)] = ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 2) & 1);
              SIGWAVE[XPOS * 2 + 4] = YPOS;
              SIGWAVE[XPOS * 2 + 5] = YPOS;
            }
            if ((SIGWAVE[XPOS * 2 + 6] == 0) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 3) & 1) != CALCWAVE[(XPOS + 3)]) {
              CALCWAVE[(XPOS + 3)] = ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 3) & 1);
              SIGWAVE[XPOS * 2 + 6] = YPOS;
              SIGWAVE[XPOS * 2 + 7] = YPOS;
            }
            if ((SIGWAVE[XPOS * 2 + 8] == 0) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 4) & 1) != CALCWAVE[(XPOS + 4)]) {
              CALCWAVE[(XPOS + 4)] = ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 4) & 1);
              SIGWAVE[XPOS * 2 + 8] = YPOS;
              SIGWAVE[XPOS * 2 + 9] = YPOS;
            }
            if ((SIGWAVE[XPOS * 2 + 10] == 0) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 5) & 1) != CALCWAVE[(XPOS + 5)]) {
              CALCWAVE[(XPOS + 5)] = ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 5) & 1);
              SIGWAVE[XPOS * 2 + 10] = YPOS;
              SIGWAVE[XPOS * 2 + 11] = YPOS;
            }
            if ((SIGWAVE[XPOS * 2 + 12] == 0) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 6) & 1) != CALCWAVE[(XPOS + 6)]) {
              CALCWAVE[(XPOS + 6)] = ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 6) & 1);
              SIGWAVE[XPOS * 2 + 12] = YPOS;
              SIGWAVE[XPOS * 2 + 13] = YPOS;
            }
            if ((SIGWAVE[XPOS * 2 + 14] == 0) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 7) & 1) != CALCWAVE[(XPOS + 7)]) {
              CALCWAVE[(XPOS + 7)] = ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 7) & 1);
              SIGWAVE[XPOS * 2 + 14] = YPOS;
              SIGWAVE[XPOS * 2 + 15] = YPOS;
            }
          }
        }
        //SCALE FROM 256/8*7=224
        uint32_t YTMP = 1;
        uint32_t XTMP = 0;
        for (uint32_t YPOS = 0; YPOS < Height * 7 / 8; YPOS++) {
          if (YPOS % 7 == 6) {
            XTMP = 0;
            for (uint32_t XPOS = 7; XPOS < Width * 7 / 8 + 7; XPOS += 7) {
              if (((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 0) & 1) && ((DECODED_DATA[(YTMP + 1) * (Width / 8) + (XTMP / 8)] >> 0) & 1)) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 0), YPOS, 0xffff);
              if (((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 1) & 1) && ((DECODED_DATA[(YTMP + 1) * (Width / 8) + (XTMP / 8)] >> 1) & 1)) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 1), YPOS, 0xffff);
              if (((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 2) & 1) && ((DECODED_DATA[(YTMP + 1) * (Width / 8) + (XTMP / 8)] >> 2) & 1)) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 2), YPOS, 0xffff);
              if (((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 3) & 1) && ((DECODED_DATA[(YTMP + 1) * (Width / 8) + (XTMP / 8)] >> 3) & 1)) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 3), YPOS, 0xffff);
              if (((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 4) & 1) && ((DECODED_DATA[(YTMP + 1) * (Width / 8) + (XTMP / 8)] >> 4) & 1)) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 4), YPOS, 0xffff);
              if (((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 5) & 1) && ((DECODED_DATA[(YTMP + 1) * (Width / 8) + (XTMP / 8)] >> 5) & 1)) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 5), YPOS, 0xffff);
              if (((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 6) & 1) && ((DECODED_DATA[(YTMP + 1) * (Width / 8) + (XTMP / 8)] >> 6) & 1) &&
                ((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 7) & 1) && ((DECODED_DATA[(YTMP + 1) * (Width / 8) + (XTMP / 8)] >> 7) & 1)) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 6), YPOS, 0xffff);
              XTMP += 8;
            }
            YTMP++;
          } else {
            XTMP = 0;
            for (uint32_t XPOS = 7; XPOS < Width * 7 / 8 + 7; XPOS += 7) {
              if ((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 0) & 1) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 0), YPOS, 0xffff);
              if ((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 1) & 1) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 1), YPOS, 0xffff);
              if ((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 2) & 1) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 2), YPOS, 0xffff);
              if ((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 3) & 1) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 3), YPOS, 0xffff);
              if ((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 4) & 1) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 4), YPOS, 0xffff);
              if ((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 5) & 1) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 5), YPOS, 0xffff);
              if (((DECODED_DATA[YTMP * (Width / 8) + (XTMP / 8)] >> 6) & 1) && ((DECODED_DATA[YPOS * (Width / 8) + (XPOS / 8)] >> 7) & 1)) screenmemory_drawpixel_static(Width * 7 / 8 - (XPOS + 6), YPOS, 0xffff);
              XTMP += 8;
              }
          }
          YTMP++;
        }

        for (uint32_t XTMP = 0; XTMP < 512; XTMP += 2) {
          //screenmemory_drawpixel_static(224 - (XTMP/2 *7/8 + 7),SIGWAVE[XTMP]*7/8,0b0000011111100000);
          screenmemory_line_static(224 - (XTMP / 2 * 7 / 8 + 7), SIGWAVE[XTMP + 1] * 7 / 8, 224 - (XTMP / 2 * 7 / 8 + 7) + 1, SIGWAVE[XTMP] * 7 / 8, 0b0000011111100000); //26=green //6=red ///DOUBLE LINE...
        }
//--------------------------------------------------------------------------------
        I2S0.clkm_conf.clkm_div_num = 1; // I2S clock divider’s integral value.
        POWER = APP_SIGNALGEN;

        uint8_t DIGIT0 = 0;
        uint8_t DIGIT1 = 0;
        uint8_t DIGIT2 = 0;
        uint8_t DIGIT3 = 0;
        uint8_t DIGIT4 = 0;
        uint8_t DIGIT_SETPOS = 0;
        uint8_t FREQ_MODE = 1;
        uint8_t CLKDIVME = 1;

        sprintf(TEXTBUFFER, " LEVEL=%d", FREQ_MODE);
        draw_string_xy_static(8, 0, TEXTBUFFER, font_Ishmeria8x16, 0xffff, 0);
        while (POWER == APP_SIGNALGEN) {
          if (INPUTREAD()) {
            INPUTREPORT();
            if ((INPUT_START && INPUT_SELECT) || (INPUT_SYSTEM)) POWER = 0; //exit SIGNALGEN        
            if (INPUT_B) {
              if (FREQ_MODE < 4) FREQ_MODE++;
              else FREQ_MODE = 0;        
              vTaskDelay___(250);
              sprintf(TEXTBUFFER, " LEVEL=%d", FREQ_MODE);
              draw_string_xy_static(8, 0, TEXTBUFFER, font_Ishmeria8x16, 0xffff, 0);
            }            
            if (INPUT_A) {
              DIGIT_SETPOS = 5;
              vTaskDelay___(250);
              double REQ_FREQ = (DIGIT0 * 1000.0) + (DIGIT1 * 10000.0) + (DIGIT2 * 100000.0) + (DIGIT3 * 1000000.0) + (DIGIT4 * 10000000.0);
        
              printf("REQ_FREQ: %lf Hz\n", REQ_FREQ);
        
              screenmemory_drawfillrectangle_static(8, 240 - 48, 200, 16 * 3, 0);
        
              CLKDIVME = 1;
              double FREQ_OUT = APLL_calc(REQ_FREQ, 1, FREQ_MODE, & CLKDIVME); //1=precise
        
              printf("rtc_clk_apll_enable: sdm0=%d, sdm1=%d, sdm2=%d, odiv=%d\n", bestSdm0, bestSdm1, bestSdm2, bestOdiv);
              I2S0.clkm_conf.clkm_div_num = 1; // I2S clock divider’s integral value.
              rtc_clk_apll_enable(1, bestSdm0, bestSdm1, bestSdm2, bestOdiv);
        
              I2S0.clkm_conf.clkm_div_num = CLKDIVME; // I2S clock divider’s integral value.
        
              double TIME_per_PIXEL = 1000000.0 / (double) FREQ_OUT * (684.0 / 255.0);
        
              sprintf(TEXTBUFFER, "FREQ_OUT=%lf Hz", FREQ_OUT);
              draw_string_xy_static(8, 240 - 48, TEXTBUFFER, font_Ishmeria8x16, 0xffff, 0);
        
              sprintf(TEXTBUFFER, "PIXEL time=%lf us", TIME_per_PIXEL);
              draw_string_xy_static(8, 240 - 32, TEXTBUFFER, font_Ishmeria8x16, 0xffff, 0);
        
              sprintf(TEXTBUFFER, "s0=%d,s1=%d,s2=%d,od=%d", bestSdm0, bestSdm1, bestSdm2, bestOdiv);
              draw_string_xy_static(8, 240 - 16, TEXTBUFFER, font_Ishmeria8x16, 0xffff, 0);
            }
            if (INPUT_LEFT) {
              if (DIGIT_SETPOS < 5) DIGIT_SETPOS++;
              else DIGIT_SETPOS = 0;
              vTaskDelay___(250);
            }
            if (INPUT_RIGHT) {
              if (DIGIT_SETPOS > 0) DIGIT_SETPOS--;
              else DIGIT_SETPOS = 5;
              vTaskDelay___(250);
            }
            if (INPUT_UP) {
              if (DIGIT_SETPOS == 0) {
                if (DIGIT0 < 9) DIGIT0++;
                else DIGIT0 = 0;
              }
              if (DIGIT_SETPOS == 1) {
                if (DIGIT1 < 9) DIGIT1++;
                else DIGIT1 = 0;
              }
              if (DIGIT_SETPOS == 2) {
                if (DIGIT2 < 9) DIGIT2++;
                else DIGIT2 = 0;
              }
              if (DIGIT_SETPOS == 3) {
                if (DIGIT3 < 9) DIGIT3++;
                else DIGIT3 = 0;
              }
              if (DIGIT_SETPOS == 4) {
                if (DIGIT4 < 9) DIGIT4++;
                else DIGIT4 = 0;
              }
              vTaskDelay___(250);
            }
            if (INPUT_DOWN) {
              if (DIGIT_SETPOS == 0) {
                if (DIGIT0 > 0) DIGIT0--;
                else DIGIT0 = 9;
              }
              if (DIGIT_SETPOS == 1) {
                if (DIGIT1 > 0) DIGIT1--;
                else DIGIT1 = 9;
              }
              if (DIGIT_SETPOS == 2) {
                if (DIGIT2 > 0) DIGIT2--;
                else DIGIT2 = 9;
              }
              if (DIGIT_SETPOS == 3) {
                if (DIGIT3 > 0) DIGIT3--;
                else DIGIT3 = 9;
              }
              if (DIGIT_SETPOS == 4) {
                if (DIGIT4 > 0) DIGIT4--;
                else DIGIT4 = 9;
              }
              vTaskDelay___(250);
            }

            if (DIGIT_SETPOS == 4) {
              draw_char_xy_static(240 - 96, 8, '0' + DIGIT4, font_Ishmeria8x16, 0, 0xffff);
            } else {
              draw_char_xy_static(240 - 96, 8, '0' + DIGIT4, font_Ishmeria8x16, 0xffff, 0);
            }
            if (DIGIT_SETPOS == 3) {
              draw_char_xy_static(240 - 96 + 8, 8, '0' + DIGIT3, font_Ishmeria8x16, 0, 0xffff);
            } else {
              draw_char_xy_static(240 - 96 + 8, 8, '0' + DIGIT3, font_Ishmeria8x16, 0xffff, 0);
            }
            if (DIGIT_SETPOS == 2) {
              draw_char_xy_static(240 - 96 + 16, 8, '0' + DIGIT2, font_Ishmeria8x16, 0, 0xffff);
            } else {
              draw_char_xy_static(240 - 96 + 16, 8, '0' + DIGIT2, font_Ishmeria8x16, 0xffff, 0);
            }
            if (DIGIT_SETPOS == 1) {
              draw_char_xy_static(240 - 96 + 24, 8, '0' + DIGIT1, font_Ishmeria8x16, 0, 0xffff);
            } else {
              draw_char_xy_static(240 - 96 + 24, 8, '0' + DIGIT1, font_Ishmeria8x16, 0xffff, 0);
            }
            if (DIGIT_SETPOS == 0) {
              draw_char_xy_static(240 - 96 + 32, 8, '0' + DIGIT0, font_Ishmeria8x16, 0, 0xffff);
            } else {
              draw_char_xy_static(240 - 96 + 32, 8, '0' + DIGIT0, font_Ishmeria8x16, 0xffff, 0);
            }

            draw_char_xy_static(240 - 96 + 48, 8, 'k', font_Ishmeria8x16, 0xffff, 0);
            draw_char_xy_static(240 - 96 + 56, 8, 'H', font_Ishmeria8x16, 0xffff, 0);
            draw_char_xy_static(240 - 96 + 64, 8, 'z', font_Ishmeria8x16, 0xffff, 0);

            INPUTCLEAR();
          }
          vTaskDelay___(10);
        }
        free(CALCWAVE);
        free(SIGWAVE);
        free(DECODED_DATA);
        free(DATA);

        rtc_clk_apll_enable(1, 0x46, 0x97, 0x4, 2); //RESTORE COMPOSITE VIDEO
        I2S0.clkm_conf.clkm_div_num = 1; // I2S clock divider’s integral value.
#endif
#endif
      }
//--------------------------------------------------------------------------------
      SETPAL(0);
      DRAWMODE = 0;
      delay___(100); //system needs small pause!

      BATTERY_METER_ACTIVE = 1;
      //reset all status variables = call redraw
      lastBATERYLEVEL = 0;
      lastVOLUME = 0;
      lastBATTERY = 0;
      lastUSBHOST_dev = -2;
      lastBTDEVICESCONNECTED = -1;
      lastBLUETOOTH_APP = -1;

      LAST_ACTIVITY_SECONDS=millis___()/1000; 
      wakeup_LCD();           
    }
  }
}


//████████████████████████████████████████████████
//█  EVENT TASK
//████████████████████████████████████████████████

void EVENTS() {
//--------------------------------------------------------------------------------
  if (eventCore1_counter + 1000 < millis___()) { // update EVENTS every 1second
    eventCore1_counter = millis___();
    ON_SECONDS=millis___()/1000;
    //if (DEBUG>3) printf("ON: %d\n",ON_SECONDS);
  } else return;
//--------------------------------------------------------------------------------
#if LCD_BACKLIGHT_CONTROL
#if AUTO_POWER_SAVER
  if (BACKLIGHT_OFF_TIME>0 && ON_SECONDS - LAST_ACTIVITY_SECONDS > BACKLIGHT_OFF_TIME && BACKLIGHT_PIN_LAST_VALUE==0 && LCD_BACKLIGHT>0) {
    if (DEBUG>1) printf("LCD OFF request...\n");
 
    BACKLIGHT_PIN_LAST_VALUE=LCD_BACKLIGHT;
    
    //FADE OFF EFFECT:
    for (int16_t lcd_fade_off=LCD_BACKLIGHT;lcd_fade_off>0;lcd_fade_off-=1) {
      BACKLIGHT_PIN_UPDATE(lcd_fade_off);
      delay___(5); //5ms 
    }

    LCD_BACKLIGHT=0;
   
    //and INSTANT OFF:
    BACKLIGHT_PIN_UPDATE(LCD_BACKLIGHT); ///backlight level was changed...    
  }
#endif  
#endif
//--------------------------------------------------------------------------------
#if defined(NCAT_BOARD) || defined(NCAT_LITE) || defined(NCAT_LOW_POWER)
#ifdef BATTERY_METTER_PIN
  if (BATTERY_METER_ACTIVE && BatteryLevelCounter + BATERY_READ_INTERVAL < millis___()) { //read battery value 
    pinMode(BATTERY_METTER_PIN, INPUT);

    //uint16_t BATTERY_RAW=analogRead(BATTERY_METTER_PIN);

    int BATTERY_RAW =0;
    BATTERY_RAW=adc1_get_raw((adc1_channel_t)ADC1_CHANNEL_3); //GPIO39
    BATTERY=BATTERY_RAW>>4;
    BatteryLevelCounter=millis___();
    printf("BATTERY: %d\n", BATTERY);    
    vTaskDelay___(5); 
  }
#endif        
#endif    
//--------------------------------------------------------------------------------
#if USBHOST
#if USBHOST_STORAGE
  if (USBhost_mass_storage == 2) {
    //Information icon activate
    if (current_state >= 0x50 && USB_HOST_devices < 1 && partsready) USB_HOST_devices = 1; 
    if (current_state >= 0x50 && !partsready) USB_HOST_devices = 0; 
    if (current_state <= 0x12 && USB_HOST_devices > 0) USB_HOST_devices = -1; 
  }
#endif  
#endif
//--------------------------------------------------------------------------------
  vPortYield();
#if BLUETOOTH
  if (BLUETOOTH_APP > 0 && nr_hci_connections() != BLUETOOTH_CONNECTIONS) {
    BLUETOOTH_CONNECTIONS = nr_hci_connections();
    printf("CONNECTIONS: %d\n", BLUETOOTH_CONNECTIONS);
  }
//--------------------------------------------------------------------------------
  //Can not shutdown scan when is some Connections... also can not shutdown when running some APP...?
  if (Force_Bluetooth != 2 && (POWER == 0 || POWER == APP_EMU_CLOCK) && BLUETOOTH_APP > 1 && BLUETOOTH_CONNECTIONS == 0 && BLUETOOTH_UPDATE_Counter + (BLUETOOTH_INQUIRY_SECONDS * 1000) < eventCore1_counter) {
    printf("Bluetooth scan: SHUTING DOWN! \n");
    gap_inquiry_stop();   //stop scan devices
    BLUETOOTH_APP = 1;
  }
//--------------------------------------------------------------------------------
  if (Force_Bluetooth != 2 && (POWER == 0 || POWER == APP_EMU_CLOCK) && BLUETOOTH_CONNECTIONS == 0 && BLUETOOTH_APP != 0 && BLUETOOTH_UPDATE_Counter + (BLUETOOTH_SHUTDOWN_SECONDS * 1000) < eventCore1_counter) {
    printf("Bluetooth no connections: SHUTING DOWN! \n");
    BLUETOOTH_APP = 0;
    SLEEP_BT();
    UPDATE_STATUS_BAR(); ///
  }
#endif
//--------------------------------------------------------------------------------
#if USBHOST
#if USBHOST_STORAGE
  if (USBhost_mass_storage == 2) {
    if (!SDCARD_BUSY) {
      FIL My_File_Object_x; /* File object */

      // We also will be needing to test each hub port, we don't do this yet!
      if (!change && !usbon && (int32_t)((uint32_t) millis___() - usbon_time) >= 0L) {
        change = true;
        usbon = true;
      }
      if (change) {
        change = false;
        if (usbon) {
          Usb.vbusPower(vbus_on);
          printf("USB: VBUS on\r\n");
        } else {
          Usb.vbusPower(vbus_off);
          usbon_time = (uint32_t) millis___() + 2000;
        }
      }
//--------------------------------------------------------------------------------
      current_state = Usb.getUsbTaskState();
      if (current_state != last_state) {
        if (UsbDEBUGlvl > 0x50) printf("USB: state = %x\r\n", current_state);
        if (current_state == USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE) {
          partsready = false;
          for (int i = 0; i < cpart; i++) {
            if (Fats[i] != NULL)
            delete Fats[i];
            Fats[i] = NULL;
          }
          fatready = false;
          notified = false;
          cpart = 0;
        }
        last_state = current_state;
      }
      //if (current_state>=0x50 && USB_HOST_devices<1 && partsready) USB_HOST_devices=1; ///Information icon activate
      //if (current_state>=0x50 && USB_HOST_devices>0 && !partsready) USB_HOST_devices=0; ///Information icon activate
      //if (current_state<=0x12 && USB_HOST_devices>0) USB_HOST_devices=-1; ///Information icon activate

      // only do any of this if usb is on
      if (usbon) {
        //printf("USB: usbon\n");
        if (partsready && !fatready) {
          if (cpart > 0) fatready = true;
        }
//--------------------------------------------------------------------------------
        for (int B = 0; B < MAX_USB_MS_DRIVERS; B++) {
          if ((!partsready) && (UHS_USB_BulkOnly[B] -> GetAddress())) {
            // Build a list.
            int ML = UHS_USB_BulkOnly[B] -> GetbMaxLUN();
            //printf("MAXLUN = %i\r\n", ML);
            ML++;
            for (int i = 0; i < ML; i++) {
              if (UHS_USB_BulkOnly[B] -> LUNIsGood(i)) {
                partsready = true;
                ((pvt_t * )(sto[i].private_data)) -> lun = i;
                ((pvt_t * )(sto[i].private_data)) -> B = B;
                sto[i].Reads = * UHS_USB_BulkOnly_Read;
                sto[i].Writes = * UHS_USB_BulkOnly_Write;
                sto[i].Status = * UHS_USB_BulkOnly_Status;
                sto[i].Initialize = * UHS_USB_BulkOnly_Initialize;
                sto[i].Commit = * UHS_USB_BulkOnly_Commit;
                sto[i].TotalSectors = UHS_USB_BulkOnly[B] -> GetCapacity(i);
                sto[i].SectorSize = UHS_USB_BulkOnly[B] -> GetSectorSize(i);
                printf_P("USB: LUN:\t\t%u\r\n", i);
                printf_P("USB: Total Sectors:\t%08lx\t%lu\r\n", sto[i].TotalSectors, sto[i].TotalSectors);
                printf_P("USB: Sector Size:\t%04x\t\t%u\r\n", sto[i].SectorSize, sto[i].SectorSize);
                // get the partition data...
                PT = new PCPartition;

                if (!PT -> Init( & sto[i])) {
                  part_t * apart;
                  for (int j = 0; j < 4; j++) {
                    apart = PT -> GetPart(j);
                    if (apart != NULL && apart -> type != 0x00) {
                      memcpy( & (parts[cpart]), apart, sizeof(part_t));
                      printf("USB: Partition %u type %#02x\r\n", j, parts[cpart].type);
                      // for now
                      if (isfat(parts[cpart].type)) {
                        Fats[cpart] = new PFAT( & sto[i], cpart, parts[cpart].firstSector);
                        //int r = Fats[cpart]->Good();
                        if (Fats[cpart] -> MountStatus()) {
                          delete Fats[cpart];
                          Fats[cpart] = NULL;
                        } else cpart++;
                      }
                    }
                  }
                } else {
                  // try superblock
                  Fats[cpart] = new PFAT( & sto[i], cpart, 0);
                  //int r = Fats[cpart]->Good();
                  if (Fats[cpart] -> MountStatus()) {
                    //printf_P(PSTR("Superblock error %x\r\n"), r);
                    delete Fats[cpart];
                    Fats[cpart] = NULL;
                  } else cpart++;
                }
                delete PT;
              } else {
                sto[i].Writes = NULL;
                sto[i].Reads = NULL;
                sto[i].Initialize = NULL;
                sto[i].TotalSectors = 0UL;
                sto[i].SectorSize = 0;
              }
            }
          }
        }
//--------------------------------------------------------------------------------
        if (fatready) {
          printf("USB: fatready\n");
          if (Fats[0] != NULL) {
            struct Pvt * p;
            p = ((struct Pvt * )(Fats[0] -> storage -> private_data));
            if (!UHS_USB_BulkOnly[p -> B] -> LUNIsGood(p -> lun)) {
              // media change
              partsready = false;
              for (int i = 0; i < cpart; i++) {
                if (Fats[i] != NULL) delete Fats[i];
                Fats[cpart] = NULL;
              }
              fatready = false;
              notified = false;
              cpart = 0;          
              USBSTORAGE_BUSY = 0;
            } else USBSTORAGE_BUSY = 1;
          }
        }
      }
    }
  }
#endif   
#endif
//================================================================================
/*
#if DEBUG>5
  if (Serial.available() > 0) {
    while(Serial.available() > 0) {uint8_t incomingByte = Serial.read();}
    if (BLUETOOTH_APP==0) {
      START_BT(); //
      BLUETOOTH_APP=1;
    } else if (BLUETOOTH_APP==2) {
      printf("================================================================================\n");
      printf("Switching to BLUETOOTH A2DP TASK\n");
      printf("================================================================================\n");
      gap_inquiry_stop(); //top scan devices
      BLUETOOTH_APP=3;
      delay(1000);
      MEMORY_STATUS();
    } else if (BLUETOOTH_APP==3) {
      printf("================================================================================\n");
      printf("Switching to BLUETOOTH HID TASK\n");
      printf("================================================================================\n");
      STREAM_PAUSED=1; ///stop stream when switching task
      BLUETOOTH_APP=2;
      gap_inquiry_start(INQUIRY_INTERVAL);
      delay(1000);
      MEMORY_STATUS();
    }       
  }    
#endif 
*/
//--------------------------------------------------------------------------------
}
