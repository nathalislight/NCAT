//                          ▄▀▀█▄                ▄██▀▀▄       
//                         ▐█░░ ▀██▄          ▄███▀░░░█       
//                         ▐█░░░░░▀█████████████▀▒░░░░█       
//                 ▄▄████████░░▒▒▒▒░██████████████░▒░░█       
//             ▄█████████████▄▒▒▒▒▒▄████████████████░▐        
//           ▄████████████████▄▄▄█████████  ▀████████▌        
//          ▐█████████▀▀▀▀▀█████████████▀    ▀███████▌        
//         ▐█▀███▀▀▄       ▐██████████▀        ▀█████▌        
//         ▓▌               ░████▀▀▀   ▀▄    ▄▀   ▀██         
//        ▄██              ▐ ▀▌                    ▄▀         
//       ▓███▒             ▐  ▀▓▄▒         ▄    ▒▄▀▀          
//       █████▒▒         ▒▒▌             ▀▀▒▀▀▀ ▀█            
//       ▓████████████████████▒▒         ▄ ▄▄ ▒▄▒▄█▄▄         
//     ▄▄███████████████████████████████████▀██████████▄▄     
//  ▄█▀▒▒▀▒▒▒▀█▓▀▒█▓▒▒▓█▀▒▒▀█▄▒▒▒▒█▄████▓▓▓███▒███▓▓████▓██   
// ▐█▒▒▒▒▒▒▒▀▒▄▒▀▒▄▄▒▒▀█▄▒▒▒▄█▒▓▄█████▓▓███████▓▓▒▒███▀▒▓▒▒██▄
//  ▀▀█▄▒▒▒▒▄▄▒▒▒▄▒▒█▄▒▒▒▒▀▒▒▒▒▒▄███▄██▀▒██▒██▒▓▓▓▓▓▓▓▓▒▄█▀▀  
//      ▀▀█▄▒▒██▒▒█████████▒▒▒▒██▀▄██▒▒▄▓▓▓▓▓▒▒▓▓▓▓▒▄█▀▀      
//          ▀▀████▓████▓████▒▒▒▒▒▒▒▄▒▀▄█▓██▒▒▓▓▒▄█▀▀          
//              ▀▀███████▀▀▀▒▒▄▄▀▀▒▄▓█▓▒▒▓█▒██▀▀              
//                  ▀███▀████▄▒▓▓█▌▒▒▓█▒██▀▀                  
//                      ▀██▒▒▒▒▓▓▓▓▒███▀                      
//                          ▀▀▀▀▀▀▀▀                          

//********************************************************************************
//*                                                                              *
//*   NCAT SYSTEM 7.0 by Nathalis                                                *
//*                                                                              *
//*   Programs: 1. NES EMULATOR,                                                 *
//*             2. GBC EMULATOR,                                                 *
//*             3. SMS EMULATOR,                                                 *
//*             4. MP3 player,                                                   *
//*             5. BLUETOOTH SPEAKER,                                            *
//*             6. FAKE86 EMULATOR,                                              *
//*             7. SETTINGS                                                      *
//*             8. Keyboard-Mouse SWITCH (extra app),                            *
//*             9. OSCILLOSCOPE (extra app),                                     *
//*            10. SIGNAL GENERATOR (extra app),                                 *
//*                                                                              *
//*   Hardware: 1. NCAT PRO version or                                           *
//*             2. NCAT LITE version or                                          *
//*             3. TTGO TWATCH 2020 V1 or                                        *
//*             3. TTGO TWATCH 2020 V2 (V2 with I2S AUDIO HACK) or               *
//*             4. RAW ESP32 WROVER (for test)                                   *
//*                                                                              *
//*   Requirements: ESP32 WROVER (E or B) CHIP, ST7789 SPI LCD,                  *
//*   MICROSD CARD slot, MAX I2S AUDIO IC (optional), MAX3421E USB HOST,         *
//*   (extra parts: 4017 IC (for BUTTONS), 74HC00 IC (for CLEAN PWM AUDIO),      *
//*   2x 3.3V LDO 1117 (recommending...), CHARGING IC (HT4928S or TP4056),       *
//*   OTHER PARTS...                                                             *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   Features:                                                                  *
//*   __________________________________________________________________________ *
//*   - ESP32-WROVER 8MB PSRAM module support                                    *
//*   - CP2102 USB to Serial IC                                                  *
//*   - MicroSD card support                                                     *
//*   - 240*240 1.3" (1.54") LCD ST7789 display                                  *
//*   - Composite TV OUT Video NTSC                                              *
//*   - I2S AUDIO support MAX98357 IC (optional)                                 *
//*   - PWM AUDIO support (optional)                                             *
//*   - USB HOST MAX3421E IC (optional): USB FLASH DRIVE & Keyboard support      *
//*     (also other USB DEVICES).                                                *
//*   - Extended BLUETOOTH support:                                              *
//*     BLUETOOTH Keyboard (experimental)                                        *
//*     PS4 DualShock BLUETOOTH Gamepad (experimental)                           *
//*   - DS3232MZ+ or DS1307 I2C Real Time Clock support                          *
//*   - Himem support 8MB PSRAM (GBC & Fake86 emulator implemented)              *
//*   - I2C docking (experimental for KM Switch)                                 *
//*   - Custom Wallpaper (must be placed in MicroSd card as wallpaper.jpg and    *
//*     size needs be 240x240 pixels)                                            *
//*   - InGame Menu with State Save and State load options                       *
//*   - Overclock support (experimental - please take care on ESP32 :D)          *
//*   - USB Bluetooth adapter (PS4 joystick supported - experimental)            *
//*   - Multiplayer suppor for NES                                               *
//*   - POWER SAVER (display off when inactivity)                                *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   Only for personal & educational use!                                       *
//*   [GPL-3.0 License]                                                          *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   Please take care on correct settings and connections of parts              *
//*   in schematics, it was tested many times but nothing guaranteed.            *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   Recommended Arduino core for the ESP32 IDF 1.0.6 or 1.0.5                  *
//*   Please do not use Arduino core for the ESP32 IFD 2.0.0                     *
//*   Please do not use Arduino core for the ESP32 IFD 1.0.4 or lower            *
//*                                                                              *
//*   LINK FOR ARDUINO IDE:                                                      *
//*   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   INCREASE PERFORMANCE STEP:                                                 *
//*                                                                              *
//*   Also You need disable fix PSRAM cache: in file (for example):              *
//*      C:\Users\ {MYUSERNAME} \AppData\Local\Arduino15\packages\esp32          *
//*      \hardware\esp32\1.0.5\boards.txt                                        *
//*                                                                              *
//*   at esp32.menu.PSRAM.enabled.build.defines=... REMOVE:                      *
//*   -mfix-esp32-psram-cache-issue                                              *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   When you change code, you need create new save state file because          *
//*   it contains memory pointers and they have been changed... so old state     *
//*   load cause crash...                                                        *
//*                                                                              *
//********************************************************************************
//*                                                                              *
//*   This is really huge code, so please have a patience to messy sections      *
//*   of code...                                                                 *
//*                                                                              *
//********************************************************************************

///#pragma GCC optimize("Ofast")
#pragma GCC optimize ("O3")

extern "C" {
  void btdm_controller_set_sleep_mode(uint8_t mode);
  uint8_t btdm_controller_get_sleep_mode(void);
  void btdm_controller_enable_sleep(bool enable);
}

// DO NOT FORGET SET 80MHz FLASH & SPIRAM for better performance.

//********************************************************************************
//********************************************************************************
//********************************************************************************

//██████████████████████████████████████████████████
//█  DEFINITIONS AND VARIABLES:
//██████████████████████████████████████████████████

// -> uncomment correct BOARD setup:

///#define NCAT_BOARD               //PRO VERSION of NCAT
///#define NCAT_LITE                //LITE VERSION of NCAT
///#define NCAT_WATCH               //not implemented yet...
#define NCAT_TWATCH_V1           //experimental...
///#define NCAT_TWATCH_V2           //experimental...
///#define HACKED_NCAT_TWATCH_V2    //experimental... (I2S audio added)
///#define NCAT_MINIMAL             //Composite and Bluetooth (No LCD, No Buttons)
///#define NCAT_VGA                 //VGA and Bluetooth and USB
///#define NCAT_MODULAR             //(NesCat 1.0 modular board)

// DEBUG LEVEL DEFINITION: 0...6
#define DEBUG 4


//--------------------------------------------------------------------------------
// FOR SD CARD CONFIG PLEASE SET CORRECT SPI DRIVER MODE "CONFIG.h" 
//--------------------------------------------------------------------------------


//================================================================================
// BOARD SETUP PROFILES:
//================================================================================

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

//--------------------------------------------------------------------------------
#ifdef NCAT_BOARD
#define SETTINGS_OPTIONS 1              // Enable EEPROM SETTINGS
#define JPG_DECODER 1                   // Enable JPG_DECODER
#define PNG_DECODER 1                   // Enable PNG_DECODER
#define USBHOST 1                       // USB HOST support: 0=disabled, 1=enabled, 2=required
#define USBHOST_STORAGE 1               // USB HOST Mass Storage features: 0=disabled, 1=enabled
#define USBHOST_HID 1                   // USB HOST HID features: 0=disabled, 1=enabled
#define USBHOST_JOYSTICKS 1             // USB HOST MULTIPLE JOYSTICK features: 0=disabled, 1=enabled
#define USBHOST_BLUETOOTH 0             // USB HOST Mass Storage features: 0=disabled, 1=enabled
#define LCD_ENABLED 1                   // Enable ST7789 LCD 240*240 
#define LCD_SPI_SPEED 80000000          // 1.54" ST7789 LCD can handle 80MHz SPI
#define LCD_BACKLIGHT_CONTROL 1         // Enable PWM control of Backlight
#define TOUCHPAD_ENABLED 0              // Enable I2C TOUCHPAD DEVICE
#define HIMEM 1                         // Enable 8MB PSRAM support (only by himem) if is present...
#define COMPOSITE_VIDEO 1               // Composite TV OUT on DAC I2S (NTSC mode)
#define VGA_OUTPUT 0                    // VGA 64 COLORS OUTPUT
#define BLUETOOTH 1                     // Enable Bluetooth (~64kB STATIC INTERNAL RAM)
#define SOUND_ENABLED 1                 // Enable SOUND
#define I2SAUDIO_ENABLED 1              // Enable I2S sound
#define PWMAUDIO_ENABLED 0              // Enable fallback PWM audio (when no I2S)
#define AUDIO_I2SPORT 1                 // I2S_NUM_0 or I2S_NUM_1 (I2S_NUM_0 is USED BY COMPOSITE VIDEO)
#define MICROSD 1                       // MicroSD support 0=disabled, 1=enabled, 2=required
#define FLASH_ROM 0                     // Extra ROMs in FLASH
#define GPIO_BUTTONS 1                  // 0=disabled, 1=IC 4017 Buttons, 2=Buttons Matrix
#define CLOCK 1                         // View CLOCK on main screen
#define LCD_WIDTH 240                   // LCD MAX WIDTH
#define LCD_HEIGHT 240                  // LCD MAX HEIGHT
#define SCREEN_WIDTH 256                // BUFFER MAX WIDTH
#define SCREEN_HEIGHT 256               // BUFFER MAX HEIGHT
#define BACKGROUND_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2) //256*256*2 = 131072bytes PSRAM 16bit color
#define JPG_BUFFER_SIZE 32768           // Size of buffer for JPG decoding (PSRAM)
#define AUTO_POWER_SAVER 1              // Allows Power Saving when inactive  
#define BOOST_BY_IRAM 0                 // takes more IRAM for better Performance
#define NES_CONTROLLER 0                 // Original NES CONTROLLER usage  
#define PS2_KEYBOARD 0                  // PS2 (USB) KEYBOARD   
#endif
//--------------------------------------------------------------------------------
#ifdef NCAT_LITE
#define SETTINGS_OPTIONS 1              // Enable EEPROM SETTINGS
#define JPG_DECODER 1                   // Enable JPG_DECODER
#define PNG_DECODER 1                   // Enable PNG_DECODER
#define USBHOST 0                       // USB HOST support: 0=disabled, 1=enabled, 2=required
#define USBHOST_HID 0                   // USB HOST HID features: 0=disabled, 1=enabled
#define LCD_ENABLED 1                   // Enable ST7789 LCD 240*240 
#define LCD_SPI_SPEED 40000000          // 1.54" ST7789 LCD can handle 80MHz SPI
#define LCD_BACKLIGHT_CONTROL 1         // Enable PWM control of Backlight  
#define TOUCHPAD_ENABLED 0              // Enable I2C TOUCHPAD DEVICE
#define HIMEM 1                         // Enable 8MB PSRAM support (only by himem) if is present...
#define COMPOSITE_VIDEO 1               // Composite TV OUT on DAC I2S (NTSC mode)
#define VGA_OUTPUT 0                    // VGA 64 COLORS OUTPUT
#define BLUETOOTH 1                     // Enable Bluetooth (~64kB STATIC INTERNAL RAM)
#define SOUND_ENABLED 1                 // Enable SOUND
#define I2SAUDIO_ENABLED 0              // Enable I2S sound
#define PWMAUDIO_ENABLED 1              // Enable fallback PWM audio (when no I2S)
#define AUDIO_I2SPORT 1                 // I2S_NUM_0 or I2S_NUM_1 (I2S_NUM_0 is USED BY COMPOSITE VIDEO)
#define MICROSD 2                       // MicroSD support 0=disabled, 1=enabled, 2=required
#define FLASH_ROM 0                     // Extra ROMs in FLASH
#define GPIO_BUTTONS 2                  // 0=disabled, 1=IC 4017 Buttons, 2=Buttons Matrix
#define CLOCK 1                         // View CLOCK on main screen
#define LCD_WIDTH 240                   // LCD MAX WIDTH
#define LCD_HEIGHT 240                  // LCD MAX HEIGHT
#define SCREEN_WIDTH 256                // BUFFER MAX WIDTH
#define SCREEN_HEIGHT 256               // BUFFER MAX HEIGHT
#define BACKGROUND_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2) //256*256*2 = 131072bytes PSRAM 16bit color
#define JPG_BUFFER_SIZE 32768           // Size of buffer for JPG decoding (PSRAM)
#define AUTO_POWER_SAVER 0              // Allows Power Saving when inactive
#define BOOST_BY_IRAM 0                 // takes more IRAM for better Performance   
#define NES_CONTROLLER 0                 // Original NES CONTROLLER usage  
#define PS2_KEYBOARD 0                  // PS2 (USB) KEYBOARD     
#endif
//--------------------------------------------------------------------------------
#ifdef NCAT_TWATCH_V1
#define SETTINGS_OPTIONS 1              // Enable EEPROM SETTINGS
#define JPG_DECODER 1                   // Enable JPG_DECODER
#define PNG_DECODER 1                   // Enable PNG_DECODER
#define USBHOST 0                       // USB HOST support: 0=disabled, 1=enabled, 2=required
#define USBHOST_HID 0                   // USB HOST HID features: 0=disabled, 1=enabled
#define LCD_ENABLED 1                   // Enable ST7789 LCD 240*240 
#define LCD_SPI_SPEED 80000000          // 1.54" ST7789 LCD can handle 80MHz SPI
#define LCD_BACKLIGHT_CONTROL 1         // Enable PWM control of Backlight  
#define TOUCHPAD_ENABLED 1              // Enable I2C TOUCHPAD DEVICE
#define HIMEM 1                         // Enable 8MB PSRAM support (only by himem) if is present...
#define COMPOSITE_VIDEO 0               // Composite TV OUT on DAC I2S (NTSC mode)
#define VGA_OUTPUT 0                    // VGA 64 COLORS OUTPUT
#define BLUETOOTH 1                     // Enable Bluetooth (~64kB STATIC INTERNAL RAM)
#define SOUND_ENABLED 1                 // Enable SOUND
#define I2SAUDIO_ENABLED 1              // Enable I2S sound
#define PWMAUDIO_ENABLED 0              // Enable fallback PWM audio (when no I2S)
#define AUDIO_I2SPORT 1                 // I2S_NUM_0 or I2S_NUM_1 (I2S_NUM_0 is USED BY COMPOSITE VIDEO)
#define MICROSD 0                       // MicroSD support 0=disabled, 1=enabled, 2=required
#define FLASH_ROM 1                     // Extra ROMs in FLASH
#define GPIO_BUTTONS 0                  // 0=disabled, 1=IC 4017 Buttons, 2=Buttons Matrix
#define CLOCK 1                         // View CLOCK on main screen
#define LCD_WIDTH 240                   // LCD MAX WIDTH
#define LCD_HEIGHT 240                  // LCD MAX HEIGHT
#define SCREEN_WIDTH 256                // BUFFER MAX WIDTH
#define SCREEN_HEIGHT 256               // BUFFER MAX HEIGHT
#define BACKGROUND_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2) //256*256*2 = 131072bytes PSRAM 16bit color
#define JPG_BUFFER_SIZE 32768           // Size of buffer for JPG decoding (PSRAM)
#define AUTO_POWER_SAVER 1              // Allows Power Saving when inactive  
#define BOOST_BY_IRAM 0                 // takes more IRAM for better Performance  
#define NES_CONTROLLER 0                 // Original NES CONTROLLER usage    
#define PS2_KEYBOARD 0                  // PS2 (USB) KEYBOARD  
#endif
//--------------------------------------------------------------------------------
#ifdef HACKED_NCAT_TWATCH_V2
#define SETTINGS_OPTIONS 1              // Enable EEPROM SETTINGS
#define JPG_DECODER 1                   // Enable JPG_DECODER
#define PNG_DECODER 1                   // Enable PNG_DECODER
#define USBHOST 0                       // USB HOST support: 0=disabled, 1=enabled, 2=required
#define USBHOST_HID 0                   // USB HOST HID features: 0=disabled, 1=enabled
#define LCD_ENABLED 1                   // Enable ST7789 LCD 240*240 
#define LCD_SPI_SPEED 80000000          // 1.54" ST7789 LCD can handle 80MHz SPI
#define LCD_BACKLIGHT_CONTROL 1         // Enable PWM control of Backlight  
#define TOUCHPAD_ENABLED 1              // Enable I2C TOUCHPAD DEVICE
#define HIMEM 1                         // Enable 8MB PSRAM support (only by himem) if is present...
#define COMPOSITE_VIDEO 0               // Composite TV OUT on DAC I2S (NTSC mode)
#define BLUETOOTH 1                     // Enable Bluetooth (~64kB STATIC INTERNAL RAM)
#define SOUND_ENABLED 1                 // Enable SOUND
#define I2SAUDIO_ENABLED 1              // Enable I2S sound
#define PWMAUDIO_ENABLED 0              // Enable fallback PWM audio (when no I2S)
#define AUDIO_I2SPORT 1                 // I2S_NUM_0 or I2S_NUM_1 (I2S_NUM_0 is USED BY COMPOSITE VIDEO)
#define MICROSD 1                       // MicroSD support 0=disabled, 1=enabled, 2=required
#define FLASH_ROM 1                     // Extra ROMs in FLASH
#define GPIO_BUTTONS 0                  // 0=disabled, 1=IC 4017 Buttons, 2=Buttons Matrix
#define CLOCK 1                         // View CLOCK on main screen
#define LCD_WIDTH 240                   // LCD MAX WIDTH
#define LCD_HEIGHT 240                  // LCD MAX HEIGHT
#define SCREEN_WIDTH 256                // BUFFER MAX WIDTH
#define SCREEN_HEIGHT 256               // BUFFER MAX HEIGHT
#define BACKGROUND_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2) //256*256*2 = 131072bytes PSRAM 16bit color
#define JPG_BUFFER_SIZE 32768           // Size of buffer for JPG decoding (PSRAM)
#define AUTO_POWER_SAVER 1              // Allows Power Saving when inactive  
#define BOOST_BY_IRAM 0                 // takes more IRAM for better Performance  
#define NES_CONTROLLER 0                 // Original NES CONTROLLER usage 
#define PS2_KEYBOARD 0                  // PS2 (USB) KEYBOARD     
#endif
//--------------------------------------------------------------------------------
#ifdef NCAT_MINIMAL
#define SETTINGS_OPTIONS 1              // Enable EEPROM SETTINGS
#define JPG_DECODER 1                   // Enable JPG_DECODER
#define PNG_DECODER 1                   // Enable PNG_DECODER
#define USBHOST 0                       // USB HOST support: 0=disabled, 1=enabled, 2=required
#define USBHOST_HID 0                   // USB HOST HID features: 0=disabled, 1=enabled
#define LCD_ENABLED 0                   // Enable ST7789 LCD 240*240 
#define LCD_SPI_SPEED 40000000          // 1.54" ST7789 LCD can handle 80MHz SPI
#define LCD_BACKLIGHT_CONTROL 0         // Enable PWM control of Backlight  
#define TOUCHPAD_ENABLED 0              // Enable I2C TOUCHPAD DEVICE
#define HIMEM 1                         // Enable 8MB PSRAM support (only by himem) if is present...
#define COMPOSITE_VIDEO 1               // Composite TV OUT on DAC I2S (NTSC mode)
#define VGA_OUTPUT 0                    // VGA 64 COLORS OUTPUT
#define BLUETOOTH 1                     // Enable Bluetooth (~64kB STATIC INTERNAL RAM)
#define SOUND_ENABLED 1                 // Enable SOUND
#define I2SAUDIO_ENABLED 0              // Enable I2S sound
#define PWMAUDIO_ENABLED 1              // Enable fallback PWM audio (when no I2S)
#define AUDIO_I2SPORT 1                 // I2S_NUM_0 or I2S_NUM_1 (I2S_NUM_0 is USED BY COMPOSITE VIDEO)
#define MICROSD 1                       // MicroSD support 0=disabled, 1=enabled, 2=required
#define FLASH_ROM 0                     // Extra ROMs in FLASH
#define GPIO_BUTTONS 0                  // 0=disabled, 1=IC 4017 Buttons, 2=Buttons Matrix
#define CLOCK 0                         // View CLOCK on main screen
#define LCD_WIDTH 240                   // LCD MAX WIDTH
#define LCD_HEIGHT 240                  // LCD MAX HEIGHT
#define SCREEN_WIDTH 256                // BUFFER MAX WIDTH
#define SCREEN_HEIGHT 256               // BUFFER MAX HEIGHT
#define BACKGROUND_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2) //256*256*2 = 131072bytes PSRAM 16bit color
#define JPG_BUFFER_SIZE 32768           // Size of buffer for JPG decoding (PSRAM)
#define AUTO_POWER_SAVER 1              // Allows Power Saving when inactive  
#define BOOST_BY_IRAM 1                 // takes more IRAM for better Performance   
#define NES_CONTROLLER 0                // Original NES CONTROLLER usage
#define PS2_KEYBOARD 0                  // PS2 (USB) KEYBOARD  
#endif
//--------------------------------------------------------------------------------
#ifdef NCAT_VGA
#define SETTINGS_OPTIONS 1              // Enable EEPROM SETTINGS
#define JPG_DECODER 1                   // Enable JPG_DECODER
#define PNG_DECODER 1                   // Enable PNG_DECODER
#define USBHOST 1                       // USB HOST support: 0=disabled, 1=enabled, 2=required
#define USBHOST_STORAGE 1               // USB HOST Mass Storage features: 0=disabled, 1=enabled
#define USBHOST_HID 1                   // USB HOST HID features: 0=disabled, 1=enabled
#define USBHOST_JOYSTICKS 1             // USB HOST MULTIPLE JOYSTICK features: 0=disabled, 1=enabled
#define USBHOST_BLUETOOTH 0               // USB HOST Mass Storage features: 0=disabled, 1=enabled
#define LCD_ENABLED 0                   // Enable ST7789 LCD 240*240 
#define LCD_SPI_SPEED 40000000          // 1.54" ST7789 LCD can handle 80MHz SPI
#define LCD_BACKLIGHT_CONTROL 0         // Enable PWM control of Backlight  
#define TOUCHPAD_ENABLED 0              // Enable I2C TOUCHPAD DEVICE
#define HIMEM 1                         // Enable 8MB PSRAM support (only by himem) if is present...
#define COMPOSITE_VIDEO 0               // Composite TV OUT on DAC I2S (NTSC mode)
#define VGA_OUTPUT 1                   // VGA 64 COLORS OUTPUT
#define BLUETOOTH 1                     // Enable Bluetooth (~64kB STATIC INTERNAL RAM)
#define SOUND_ENABLED 0                 // Enable SOUND
#define I2SAUDIO_ENABLED 0              // Enable I2S sound
#define PWMAUDIO_ENABLED 0              // Enable fallback PWM audio (when no I2S)
#define AUDIO_I2SPORT 0                 // I2S_NUM_0 or I2S_NUM_1 (I2S_NUM_0 is USED BY COMPOSITE VIDEO)
#define MICROSD 1                       // MicroSD support 0=disabled, 1=enabled, 2=required
#define FLASH_ROM 0                     // Extra ROMs in FLASH
#define GPIO_BUTTONS 0                  // 0=disabled, 1=IC 4017 Buttons, 2=Buttons Matrix
#define CLOCK 0                         // View CLOCK on main screen
#define LCD_WIDTH 240                   // LCD MAX WIDTH
#define LCD_HEIGHT 240                  // LCD MAX HEIGHT:    200 OR 240 
#define SCREEN_WIDTH 320                // BUFFER MAX WIDTH
#define SCREEN_HEIGHT 240               // BUFFER MAX HEIGHT
#define BACKGROUND_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2) //256*256*2 = 131072bytes PSRAM 16bit color
#define JPG_BUFFER_SIZE 32768           // Size of buffer for JPG decoding (PSRAM)
#define AUTO_POWER_SAVER 1              // Allows Power Saving when inactive  
#define BOOST_BY_IRAM 0                 // takes more IRAM for better Performance
#define NES_CONTROLLER 0                // Original NES CONTROLLER usage
#define PS2_KEYBOARD 0                  // PS2 (USB) KEYBOARD   
#endif
//--------------------------------------------------------------------------------
#ifdef NCAT_MODULAR
#define NCAT_NO_PSRAM

#define SETTINGS_OPTIONS 1              // Enable EEPROM SETTINGS

#ifdef NCAT_NO_PSRAM
  #define JPG_DECODER 0                   // Enable JPG_DECODER
  #define PNG_DECODER 0                   // Enable PNG_DECODER
  #define BLUETOOTH 0                     // Enable Bluetooth (~64kB STATIC INTERNAL RAM)
#else
  #define JPG_DECODER 1                   // Enable JPG_DECODER
  #define PNG_DECODER 1                   // Enable PNG_DECODER
  #define BLUETOOTH 1                     // Enable Bluetooth (~64kB STATIC INTERNAL RAM)
#endif

#define USBHOST 0                       // USB HOST support: 0=disabled, 1=enabled, 2=required
#define USBHOST_HID 0                   // USB HOST HID features: 0=disabled, 1=enabled
#define LCD_ENABLED 1                   // Enable ST7789 LCD 240*240 
#define LCD_SPI_SPEED 40000000          // 1.3" ST7789 LCD can handle 40MHz SPI
#define LCD_BACKLIGHT_CONTROL 0         // Enable PWM control of Backlight  
#define TOUCHPAD_ENABLED 0              // Enable I2C TOUCHPAD DEVICE
#define HIMEM 0                         // Enable 8MB PSRAM support (only by himem) if is present...
#define COMPOSITE_VIDEO 1               // Composite TV OUT on DAC I2S (NTSC mode)
#define VGA_OUTPUT 0                    // VGA 64 COLORS OUTPUT
#define SOUND_ENABLED 1                 // Enable SOUND
#define I2SAUDIO_ENABLED 1              // Enable I2S sound
#define PWMAUDIO_ENABLED 0              // Enable fallback PWM audio (when no I2S)
#define AUDIO_I2SPORT 1                 // I2S_NUM_0 or I2S_NUM_1 (I2S_NUM_0 is USED BY COMPOSITE VIDEO)
#define MICROSD 1                       // MicroSD support 0=disabled, 1=enabled, 2=required
#define FLASH_ROM 0                     // Extra ROMs in FLASH
#define GPIO_BUTTONS 3                  // 0=disabled, 1=IC 4017 Buttons, 2=Buttons Matrix
#define CLOCK 1                         // View CLOCK on main screen
#define LCD_WIDTH 240                   // LCD MAX WIDTH
#define LCD_HEIGHT 240                  // LCD MAX HEIGHT:    200 OR 240 
#define SCREEN_WIDTH 256                // BUFFER MAX WIDTH
#define SCREEN_HEIGHT 256               // BUFFER MAX HEIGHT
#define BACKGROUND_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2) //256*256*2 = 131072bytes PSRAM 16bit color
#define JPG_BUFFER_SIZE 32768           // Size of buffer for JPG decoding (PSRAM)
#define AUTO_POWER_SAVER 0              // Allows Power Saving when inactive  
#define BOOST_BY_IRAM 0                 // takes more IRAM for better Performance
#define NES_CONTROLLER 0                // Original NES CONTROLLER usage
#define PS2_KEYBOARD 0                  // PS2 (USB) KEYBOARD   
#endif
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#define OVERCLOCK_ALLOWED 1               // Allows OVERCLOCKING ESP32

#ifndef NCAT_NO_PSRAM
  #define NES_EMULATOR 1                    // (5kB STATIC INTERNAL RAM)
  #define GBC_EMULATOR 1                    // (2kB STATIC INTERNAL RAM)
  #define SMS_EMULATOR 1
  #define AUDIO_PLAYER 1
  #define BT_SPEAKER 1
  #define FAKE86_EMULATOR 1
  #define SETTINGS_OPTION 1
  #define KMSWITCH 1
  #define OSCILLOSCOPE 1
  #define SIGNAL_GENERATOR 1
#else 
  #define NES_EMULATOR 1                    // (5kB STATIC INTERNAL RAM)
  #define GBC_EMULATOR 1                    // (2kB STATIC INTERNAL RAM)
  #define SMS_EMULATOR 1
  #define AUDIO_PLAYER 1
  #define BT_SPEAKER 1
  #define FAKE86_EMULATOR 0
  #define SETTINGS_OPTION 1
  #define KMSWITCH 0
  #define OSCILLOSCOPE 0
  #define SIGNAL_GENERATOR 0
#endif

#define USB_UNIVERSAL_HID 1               // better compatibility of USB HID devices if is set

#define EEPROM_RESET_SETTINGS 0           // if set 1= EEPROM will be force RESETED
#define BT_RAW_PARSER 1                   // 0=generic (slower, but more devices supported) 1=raw (faster, less compatibility)
#define KEEP_WALLPAPER_IN_PSRAM 1         // 0=not stored, 1=stored in PSRAM (warning do not use huge files!)

#define PSRAM_SPEED PSRAM_CACHE_F80M_S80M // 40..80MHz SPI speed of FLASH or PSRAM
//FLASH & SPIRAM SPEEDS:
//PSRAM_CACHE_F40M_S40M
//PSRAM_CACHE_F80M_S40M
//PSRAM_CACHE_F80M_S80M

//--------------------------------------------------------------------------------
#ifdef NCAT_NO_PSRAM
//SPI FLASH MEMORY ACCESS:
#include "esp_spi_flash.h"

#define MAX_FLASH_FILESIZE (1024*1024) //max 1MB files

#define SPI_FLASH_ADDRESS 0x00280000 //2.5MB address test (CAN LOAD UP TO 1MB data)
//#define SPI_FLASH_ADDRESS 0x00300000 //0x00300000 working (empty flash area)
#define SPI_FLASH_SECTOR_SIZE 0x1000 //4kB = better not change

uint32_t FILE_ROM_SIZE = 0;
uint32_t FLASH_ROM_SIZE = 0;

//constant data pointer for direct access
const void *ROM;

spi_flash_mmap_handle_t handle1;

uint8_t flashdata[4096] = {0}; //4kB buffer
#endif
//--------------------------------------------------------------------------------

//================================================================================
// BLUETOOTH
//================================================================================
#define BLUTOOTH_POWER_MIN ESP_PWR_LVL_P3
#define BLUTOOTH_POWER_MAX ESP_PWR_LVL_P9
// BLUETOOTH POWER LEVELS
//[ 0 ] = ESP_PWR_LVL_N12 = Corresponding to -12dbm
//[ 1 ] = ESP_PWR_LVL_N9  = Corresponding to -9dbm
//[ 2 ] = ESP_PWR_LVL_N6  = Corresponding to -6dbm
//[ 3 ] = ESP_PWR_LVL_N3  = Corresponding to -3dbm
//[ 4 ] = ESP_PWR_LVL_N0  = Corresponding to 0dbm
//[ 5 ] = ESP_PWR_LVL_P3  = Corresponding to +3dbm
//[ 6 ] = ESP_PWR_LVL_P6  = Corresponding to +6dbm
//[ 7 ] = ESP_PWR_LVL_P9  = Corresponding to +9dbm
//----------------------------------------
#define BLUETOOTH_DELETE_ALL_LINK_KEYS 0  //when is enabled then BLUETOOH WILL DELETE ALL LINKED KEYS...
#define BLUETOOTH_INQUIRY_SECONDS 60      // BLUETOOTH INQUIRY SECONDS SHUTDOWN      
#define BLUETOOTH_SHUTDOWN_SECONDS 80     // BLUETOOTH SHUTDOWN SECONDS if nothing connected       
#define BLUETOOTH_STOP_SCAN_AFTER_CONNECTED 0

int8_t BLUETOOTH_APP = -1;                // -1=not inited, 0=bluetooth off, 1=bluetooth scanning, 2=bluetooth HID, 3=bluetooth A2DP, 4=EXTERNAL USB BLUETOOTH ADAPTER FOUND
#if BLUETOOTH
int8_t BLUETOOTH_CONNECTIONS = 0;
#else
int8_t BLUETOOTH_CONNECTIONS = -1;      // -1=not inited, 0=no connections, 1..3=connections
#endif

//================================================================================
// I2C setup
//================================================================================
// touch device address:
#define TOUCHPAD_I2C_ADDRESS 0x38
// i2c slave address of the DS3231 chip:
#define DS3231_I2C_ADDR      0x68
// i2c slave address of the DS1307 chip:
#define DS1307_CTRL_ID       0x68 

// i2c slave address for KM SWITCH SETUP:
#define ADDRESS_SUB_ARDUINO1 0x0A
#define ADDRESS_SUB_ARDUINO2 0x0B
#define ADDRESS_SUB_ARDUINO3 0x0C
#define ADDRESS_SUB_ARDUINO4 0x0D
#define ADDRESS_SUB_ARDUINO5 0x0E
#define ADDRESS_SUB_ARDUINO6 0x0F

//================================================================================
// SYSTEM VARIABLES
//================================================================================

//--------------------------------------------------------------------------------
uint8_t MULTITASK_MODE = 0;               // 0=no multitasking, 1=multitasking (DRAW & AUDIO functions)
//--------------------------------------------------------------------------------
uint32_t PSRAM_BASE_SIZE = 0;             // PSRAM BASE SIZE in bytes
uint32_t PSRAM_HIMEM_SIZE = 0;            // PSRAM HIMEM SIZE in bytes
uint8_t* HIMEM_READBUFFER;                // BUFFER for HIMEM must be in Internal RAM (not PSRAM)
//--------------------------------------------------------------------------------
uint8_t* SCREENMEMORY;                    // 64kB (SCREEN_WIDTH=256)*(SCREEN_HEIGHT=256) bytes //do not change this!!!
uint8_t* SCREENMEMORY_LINE[SCREEN_HEIGHT]; // SCREEN_HEIGHT = 256 lines mapped to SCREENMEMORY.
uint16_t SCREENBUFFER[1024] = {0};        // 4kB ...
//--------------------------------------------------------------------------------
uint32_t adj_32bit[256] = {0};            // COMPOSITE PALETTE 256 colors = 1kB
//--------------------------------------------------------------------------------
uint16_t DRAWMODE = 0;
//--------------------------------------------------------------------------------
uint8_t* DATA;                            // DATA POINTER for APPLICATIONS
uint32_t DATA_SIZE = 0;                   // SIZE OF ALLOCATED DATA
//--------------------------------------------------------------------------------
char* MAINPATH;                           // ps_malloc 256bytes
char* TEXTBUFFER;                         // ps_malloc 256bytes
char* DATAPATH1;                          // ps_malloc 256bytes
char* DATAPATH2;                          // ps_malloc 256bytes
char* TRACKNAME;                          // ps_malloc 256bytes
//--------------------------------------------------------------------------------
#ifndef NCAT_NO_PSRAM
//--------------------------------------------------------------------------------
  uint8_t* JPGOUTPUT_BUFFER;              // BUFFER for JPG is static allocated (PSRAM)
//--------------------------------------------------------------------------------
  uint8_t* PNGOUTPUT_BUFFER;              // BUFFER for PNG is dynamic allocated (PSRAM)
//--------------------------------------------------------------------------------
  uint8_t* WALLPAPER_FILE;                // BUFFER for WALLPAPER FILE (PSRAM stored wallpaper image)
//--------------------------------------------------------------------------------
  uint16_t* BACKGROUND_BUFFER;            // BUFFER for WALLPAPER DATA (~128kB of MEMORY)
//--------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------
uint8_t VOLUME = 64;                      // Audio Volume 1..255, 0=not detected (OFF)
//--------------------------------------------------------------------------------
uint8_t SDCARD_BUSY = 0;                  // 0=SDCARD inactive, 1=SDCARD is BUSY
uint8_t USBSTORAGE_BUSY = 0;              // USB STORAGE: 0=not inited, 1=inited, 2=BUSY
//--------------------------------------------------------------------------------
int8_t MENU_POS = -1;                     // MENU POSITION: -1=inactive, 0..127 = active positions
int8_t MENU_ACTIVE = -2;                  // MENU REQUEST: -1=inactive, 0=activate request, 1=active
int8_t MENU_ITEM = 0;                     // SELECTED MENU ITEM
//--------------------------------------------------------------------------------
uint8_t BATTERY = 1;                      // 0..255 or 0=not detected
uint8_t BATTERY_METER_ACTIVE = 1;         // 0=inactive, 1=active (do not check battery level in APPLICATIONS)
//--------------------------------------------------------------------------------
int8_t USB_HOST_devices = -2;             // -2=OFF, -1=USB ENABLED, 0=USB READY, 1=USB connected
uint32_t USB_HOST_devices_timeout = 0;
//--------------------------------------------------------------------------------
uint32_t SCROLLLOCK_millis = 0;
uint8_t SCROLLLOCK = 0;                   // 0=no device active, 1..6=number of active KMSWITCH HID Device
uint8_t KM_max_devices = 6;               // maximum of HID devices 0..6
//--------------------------------------------------------------------------------
uint8_t DRAWREADY = 0;                    // DRAW: 0=not ready, 1=ready, 2=busy
uint8_t AUDIOREADY = 0;                   // AUDIO: 0=not ready, 1=ready, 2=busy
//--------------------------------------------------------------------------------
SemaphoreHandle_t draw_mutex;             // Semaphore handle variable for DRAW
SemaphoreHandle_t audio_mutex;            // Semaphore handle variable for AUDIO
//--------------------------------------------------------------------------------
//================================================================================
// SOFTWARE SETTINGS
//================================================================================
//0 = NOT SET, 1=DISABLED, 2=ENABLED, 10..19 =NUMBER (10=0 .. 19=9)
uint8_t RESET_SETTINGS = 0;
uint8_t Clock_Enabled = 0;
uint8_t Sound_Enabled = 1;
uint8_t Force_Bluetooth = 1;
uint8_t Composite_Enabled = 2;
uint8_t Wallpaper = 10;
uint8_t MicroSD_Required = 2;
uint8_t USB_Host_Required = 1;
uint8_t Clock_Color = 10;
uint8_t InGame_Menu = 2;
uint8_t Overclock_level = 10;
uint8_t LCD_rotation = 10;
uint8_t Extra_apps = 1;
uint8_t Bluetooth_default_security_level = 10; 
uint8_t Multiplayer_dev_input_support = 2; 
uint8_t USBhost_joysticks = 2;
uint8_t USBhost_keyboard_and_mouse = 2;
uint8_t USBhost_mass_storage = 2;
uint8_t USBhost_bluetooth_PS4joysticks = 1;
uint8_t LCD_off_time = 0;


#if SETTINGS_OPTIONS
#include "src/EEPROM/EEPROM.h"
#define EEPROM_SIZE 64                  // 64bytes of EEPROM used
#endif
//--------------------------------------------------------------------------------
//================================================================================
// APP
//================================================================================
uint16_t POWER = 0;
//--------------------------------------------------------------------------------
#define APP_EMU_NONE 0                   // define number for no app 
#define APP_EMU_CLOCK 1                  // define number for CLOCK
#define APP_EMU_NES 2                    // define number for NES emulator
#define APP_EMU_GBC 4                    // define number for GBC emulator
#define APP_EMU_SMS 8                    // define number for SMS emulator
#define APP_AUD_PLAYER 16                // define number for audio player
#define APP_AUD_BTSPKER 32               // define number for bluetooth speaker
#define APP_EMU_FAKE86 64                // define number for app FAKE86
#define APP_SETTINGS 128                 // define number for settings
//--------------------------------------------------------------------------------
#define APP_KMSWITCH 256                 // define number for extra app Keyboard-Mouse Switch
#define APP_OSCILLOSCOPE 512             // define number for extra app Oscilloscope
#define APP_SIGNALGEN 1024               // define number for extra app Signal Generator
//--------------------------------------------------------------------------------

//██████████████████████████████████████████████████
//█  OVERCLOCK:
//██████████████████████████████████████████████████
// WHEN YOU OVERCLOCK (BLUETOOH WILL NOT WORK!)

#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/dport_reg.h"
#include "rom/ets_sys.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/apb_ctrl_reg.h"
#include "sdkconfig.h"

extern "C" {
  // ROM functions which read/write internal control bus
  uint8_t rom_i2c_readReg(uint8_t block, uint8_t host_id, uint8_t reg_add);
  uint8_t rom_i2c_readReg_Mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);
  void rom_i2c_writeReg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
  void rom_i2c_writeReg_Mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);
}

//********************************************************************************

uint8_t OVERCLOCK = 0;

//0 = 1.000x of speed = no overclock
//1 = 1.290x of speed
//2 = 1.414x of speed
//3 = 1.477x of speed
//4 = 1.540x of speed [UNSTABLE]
//5 = 1.666x of speed [VERY UNSTABLE] (disabled)

//********************************************************************************

#define I2C_BBPLL 0x66
#define I2C_BBPLL_ENDIV5 11
#define I2C_BBPLL_BBADC_DSMP 9
#define I2C_BBPLL_HOSTID 4
#define I2C_BBPLL_OC_LREF 2
#define I2C_BBPLL_OC_DIV_7_0 3
#define I2C_BBPLL_OC_DCUR 5

#define BBPLL_ENDIV5_VAL_320M 0x43
#define BBPLL_BBADC_DSMP_VAL_320M 0x84
#define BBPLL_ENDIV5_VAL_480M 0xc3
#define BBPLL_BBADC_DSMP_VAL_480M 0x74

#define DIG_DBIAS_240M RTC_CNTL_DBIAS_1V25
#define DELAY_PLL_DBIAS_RAISE 3

#define I2C_WRITEREG_MASK_RTC(block, reg_add, indata) rom_i2c_writeReg_Mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB,  indata)
#define I2C_READREG_MASK_RTC(block, reg_add) rom_i2c_readReg_Mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB)
#define I2C_WRITEREG_RTC(block, reg_add, indata) rom_i2c_writeReg(block, block##_HOSTID,  reg_add, indata)
#define I2C_READREG_RTC(block, reg_add) rom_i2c_readReg(block, block##_HOSTID,  reg_add)

////--------------------------------------------------------------------------------

/*void UNDERVOLT_ME() {
  REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DBIAS_1V20);
  ets_delay_us(DELAY_PLL_DBIAS_RAISE);
  }*/

void OVERCLOCK_ME(uint8_t OC_LEVEL) {
#if OVERCLOCK_ALLOWED
  
  I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_ENDIV5, BBPLL_ENDIV5_VAL_480M);
  I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_BBADC_DSMP, BBPLL_BBADC_DSMP_VAL_480M);

  REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, DIG_DBIAS_240M);
  //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, 8);
  ets_delay_us(DELAY_PLL_DBIAS_RAISE);

  uint8_t div_ref;
  uint8_t div7_0;
  uint8_t div10_8;
  uint8_t lref;
  uint8_t dcur;
  uint8_t bw;

  div_ref = 0;

  if (OC_LEVEL == 1) {
    div7_0 = 40; // ~SQRT(5/3)x speed (~310MHz?)
  } else if (OC_LEVEL == 2) {
    div7_0 = 48; // ~1.41421356x speed (~340MHz?)
  } else if (OC_LEVEL == 3) {
    div7_0 = 52; // ~1.47732683x speed (~355MHz?)
  } else if (OC_LEVEL == 4) {
    div7_0 = 56; // ~1.54044011x speed (~370MHz?) UNSTABLE
  } else if (OC_LEVEL == 5) {
    div7_0 = 64; // ~1.66666667x speed (~400MHz?) VERY UNSTABLE
  } else {
    div7_0 = 32; // 1x speed (240MHz)
  }
  div10_8 = 0;
  lref = 0;
  dcur = 6;
  bw = 3;

  uint8_t i2c_bbpll_lref = (lref << 7) | (div10_8 << 4) | (div_ref);
  uint8_t i2c_bbpll_div_7_0 = div7_0;
  uint8_t i2c_bbpll_dcur = (bw << 6) | dcur;
  I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_LREF, i2c_bbpll_lref);
  I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_DIV_7_0, i2c_bbpll_div_7_0);
  I2C_WRITEREG_RTC(I2C_BBPLL, I2C_BBPLL_OC_DCUR, i2c_bbpll_dcur);
#endif  
}
//--------------------------------------------------------------------------------

float TIMING = 1.000;

//--------------------------------------------------------------------------------
//overclock adjusted timings
uint64_t millis___() {
  return ((uint64_t)(millis() / TIMING));
}
uint64_t micros___() {
  return ((uint64_t)(micros() / TIMING));
}
void delay___(uint32_t DELAY) {
  delay((uint32_t)(DELAY * TIMING));
}
void delayMicroseconds___(uint32_t DELAY) {
  delayMicroseconds((uint32_t)(DELAY * TIMING));
}
void vTaskDelay___(uint32_t DELAY) {
  vTaskDelay((uint32_t)(DELAY * TIMING));
}
uint64_t xTaskGetTickCount___() {
  return ((uint64_t)(xTaskGetTickCount() / TIMING));
}
uint32_t TIMEMS___(uint32_t TIME) {
  return ((uint32_t)(TIME * TIMING));
}
uint32_t HERTZ___(uint32_t HERTZ) {
  return ((uint32_t)(HERTZ / TIMING));
}
uint32_t FREQ___(uint32_t HERTZ) {
  return ((uint32_t)(HERTZ * TIMING));
}
/*uint32_t HERTZ___(uint32_t HERTZ) {
  return ((uint32_t)(HERTZ / TIMING));
  }*/


//--------------------------------------------------------------------------------

//██████████████████████████████████████████████████
//█  HARDWARE CONFIGURATION & LIBRARIES:
//██████████████████████████████████████████████████

//--------------------------------------------------------------------------------
// KEY BUTTONS PINS: (CD4017 keypad)
//--------------------------------------------------------------------------------
#if(GPIO_BUTTONS == 1)
#define DIGITAL_BUTTON_IN 34           // ---[ 10K ]---| GND (PullDown)
#define DIGITAL_BUTTON_CALIBRATION 35  // ---[ 10K ]---| GND (PullDown)
#define DIGITAL_BUTTON_SWITCH 32       //
#define DIGITAL_BUTTON_DELAY_us 10
#endif
//--------------------------------------------------------------------------------
// KEY BUTTONS PINS: (Buttons Matrix)
//--------------------------------------------------------------------------------
#if(GPIO_BUTTONS == 2)
#define PIN_UP_SELECT 32               
#define PIN_DOWN_A 33                  
#define PIN_LEFT_B 4                  
#define PIN_RIGHT_START 5             
#define PIN_BUS1 34                    // ---[ 330R ]---GND
#define PIN_BUS2 35                    // ---[ 330R ]---GND
#define DIGITAL_BUTTON_DELAY_us 10
#endif
//--------------------------------------------------------------------------------
// KEY BUTTONS PINS: (Direct Buttons Pulleddown)
//--------------------------------------------------------------------------------
#if(GPIO_BUTTONS == 3)
//KEY BUTTONS PINS:
#define PIN_UP     39  //SVN
#define PIN_DOWN   35  //IO35
#define PIN_LEFT   36  //SVP
#define PIN_RIGHT  12  //TDI => Do not install 330R resistor!!!
#define PIN_A      2   //IO2
#define PIN_B      14  //TMS
#define PIN_START  15  //TDO
#define PIN_SELECT 13  //TCK
#endif
//--------------------------------------------------------------------------------
// SERVICE PIN: (LCD BACKLIGHT & BATTERY METER)
//--------------------------------------------------------------------------------
#if(GPIO_BUTTONS == 1)
#define BATTERY_METTER_PIN 39          // + BATTERY METER ---|<|---
#endif
#if(GPIO_BUTTONS == 2)
#define BATTERY_METTER_PIN 39           // + BATTERY METER ---|<|---
#endif
//--------------------------------------------------------------------------------
#if defined(NCAT_BOARD) || defined(NCAT_LITE)
#define LCD_BACKLIGHT_PIN 2 //IS HARDWARE PULLED UP
#endif
#ifdef NCAT_TWATCH_V1
#define LCD_BACKLIGHT_PIN 12
#endif
#if defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
#define LCD_BACKLIGHT_PIN 12
#endif
//--------------------------------------------------------------------------------
// COMPOSITE_VIDEO:
//--------------------------------------------------------------------------------
uint16_t * SIGWAVE; //for SIGNAL GENERATION

#if COMPOSITE_VIDEO
#ifndef DAC_GPIO25_CHANNEL
#define DAC_GPIO25_CHANNEL DAC_CHANNEL_1
#endif
#ifndef DAC_GPIO26_CHANNEL
#define DAC_GPIO26_CHANNEL DAC_CHANNEL_2
#endif

#include "soc/rtc.h"                    //for (rtc_clk_apll_enable)
#include "driver/dac.h"                 //for DAC (DAC_GPIO26_CHANNEL)
#include "driver/periph_ctrl.h"         //for (periph_module_enable)
#include "soc/i2s_struct.h"             //for (dac_output_enable)
#include "rom/lldesc.h"                 //for (lldesc_t)

#define COMPOSITE_VIDEO_BYTE_WIDTH 3    // 3 or 4 byte (PALETTE: 24bit ... 32bit)
#define COMPOSITE_VIDEO_ISR_CORE 1
#define COMPOSITE_VIDEO_COLORS 256      // now we have 256 colors
#define VIDEO_OUT (DAC_GPIO26_CHANNEL)  //DAC_GPIO25_CHANNEL or DAC_GPIO26_CHANNEL    

//COMPOSITE_VIDEO_OUT library
#include "src/compositevideo/video.h"

#endif
//--------------------------------------------------------------------------------
// VGA OUTPUT:
//--------------------------------------------------------------------------------
#if VGA_OUTPUT
bool lowBitOnly=false; 

#include "VGA/vgabasecontroller.h"
#include "VGA/vgacontroller.h"

#include "VGA/vgabasecontroller.cpp"
#include "VGA/vgacontroller.cpp"
#include "VGA/swgenerator.cpp"

fabgl::VGAController VGA;

uint8_t VGA_READY=0;
uint8_t VGA_SCANLINE_BUF[480];
uint8_t VGA_INITED=0;

uint16_t VGA_OFFSET=(320 - 240) / 2 - 4;
#endif


//--------------------------------------------------------------------------------
// PS/2 KEYBOARD CONTROLLER
//--------------------------------------------------------------------------------
#if PS2_KEYBOARD

///!!! do not forget 1KOHM resistors

#ifdef NCAT_MODULAR
#define PS2_KEYBOARD_DATA 4  ///  ---[ 1K ]---  // ( USB -D )
#define PS2_KEYBOARD_CLK  0  ///  ---[ 1K ]---  // ( USB +D )
#else
#define PS2_KEYBOARD_DATA 36  /// VP=GPIO36 ---[ 1K ]--- // ( USB -D )
#define PS2_KEYBOARD_CLK  39  /// VN=GPIO39 ---[ 1K ]---  // ( USB +D )
#endif

#endif
//--------------------------------------------------------------------------------
// NES CONTROLLER
//--------------------------------------------------------------------------------
#if NES_CONTROLLER
#define NESCTRL_Data       34    // The data pin for the NES controller
#define NESCTRL_Clock      25    // The clock pin for the NES controller ---[ 330R ]--- +3.3V (Must be pulled up to +3.3V)
#define NESCTRL_Latch      2     // The latch pin for the NES controller (STROBE) is pulled up by default on ESP32
#endif
//--------------------------------------------------------------------------------
// I2S AUDIO PINS:
//--------------------------------------------------------------------------------
#if I2SAUDIO_ENABLED
#ifdef NCAT_BOARD
#define I2S_bck_io_num 33             // BCK
#define I2S_ws_io_num 4               // LCK
#define I2S_data_out_num 25           // DIN
#endif
#ifdef NCAT_LITE
#define I2S_bck_io_num 33             // BCK
#define I2S_ws_io_num 4               // LCK
#define I2S_data_out_num 25           // DIN
#endif
#ifdef NCAT_TWATCH_V1
#define I2S_bck_io_num 26             // BCK
#define I2S_ws_io_num 25              // LCK
#define I2S_data_out_num 33           // DIN
#endif
#ifdef HACKED_NCAT_TWATCH_V2
#define I2S_bck_io_num 0              // BCK
#define I2S_ws_io_num 33              // LCK
#define I2S_data_out_num 26           // DIN
#endif
#ifdef NCAT_MODULAR
#define I2S_bck_io_num 27             //BCK
#define I2S_ws_io_num 32              //LCK
#define I2S_data_out_num 25           //DIN
#endif
#else
#define I2S_bck_io_num -1
#define I2S_ws_io_num -1
#define I2S_data_out_num -1
#endif
//--------------------------------------------------------------------------------
// PWM AUDIO PINS:
//--------------------------------------------------------------------------------
#if PWMAUDIO_ENABLED
#ifdef NCAT_TWATCH_V2
//#define PWMAUDIO_CONFIG_LEFT_CHANNEL_GPIO 2  //IR -> AUDIO HACK (GPIO 2 is Pulled up so PWM is not good solution here)
#define PWMAUDIO_CONFIG_LEFT_CHANNEL_GPIO -1
#define PWMAUDIO_CONFIG_RIGHT_CHANNEL_GPIO -1
#endif
#if defined(NCAT_BOARD) || defined(NCAT_LITE) || defined(NCAT_MINIMAL) || defined(NCAT_VGA)
#define PWMAUDIO_CONFIG_LEFT_CHANNEL_GPIO 27
#define PWMAUDIO_CONFIG_RIGHT_CHANNEL_GPIO -1
#endif
#endif
//--------------------------------------------------------------------------------
// HIMEM SUPPORT
//--------------------------------------------------------------------------------
#if HIMEM
#include "src/HIMEM/cache_sram_mmu.c"
#include "src/HIMEM/himem.h"
#include "src/HIMEM/spiram.h"
#include "src/HIMEM/spiram.c"
#endif
//--------------------------------------------------------------------------------
// BLUETOOTH SUPPORT
//--------------------------------------------------------------------------------
#if BLUETOOTH
#include "BLEUUID.h"                    // for manual connection... (esp_bt_controller_init)!
#include "esp_bt.h"                     // for esp_bt_controller_mem_release
#endif
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  LCD INIT
//--------------------------------------------------------------------------------
#if LCD_ENABLED

#define LGFX_USE_V1
#include "src/LovyanGFX/src/LovyanGFX.hpp"
lgfx::Panel_ST7789 _panel_instance;
lgfx::Bus_SPI _bus_instance; // SPI

#if LCD_BACKLIGHT_CONTROL
#if defined(NCAT_BOARD) || defined(NCAT_LITE) || defined(NCAT_MINIMAL)
lgfx::Light_PWM _light_instance;
#endif
#endif
#if TOUCHPAD_ENABLED
lgfx::Touch_FT5x06 _touch_instance;   // FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436
#endif
class LGFX: public lgfx::LGFX_Device {
  public:
    LGFX(void) {
      {
        auto cfg = _bus_instance.config();
#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
        // SPI
        cfg.spi_host = HSPI_HOST;       //(VSPI_HOST or HSPI_HOST)
        cfg.spi_mode = 0;
        cfg.freq_write = LCD_SPI_SPEED; // (80MHz) 1.54" ST7789
        cfg.freq_read = 1000000;
        cfg.spi_3wire = false;
        cfg.use_lock = false;
        cfg.dma_channel = 2;
        cfg.pin_sclk = 18;
        cfg.pin_mosi = 19;
        cfg.pin_miso = -1;
        cfg.pin_dc = 27;
#elif defined(NCAT_BOARD)
        // SPI
        cfg.spi_host = HSPI_HOST;       //(VSPI_HOST or HSPI_HOST)
        cfg.spi_mode = 3;
        cfg.freq_write = LCD_SPI_SPEED; // (80MHz) 1.54" ST7789
        cfg.freq_read = 40000000;
        cfg.spi_3wire = false;
        cfg.use_lock = false;
        cfg.dma_channel = 1;
        cfg.pin_sclk = 14;
        cfg.pin_mosi = 13;
        cfg.pin_miso = -1;
        cfg.pin_dc = 12;
#elif defined(NCAT_MODULAR)        
        // SPI
        cfg.spi_host = VSPI_HOST;       //(VSPI_HOST or HSPI_HOST)
        cfg.spi_mode = 3;
        cfg.freq_write = LCD_SPI_SPEED; // 
        cfg.freq_read = 40000000;
        cfg.spi_3wire = false;
        cfg.use_lock = false;
        cfg.dma_channel = 1;
        cfg.pin_sclk = 18;
        cfg.pin_mosi = 23;
        cfg.pin_miso = -1;
        cfg.pin_dc = 5;
#else
        // SPI
        cfg.spi_host = HSPI_HOST;       //(VSPI_HOST or HSPI_HOST)
        cfg.spi_mode = 3;
        cfg.freq_write = LCD_SPI_SPEED; // (40MHz) 1.3" ST7789
        cfg.freq_read = 40000000;
        cfg.spi_3wire = false;
        cfg.use_lock = false;
        cfg.dma_channel = 2;
        cfg.pin_sclk = 14;
        cfg.pin_mosi = 13;
        cfg.pin_miso = -1;
        cfg.pin_dc = 12;
#endif
        _bus_instance.config(cfg);
        _panel_instance.setBus( &_bus_instance);
      }
#if TOUCHPAD_ENABLED
      {
        auto cfg = _touch_instance.config();

        cfg.x_min = 0;
        cfg.x_max = 239;
        cfg.y_min = 0;
        cfg.y_max = 239;
        cfg.bus_shared = false;           // NOT SHARED ... (if enabled TWATCH FREEZE in TFT DMA !!!)
        cfg.offset_rotation = 2;          // 180 degrees

        //I2C
        cfg.i2c_port = 1;
        cfg.i2c_addr = 0x38;
        cfg.pin_sda = 23;
        cfg.pin_scl = 32;

        cfg.pin_int = 38;
        cfg.freq = 100000;

        _touch_instance.config(cfg);
        //_panel_instance.setTouch( &_touch_instance);
      }
#endif
#if LCD_BACKLIGHT_CONTROL
#if defined(NCAT_BOARD) || defined(NCAT_LITE) || defined(NCAT_MINIMAL)
      {
        auto cfg = _light_instance.config();

        cfg.pin_bl = LCD_BACKLIGHT_PIN;   // BL pin
        cfg.invert = false;               //
        cfg.freq   = 44100;               // freq.
        cfg.pwm_channel = 7;              //

        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance);
      }
#endif
#endif
    }
};

static LGFX tft;

#endif

//--------------------------------------------------------------------------------
// MICROSD CARD
//--------------------------------------------------------------------------------

#if MICROSD

#if defined(NCAT_BOARD) || defined(NCAT_MINIMAL) || defined(NCAT_VGA)
//micro_SD_Card:
#define SOFTSD_MOSI_PIN 23            // [VSPI MOSI]
#define SOFTSD_MISO_PIN 19            // [VSPI MISO]
#define SOFTSD_SCK_PIN 18             // [VSPI SCK]
#define SD_CS_PIN 5                   // [CS]
#endif
#if defined(NCAT_LITE) 
//micro_SD_Card:
#define SOFTSD_MOSI_PIN 23            // [VSPI MOSI]
#define SOFTSD_MISO_PIN 19            // [VSPI MISO]
#define SOFTSD_SCK_PIN 18             // [VSPI SCK]
#define SD_CS_PIN -1                  // [CS]
#endif
#if defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
//micro_SD_Card:
#define SOFTSD_MOSI_PIN 15            // 
#define SOFTSD_MISO_PIN 4             // 
#define SOFTSD_SCK_PIN 14             // 
#define SD_CS_PIN 13                  // [CS]
#endif
#if defined(NCAT_MODULAR)            //micro_SD_Card: //GPIO16 and GPIO17 can not use on WROVER
#define SOFTSD_MOSI_PIN 33            // 
#define SOFTSD_MISO_PIN 22            // 
#define SOFTSD_SCK_PIN 21             // 
#define SD_CS_PIN -1                  // [CS]
#endif

///#include "src/sdfat-beta/src/SdFatConfig.h"
#include "src/sdfat-beta/src/SdFat.h" // Use SDFAT-beta...

#if defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2) || defined(NCAT_MODULAR)
#include "src/sdfat-beta/src/SpiDriver/SdSpiSoftDriver.h" //this is required for SOFTWARE SPI!
//and define #define SPI_DRIVER_SELECT 2
#endif

//#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(25)) //20MHz recommending 25MHz MAX.
#if defined(NCAT_BOARD) || defined(NCAT_MINIMAL)
  #define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_HZ(20000000)) //20MHz recommending 25MHz MAX. SHARED VSPI (used also for USB Host)
#endif
#if defined(NCAT_VGA)
  #define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_HZ(20000000)) //13MHz recommending 25MHz MAX. SHARED VSPI (used also for USB Host)
#endif
#if defined(NCAT_LITE)
  #define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_HZ(20000000))  //can not be DEDICATED_SPI only SHARED_SPI works
#endif
#if defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2) || defined(NCAT_MODULAR)
   SoftSpiDriver < SOFTSD_MISO_PIN, SOFTSD_MOSI_PIN, SOFTSD_SCK_PIN > softSpi; //Soft SPI definition
  #define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(5), &softSpi) //Soft SPI is SLOWER

///  SdFatSoftSpi<SOFTSD_MISO_PIN, SOFTSD_MOSI_PIN, SOFTSD_SCK_PIN> SD;
#endif
//SdFatSoftSpi<SOFTSD_MISO_PIN, SOFTSD_MOSI_PIN, SOFTSD_SCK_PIN> SD;

SdFat SD;
File fp;


#endif
//--------------------------------------------------------------------------------
// JPG DECODER SUPPORT
//--------------------------------------------------------------------------------
#if JPG_DECODER
#include "src/JPG_SUPPORT/tjpgd.h"

JDEC jpgdec;
JRESULT jpgres;
uint8_t * JPGDATA;
#endif
//--------------------------------------------------------------------------------
// PNG DECODER SUPPORT
//--------------------------------------------------------------------------------
#if PNG_DECODER
#include "src/PNG_SUPPORT/upng.c"
#endif
//--------------------------------------------------------------------------------
// I2C BUS & RTC
//--------------------------------------------------------------------------------
#define I2C_SDA 21
#define I2C_SCL 22
#include <Wire.h>
#if CLOCK
#ifdef NCAT_BOARD
#include "src/ds3231/ds3231.h"
#endif
#ifdef NCAT_LITE
#include "src/ds1307/ErriezDS1307.h"
#endif
#endif
//--------------------------------------------------------------------------------
//  USB JOST MAX3428E
//--------------------------------------------------------------------------------
#if USBHOST
//MAX3428E:                           // Configuration is defined in: "src/USB_Host_Shield_2.0/avrpins.h"
#if defined(NCAT_BOARD)
  #define USB_CS   0                    // => Chip select control pin
  #define USB_INT  27                   // => Interrupt control pin
  #define USB_MISO 19                   // [VSPI MISO]
  #define USB_MOSI 23                   // [VSPI MOSI]
  #define USB_SCK  18                   // [VSPI SCK]
#endif
#if defined(NCAT_VGA)

// ! be careful when you choosing GPIO25 / GPIO26 this GPIOs are internal DAC pins... 
// do not make conflict of them, in other case you can burn them... !
// and GPIO can not be 32 or higher

  #define USB_CS   0                   // => Chip select control pin
  #define USB_INT  13                   // => Interrupt control pin
  #define USB_MISO 19                   // [VSPI MISO]
  #define USB_MOSI 23                   // [VSPI MOSI]
  #define USB_SCK  18                   // [VSPI SCK]
#endif  

extern "C" {
  uint8_t USB_HOST_CS=USB_CS;
  uint8_t USB_HOST_INT=USB_INT;
  uint8_t USB_HOST_MISO=USB_MISO;
  uint8_t USB_HOST_MOSI=USB_MOSI;
  uint8_t USB_HOST_SCK=USB_SCK;
}


//!!! USB HOST SHIELD AND PWM AUDIO CAN NOT BE USED AT SAME TIME ON NCAT PRO!!!

#define SPI_HAS_TRANSACTION
#define USBHOST_SPISPEED 20000000

#include <SPI.h>

//#define DEBUG_USB_HOST
#define _USE_LFN 1 ///THIS IS VERY IMPORTAT!
//----------------------------------------
#include "src/USB_Host_Shield_2.0/Usb.h"
#include "src/USB_Host_Shield_2.0/hidboot.h"
#include "src/USB_Host_Shield_2.0/usbhub.h"
#include "src/USB_Host_Shield_2.0/masstorage.h"
#include "src/USB_Host_Shield_2.0/usbhid.h"
#include "src/USB_Host_Shield_2.0/hiduniversal.h"
#include "src/USB_Host_Shield_2.0/hidescriptorparser.h"
#if USBHOST_STORAGE
  #include "generic_storage/PCpartition/PCPartition.h"
  #include "generic_storage/FAT/FatFS/src/ff.h"
  #include "generic_storage/FAT/FAT.cpp"
  #include "generic_storage/PCpartition/PCPartition.cpp" ///
  #include "generic_storage/Storage.cpp" ///
  #include "generic_storage/FAT/FatFS/src/option/unicode.c"
  #include "generic_storage/FAT/FatFS/src/ff.c"
#endif
#include "src/USB_Host_Shield_2.0/PS4BT.h"
//----------------------------------------
//USB HOST MAX3421E:
USB Usb;
//----------------------------------------
USBHub  Hub1(&Usb); //take care for enough power for this
USBHub  Hub2(&Usb);
//USBHub  Hub3(&Usb);
//USBHub  Hub4(&Usb);

/*HIDUniversal Hid1(&Usb);
HIDUniversal Hid2(&Usb);
HIDUniversal Hid3(&Usb);*/

#include "USB.h"


#if (USBHOST_HID || USBHOST_JOYSTICKS)
  HIDUniversal2 Hid1(&Usb);
  HIDUniversal2 Hid2(&Usb);
  HIDUniversal2 Hid3(&Usb);
  HIDUniversal2 Hid4(&Usb);
#endif

/*

HIDUniversal2 Hid(&Usb);
///UniHidReportParser Uni;

#include "USB_HID.h"

#if USBHOST_JOYSTICKS
/*
#include "JOYSTICK.h"

///namespace USB_JOYSTICK_DEVICES {
  ///using namespace USB_JOYSTICK_DEVICES;

JoystickEvents JoyEvents1;
JoystickEvents2 JoyEvents2;
JoystickReportParser Joy1(&JoyEvents1);
JoystickReportParser2 Joy2(&JoyEvents2);
*/
///#endif


#if USBHOST_HID
  //take care for order of INITIALIZATION:
/*  HIDBoot < USB_HID_PROTOCOL_KEYBOARD > HidKeyboard( &Usb, true);
  HIDBoot < USB_HID_PROTOCOL_MOUSE > HidMouse( &Usb, true);
  HIDBoot < USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE > HidComposite( &Usb, true); //false=HID_BOOT_PROTOCOL true=HID_RPT_PROTOCOL !!!!
*/
#endif

#if USBHOST_BLUETOOTH
//----------------------------------------
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
//----------------------------------------
// You can create the instance of the PS4BT class in two ways:
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode
PS4BT PS4(&Btd, PAIR);
// After that you can simply create the instance like so and then press the PS button on the device
//PS4BT PS4(&Btd);
#endif




//----------------------------------------
#if USBHOST_STORAGE
PCPartition * PT;
static PFAT * Fats[_VOLUMES];
static part_t parts[_VOLUMES];
static storage_t sto[_VOLUMES];
#define mbxs 128
static uint8_t My_Buff_x[mbxs]; // File read buffer
volatile uint8_t current_state = 1;
volatile uint8_t last_state = 0;
volatile bool fatready = false;
volatile bool partsready = false;
volatile bool notified = false;
volatile bool runtest = false;
volatile bool usbon = false;
volatile uint32_t usbon_time;
volatile bool change = false;
volatile bool reportlvl = false;
int cpart = 0;
bool isfat(uint8_t t) {
  return (t == 0x01 || t == 0x04 || t == 0x06 || t == 0x0b || t == 0x0c || t == 0x0e || t == 0x1);
}
#endif
#endif
//--------------------------------------------------------------------------------
#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)

#if defined(NCAT_TWATCH_V1)
#define LILYGO_WATCH_2020_V1
#elif defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
#define LILYGO_WATCH_2020_V2
#endif

//#include <LilyGoWatch.h>

#include "TTGO_TWatch_Library/src/libraries/Adafruit_DRV2605_Library/Adafruit_DRV2605.cpp"
#include "TTGO_TWatch_Library/src/LilyGoWatch.h" //we use customized LIBRARY

TTGOClass * ttgo;

#include "TTGO_TWatch_Library/src/TTGO.cpp"
//#include "TTGO_TWatch_Library/src/libraries/TFT_eSPI/TFT_eSPI.cpp"
#include "TTGO_TWatch_Library/src/drive/axp/axp20x.cpp"
#include "TTGO_TWatch_Library/src/drive/rtc/pcf8563.cpp"
#include "TTGO_TWatch_Library/src/drive/bma423/bma423.c"
#include "TTGO_TWatch_Library/src/drive/bma423/bma4.c"
#include "TTGO_TWatch_Library/src/drive/bma423/bma.cpp"
#include "TTGO_TWatch_Library/src/drive/i2c/i2c_bus.cpp"
#include "TTGO_TWatch_Library/src/libraries/TFT_eSPI/Fonts/Font16.c"
#include "TTGO_TWatch_Library/src/libraries/TFT_eSPI/Fonts/Font32rle.c"
#include "TTGO_TWatch_Library/src/libraries/TFT_eSPI/Fonts/Font64rle.c"
#include "TTGO_TWatch_Library/src/libraries/TFT_eSPI/Fonts/Font72rle.c"
#include "TTGO_TWatch_Library/src/libraries/TFT_eSPI/Fonts/Font7srle.c"
#include "TTGO_TWatch_Library/src/libraries/TFT_eSPI/Fonts/glcdfont.c"
#include "TTGO_TWatch_Library/src/drive/fx50xx/focaltech.cpp"

bool TWATCH_irq = false;
bool toggle_screen = true;
#endif




//██████████████████████████████████████████████████
//█   FUNCTIONS INCLUDES
//██████████████████████████████████████████████████

//--------------------------------------------------------------------------------
void MEMORY_STATUS() {
  printf("========================================\n");
  printf("PSRAM first 4MB size : %u \n", ESP.getPsramSize());
  printf("PSRAM first 4MB free : %u \n", ESP.getMaxAllocPsram());
#if HIMEM
  printf("PSRAM HI-MEM    size : %u \n", esp_himem_get_phys_size());
  printf("PSRAM HI-MEM    free : %u \n", esp_himem_get_free_size());
#endif
  printf("========================================\n");
  printf("Internal RAM  size   : %u \n", ESP.getHeapSize());
  printf("Internal RAM  free   : %u \n", ESP.getFreeHeap());
  printf("========================================\n");
}
//--------------------------------------------------------------------------------
void HALT() {
  while (1) {
    vTaskDelay___(1); //infinite loop.
  }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// PALETTES & FONTS & ICONS:
//--------------------------------------------------------------------------------
#include "FONTS_PAL.h"
#include "ICONS.h"
//--------------------------------------------------------------------------------
// FLASH DATA:
//--------------------------------------------------------------------------------
#include "FLASH.h"
//--------------------------------------------------------------------------------
// AUDIO LIB:
//--------------------------------------------------------------------------------
#include "AUDIO.h"
//--------------------------------------------------------------------------------
// INPUT SYSTEM:
//--------------------------------------------------------------------------------
#include "INPUT.h"
//--------------------------------------------------------------------------------
// FUNCTIONS:
//--------------------------------------------------------------------------------
#include "FUNCTIONS.h"
//--------------------------------------------------------------------------------
// MENU FUNCTIONS:
//--------------------------------------------------------------------------------
#include "MENU.h"
//--------------------------------------------------------------------------------
// BLUETOOTH LIB:
//--------------------------------------------------------------------------------
#if BLUETOOTH
#include "BLUETOOTH.h"
#endif
//--------------------------------------------------------------------------------
///#include "USB.h" ///
#if USBHOST
#if USBHOST_HID
#if !USB_UNIVERSAL_HID
KbdRptParser KbdPrs;
MouseRptParser MousePrs;
#endif
#endif
#endif
//--------------------------------------------------------------------------------


//██████████████████████████████████████████████████
//█  I2C TASK SERIAL (for KM SWITCH)
//██████████████████████████████████████████████████

uint8_t ACTIVE_KM_DEVICE = 0;

#define KM_BUF_SIZE 8
char KM_BUFFER[KM_BUF_SIZE] = {
  0
};

void writer(uint8_t * BYTES, uint8_t LEN) {
  //printf("OUT: %c\n",out_byte);
  printf("KM_OUT: ");

  if (SCROLLLOCK == 1) {
    Wire.beginTransmission(ADDRESS_SUB_ARDUINO1); // transmit to device
    for (uint8_t i = 0; i < KM_BUF_SIZE && i < LEN; i++) {
      Wire.write(KM_BUFFER[i]); // sends one byte
      printf(" %c", KM_BUFFER[i]);
    }
    Wire.endTransmission(); // stop transmitting
    printf("\n");
  } else if (SCROLLLOCK == 2) {
    Wire.beginTransmission(ADDRESS_SUB_ARDUINO2); // transmit to device
    for (uint8_t i = 0; i < KM_BUF_SIZE && i < LEN; i++) {
      Wire.write(KM_BUFFER[i]); // sends one byte
      printf(" %c", KM_BUFFER[i]);
    }
    Wire.endTransmission(); // stop transmitting
    printf("\n");
  } else if (SCROLLLOCK == 3) {
    Wire.beginTransmission(ADDRESS_SUB_ARDUINO3); // transmit to device
    for (uint8_t i = 0; i < KM_BUF_SIZE && i < LEN; i++) {
      Wire.write(KM_BUFFER[i]); // sends one byte
      printf(" %c", KM_BUFFER[i]);
    }
    Wire.endTransmission(); // stop transmitting
    printf("\n");
  } else if (SCROLLLOCK == 4) {
    Wire.beginTransmission(ADDRESS_SUB_ARDUINO4); // transmit to device
    for (uint8_t i = 0; i < KM_BUF_SIZE && i < LEN; i++) {
      Wire.write(KM_BUFFER[i]); // sends one byte
      printf(" %c", KM_BUFFER[i]);
    }
    Wire.endTransmission(); // stop transmitting
    printf("\n");
  } else if (SCROLLLOCK == 5) {
    Wire.beginTransmission(ADDRESS_SUB_ARDUINO5); // transmit to device
    for (uint8_t i = 0; i < KM_BUF_SIZE && i < LEN; i++) {
      Wire.write(KM_BUFFER[i]); // sends one byte
      printf(" %c", KM_BUFFER[i]);
    }
    Wire.endTransmission(); // stop transmitting
    printf("\n");
  } else if (SCROLLLOCK == 6) {
    Wire.beginTransmission(ADDRESS_SUB_ARDUINO6); // transmit to device
    for (uint8_t i = 0; i < KM_BUF_SIZE && i < LEN; i++) {
      Wire.write(KM_BUFFER[i]); // sends one byte
      printf(" %c", KM_BUFFER[i]);
    }
    Wire.endTransmission(); // stop transmitting
    printf("\n");
  }
}

void reader(void) {
  printf("IN: ");

  uint8_t in_byte = 0;

  Wire.requestFrom(ADDRESS_SUB_ARDUINO1, 1); // request 1 bytes from sub device

  while (Wire.available()) { // sub may send less than requested
    in_byte = Wire.read(); // receive a byte as character
    printf("%c", in_byte);
  }
  printf("\n");
}

//--------------------------------------------------------------------------------

uint8_t Ignore_Release_Keys = 0;
uint8_t KM_process_SCROLLOCK(uint8_t KEY, uint8_t STATE) {

  if (STATE == 1) {
    if (SCROLLLOCK_millis == 0 && KEY == 0x47) {
      SCROLLLOCK_millis = millis(); //ScrollLock pressed
      return 1;
    }
    else if (SCROLLLOCK_millis > 0 && KEY == 41) { //ScrollLock + ESC (Release Keyboard and Mouse)
      SCROLLLOCK_millis = 0;
      SCROLLLOCK = 0;
      Ignore_Release_Keys = 1;
      DRAW_KM_INFO();
      return 1;
    }
    else if (SCROLLLOCK_millis > 0 && KEY == 0x3A) { //ScrollLock + F1
      SCROLLLOCK_millis = 0;
      SCROLLLOCK = 1;
      Ignore_Release_Keys = 1;
      DRAW_KM_INFO();
      return 1;
    }
    else if (SCROLLLOCK_millis > 0 && KEY == 0x3B) { //ScrollLock + F2
      SCROLLLOCK_millis = 0;
      SCROLLLOCK = 2;
      Ignore_Release_Keys = 1;
      DRAW_KM_INFO();
      return 1;
    }
    else if (SCROLLLOCK_millis > 0 && KEY == 0x3C) { //ScrollLock + F3
      SCROLLLOCK_millis = 0;
      SCROLLLOCK = 3;
      Ignore_Release_Keys = 1;
      DRAW_KM_INFO();
      return 1;
    }
    else if (SCROLLLOCK_millis > 0 && KEY == 0x3D) { //ScrollLock + F4
      SCROLLLOCK_millis = 0;
      SCROLLLOCK = 4;
      Ignore_Release_Keys = 1;
      DRAW_KM_INFO();
      return 1;
    }
    else if (SCROLLLOCK_millis > 0 && KEY == 0x3E) { //ScrollLock + F5
      SCROLLLOCK_millis = 0;
      SCROLLLOCK = 5;
      Ignore_Release_Keys = 1;
      DRAW_KM_INFO();
      return 1;
    }
    else if (SCROLLLOCK_millis > 0 && KEY == 0x3F) { //ScrollLock + F6
      SCROLLLOCK_millis = 0;
      SCROLLLOCK = 6;
      Ignore_Release_Keys = 1;
      DRAW_KM_INFO();
      return 1;
    }
    /*      else
          if (SCROLLLOCK_millis>0 && SCROLLLOCK_millis+500<millis()) {  //wait 0.5 second to switch scrolllock
             SCROLLLOCK_millis=0; //reset SCROLLLOCK call

             //Switch ScrollLock 1..6
             if (SCROLLLOCK==0 && KM_max_devices>0) SCROLLLOCK=1;
             else if (SCROLLLOCK==1 && KM_max_devices>1) SCROLLLOCK=2;
             else if (SCROLLLOCK==2 && KM_max_devices>2) SCROLLLOCK=3;
             else if (SCROLLLOCK==3 && KM_max_devices>3) SCROLLLOCK=4;
             else if (SCROLLLOCK==4 && KM_max_devices>4) SCROLLLOCK=5;
             else if (SCROLLLOCK==5 && KM_max_devices>5) SCROLLLOCK=6;
             else SCROLLLOCK=1;
          }*/
  }
  if (STATE == 0 && Ignore_Release_Keys == 1 && ((KEY >= 0x3A && KEY <= 0x3F) || ( KEY == 41))) {
    Ignore_Release_Keys = 0;
    return 1;
  }


  printf("SCROLLLOCK KMSWITCH: %d\n", SCROLLLOCK);
  if (SCROLLLOCK_millis > 0) return 1;
  return 0;
}
//--------------------------------------------------------------------------------


void KM_task_mouse(uint8_t Buttons, uint8_t Xdiff, uint8_t Ydiff, uint8_t Zdiff) {
#if USBHOST_HID
  KM_BUFFER[0] = 'M'; //Mouse
  KM_BUFFER[1] = Buttons;
  KM_BUFFER[2] = Xdiff;
  KM_BUFFER[3] = Ydiff;
  KM_BUFFER[4] = Zdiff;
  KM_BUFFER[5] = '.'; //End.
  writer((uint8_t * ) & KM_BUFFER, 6);
#endif
}

void KM_task_keyboard(uint8_t KEY, uint8_t STATE) { //STATE 0=KEYRELEASE 1=KEYPRESS
#if USBHOST_HID
  if (KM_process_SCROLLOCK(KEY, STATE) == 0) { //process KM switch (check if SCROLLOCK pressed)
    KM_BUFFER[0] = 'K'; //Keyboard
    KM_BUFFER[1] = KEY;
    KM_BUFFER[2] = STATE; //0=released KEY 1=pressed KEY 2=released control KEY 3=pressed control KEY
    KM_BUFFER[3] = '.'; //End.
    writer((uint8_t * ) &KM_BUFFER, 6);
  }
#endif
}

void DRAW_KM_INFO() {
  if (SCROLLLOCK == 0) {
    screenmemory_fillscreen_static(0);
    draw_char_xy_transparent_double((240 - 64) / 2, (240 - 128) / 2, '0', font_7SEGMENT, 0xffff); //64x128
  } else if (SCROLLLOCK == 1) {
    screenmemory_fillscreen_static(0);
    draw_char_xy_transparent_double((240 - 64) / 2, (240 - 128) / 2, '1', font_7SEGMENT, 0xffff); //64x128
  } else if (SCROLLLOCK == 2) {
    screenmemory_fillscreen_static(0);
    draw_char_xy_transparent_double((240 - 64) / 2, (240 - 128) / 2, '2', font_7SEGMENT, 0xffff); //64x128
  } else if (SCROLLLOCK == 3) {
    screenmemory_fillscreen_static(0);
    draw_char_xy_transparent_double((240 - 64) / 2, (240 - 128) / 2, '3', font_7SEGMENT, 0xffff); //64x128
  } else if (SCROLLLOCK == 4) {
    screenmemory_fillscreen_static(0);
    draw_char_xy_transparent_double((240 - 64) / 2, (240 - 128) / 2, '4', font_7SEGMENT, 0xffff); //64x128
  } else if (SCROLLLOCK == 5) {
    screenmemory_fillscreen_static(0);
    draw_char_xy_transparent_double((240 - 64) / 2, (240 - 128) / 2, '5', font_7SEGMENT, 0xffff); //64x128
  } else if (SCROLLLOCK == 6) {
    screenmemory_fillscreen_static(0);
    draw_char_xy_transparent_double((240 - 64) / 2, (240 - 128) / 2, '6', font_7SEGMENT, 0xffff); //64x128
  }
}













void die(char *fmt, ...)
{
  /*
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    //loader_unload();
    //shutdown();
    exit(1);
  */
  printf("DIED.\n");
  while (1) {}
}

uint32_t prev_us = 0;

#ifndef HAVE_USLEEP
static void my_usleep(unsigned int us)
{
  /*  struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = us;
    select(0, NULL, NULL, NULL, &tv);
  */
  delayMicroseconds(us);
}
#endif

void *sys_timer()
{
  /*  struct timeval *tv;

    tv = malloc(sizeof(struct timeval));
    gettimeofday(tv, NULL);
    return tv;
  */
}

///int sys_elapsed(struct timeval *prev)
int sys_elapsed()
{
  /*  struct timeval tv;
    int secs, usecs;

    gettimeofday(&tv, NULL);
    secs = tv.tv_sec - prev->tv_sec;
    usecs = tv.tv_usec - prev->tv_usec;
     prev = tv;
    if (!secs) return usecs;
    return 1000000 + usecs;
  */

  uint32_t now_us = micros() - prev_us;
  prev_us = micros();
  return (now_us);
}

void sys_sleep(int us)
{
  if (us <= 0) return;
#ifdef HAVE_USLEEP
  usleep(us);
#else
  my_usleep(us);
#endif
}




uint32_t AUDIO_TIMER = 0;






//██████████████████████████████████████████████████
//█   APPS INCLUDES
//██████████████████████████████████████████████████

//________________________________________________________________________________
//
// NES EMU SECTION
//________________________________________________________________________________

// Define this if running on little-endian (x86) systems
#define HOST_LITTLE_ENDIAN //!!! important for Arduino
#define ZERO_LENGTH 0
#define NES_UNUSED(x)((x) = (x)) // quell stupid compiler warnings

#ifdef PAL
#define NES_REFRESH_RATE 50 //PAL = 50Hz
#else
#define NES_REFRESH_RATE 60 //NTSC = 60Hz
#endif

//AUDIO_SETUP
#define DEFAULT_SAMPLERATE  22080
#define DEFAULT_FRAGSIZE (DEFAULT_SAMPLERATE / NES_REFRESH_RATE)

//VIDEO_SETUP
#define NES_VISIBLE_HEIGHT 240
#define NES_SCREEN_WIDTH 256
#define NES_SCREEN_HEIGHT 240

#define MAX_MEM_HANDLERS 32

#define DEFAULT_WIDTH NES_SCREEN_WIDTH
#define DEFAULT_HEIGHT NES_VISIBLE_HEIGHT

#define NES_CLOCK_DIVIDER 12 //default: 12
//#define  NES_MASTER_CLOCK     21477272.727272727272
#define NES_MASTER_CLOCK (236250000 / 11)
#define NES_SCANLINE_CYCLES (1364.0 / NES_CLOCK_DIVIDER)
#define NES_FIQ_PERIOD (NES_MASTER_CLOCK / NES_CLOCK_DIVIDER / 60)

#define NES_RAMSIZE 0x800

#define NES6502_NUMBANKS 16
#define NES6502_BANKSHIFT 12
#define NES6502_BANKSIZE (0x10000 / NES6502_NUMBANKS)
#define NES6502_BANKMASK (NES6502_BANKSIZE - 1)

//--------------------------------------------------------------------------------
void (*audio_callback)(void *buffer, int length) = NULL; //for NES
void do_audio_frame();                                   //for NES
void play_audio_frame();                                 //for NES
//--------------------------------------------------------------------------------


#include "NES.h"

uint32_t tickcnt = 0;

//________________________________________________________________________________
//
// GBC EMU SECTION
//________________________________________________________________________________

/*static int framelen = 33333;
  uint32_t framenum = 0;
  static uint8_t skipped = 0;
  uint32_t gbc_frame_time = 0;
  uint32_t deltatime = 0;

  #define GBC_SAMPLERATE 16000
  #define GBC_FRAGSIZE 100 //not more than 240?

  #define GBC_ENABLE_BIOS 0 // if set 0 = direct boot GBC rom
*/


#define GBC_SAMPLERATE 16000
#define GBC_FRAGSIZE 160

#define GBC_ENABLE_BIOS 0 // if set 0 = direct boot GBC rom

uint8_t GBC_IN_HIMEM = 0;

uint8_t *GBC_HIMEM_BANK0_BUFFER; //16kB memory ( in PSRAM)
uint8_t GBC_HIMEM_BANK0_BUFFER_LOADED = 0;

uint8_t* GBC_HIMEM_BANK_BUFFER; //16kB memory (not in PSRAM)
uint8_t GBC_HIMEM_BANK_BUFFER_ALLOCATED = 0;
uint8_t GBC_HIMEM_BANK_UPDATE_POSITION = 0;

uint16_t GBC_HIMEM_BANK_SELECTED[64] = {0}; ///experimental.
uint8_t *GBC_HIMEM_BANK[64];  ///experimental.


static int sound = 1;
#define AUDIO_HZ 22050
#define AUDIO_BUFFER_LENGTH (AUDIO_HZ / 16 + 1)
static int16_t* audioBuffer;
///static int16_t* audioBuffer1;
///static int16_t* audioBuffer2;

static volatile int audio_done;






#include "GBC.h"

void vid_init()
{
#if GBC_EMULATOR
  ///  lcd.out.format = GB_PIXEL_565_LE;
  lcd.out.format == GB_PIXEL_565_LE; ///

  lcd.out.buffer = SCREENMEMORY;
  lcd.out.enabled = 1;
#endif
}

void pcm_init()
{
#if GBC_EMULATOR
  /*  int i;
    SDL_AudioSpec as;
  */
  ///if (!audioBuffer)
#ifndef NCAT_NO_PSRAM
  audioBuffer = (int16_t*)ps_malloc(AUDIO_BUFFER_LENGTH * 2);
#else 
  audioBuffer = (int16_t*)malloc(AUDIO_BUFFER_LENGTH * 2);
#endif

  ///audioBuffer1 = (int16_t*)ps_malloc(AUDIO_BUFFER_LENGTH * 2);
  ///audioBuffer2 = (int16_t*)ps_malloc(AUDIO_BUFFER_LENGTH * 2);


  if (!sound) return;
  /*
    if (SDL_Init(SDL_INIT_AUDIO))
    {
      printf("SDL: Couldn't initialize SDL sou,d: %s\n", SDL_GetError());
      exit(1);
    }

    as.freq = AUDIO_HZ;
    as.format = AUDIO_S16SYS;
    as.channels = 2;
    as.samples = 256;
    as.callback = audio_callback;
    as.userdata = 0;
    if (SDL_OpenAudio(&as, 0) == -1)
      return;
  */

  pcm.hz = AUDIO_HZ;
  pcm.stereo = 1;
  pcm.len = AUDIO_BUFFER_LENGTH;
  pcm.buf = audioBuffer;
  ///pcm.buf1 = audioBuffer1;
  ///pcm.buf2 = audioBuffer2;

  pcm.pos = 0;
  memset(pcm.buf, 0, pcm.len);
  ///pcm.pos1 = 0;
  ///pcm.pos2 = 0;
  ///memset(pcm.buf1, 0, pcm.len*2);
  ///memset(pcm.buf2, 0, pcm.len*2);

  ///sound_mutex = SDL_CreateMutex();
  ///sound_cv = SDL_CreateCond();

  ///SDL_PauseAudio(0);
#endif
}

uint8_t GBC_pcm_busy = 0;

uint8_t pcm_submit() {
#if GBC_EMULATOR

  ///if (GBC_pcm_busy==1) return 0;
  ///GBC_pcm_busy=1;

  /*if (pcm.stereo) {
    if (pcm.pos1 >= pcm.len-2) {
    AUDIO_WRITE((uint8_t*)pcm.buf1,(pcm.pos1-2)*2,1); ///portMAX_DELAY
    pcm.pos1=0;
    }
    else if (pcm.pos2 >= pcm.len-2) {
    AUDIO_WRITE((uint8_t*)pcm.buf2,(pcm.pos2-2)*2,1); ///portMAX_DELAY
    pcm.pos2=0;
    }
    } else {
    if (pcm.pos1 >= pcm.len-1) {
    AUDIO_WRITE((uint8_t*)pcm.buf1,(pcm.pos1-1)*2,1); ///portMAX_DELAY
    pcm.pos1=0;
    }
    else if (pcm.pos2 >= pcm.len-1) {
    AUDIO_WRITE((uint8_t*)pcm.buf2,(pcm.pos2-1)*2,1); ///portMAX_DELAY
    pcm.pos2=0;
    }
    }*/


  if (pcm.stereo && pcm.pos > 2) {
    AUDIO_WRITE((uint8_t*)pcm.buf, (pcm.pos) * 2, 0); ///portMAX_DELAY
    pcm.pos = 0;

  } else if (!pcm.stereo && pcm.pos > 1) {
    AUDIO_WRITE((uint8_t*)pcm.buf, (pcm.pos) * 2, 0); ///portMAX_DELAY
    pcm.pos = 0;
  }


  ///GBC_pcm_busy=0;
  ///AUDIO_TIMER=millis___();

  return 1;

  /*
    //--------------------------------------------------------------------------------
    if (BUFIDX2<BUFIDX1) {
     while (gbcaudio_busy) delayMicroseconds(1); // need wait to sound_mix() complete...
     pcm_busy=1;
     while(!gbcaudio_busy && BUFIDX2<BUFIDX1) {
        AUDIO_WRITE((uint8_t*)audio_frame[BUFIDX2%8],4*BUFSIZE[BUFIDX2%8],0); ///
        BUFIDX2++;
     }
     pcm_busy=0;
    }
  */
#endif
  return 0;
  //--------------------------------------------------------------------------------
}

uint8_t gbc_I2S_enabled = 0;



uint8_t LAST_START = 0;
uint8_t LAST_SELECT = 0;
uint8_t LAST_A = 0;
uint8_t LAST_B = 0;
uint8_t LAST_UP = 0;
uint8_t LAST_DOWN = 0;
uint8_t LAST_LEFT = 0;
uint8_t LAST_RIGHT = 0;

void ev_poll() {
#if GBC_EMULATOR
  INPUTREAD();

  if (INPUT_START != LAST_START) {
    if (INPUT_START) hw_setpad(PAD_START, 1);
    else hw_setpad(PAD_START, 0);
    LAST_START = INPUT_START;
  }
  if (INPUT_SELECT != LAST_SELECT) {
    if (INPUT_SELECT) hw_setpad(PAD_SELECT, 1);
    else hw_setpad(PAD_SELECT, 0);
    LAST_SELECT = INPUT_SELECT;
  }
  if (INPUT_A != LAST_A) {
    if (INPUT_A) hw_setpad(PAD_A, 1);
    else hw_setpad(PAD_A, 0);
    LAST_A = INPUT_A;
  }
  if (INPUT_B != LAST_B) {
    if (INPUT_B) hw_setpad(PAD_B, 1);
    else hw_setpad(PAD_B, 0);
    LAST_B = INPUT_B;
  }
  if (INPUT_UP != LAST_UP) {
    if (INPUT_UP) hw_setpad(PAD_UP, 1);
    else hw_setpad(PAD_UP, 0);
    LAST_UP = INPUT_UP;
  }
  if (INPUT_DOWN != LAST_DOWN) {
    if (INPUT_DOWN) hw_setpad(PAD_DOWN, 1);
    else hw_setpad(PAD_DOWN, 0);
    LAST_DOWN = INPUT_DOWN;
  }
  if (INPUT_LEFT != LAST_LEFT) {
    if (INPUT_LEFT) hw_setpad(PAD_LEFT, 1);
    else hw_setpad(PAD_LEFT, 0);
    LAST_LEFT = INPUT_LEFT;
  }
  if (INPUT_RIGHT != LAST_RIGHT) {
    if (INPUT_RIGHT) hw_setpad(PAD_RIGHT, 1);
    else hw_setpad(PAD_RIGHT, 0);
    LAST_RIGHT = INPUT_RIGHT;
  }

  //........................................................................................
  if (((INPUT_START == 1 && INPUT_SELECT == 1) || INPUT_SYSTEM == 1) && !INPUT_A && !INPUT_B) { //EXIT NES EMULATOR CALL
    if (InGame_Menu == 2) {
      vTaskDelay___(100);
      do_window_menu();
      INPUTCLEAR();
      AUDIOREADY = 0;
    } else {
      AUDIOREADY = 0;
      POWER = 0; //exit
    }
  }
  //........................................................................................

  INPUTCLEAR();
#endif
}



int8_t GBC_savestate(char* Filename) {

#if MICROSD
#if GBC_EMULATOR

  ///byte *buf = (byte*)calloc(1, 4096);
#ifndef NCAT_NO_PSRAM  
  byte *buf = (byte*)ps_calloc(1, 4096);
#else 
  byte *buf = (byte*)calloc(1, 4096);
#endif  
  if (!buf) return -2;

  ///FILE *fp = fopen(file, "wb");
  printf("*** GBC savestate filename: %s ***\n", Filename); ///
  fp = SD.open(Filename, O_RDWR | O_CREAT | O_AT_END); ///
  fp.truncate(0); ///


  if (!fp) {
    ///goto _error_save;

    if (buf) free(buf);
    return -1;

  }

  sblock_t blocks[] = {
    {buf, 1},
    ///    {hw.rambanks, hw.cgb ? 8 : 2},
    ///    {lcd.vbank, hw.cgb ? 4 : 2},
    {GBC_cart.rambanks, GBC_cart.ramsize * 2},
    {NULL, 0},
  };

  un32 (*header)[2] = (un32 (*)[2])buf;

  ver = SAVE_VERSION;

  for (int i = 0; svars[i].ptr; i++)
  {
    un32 d = 0;

    switch (svars[i].len)
    {
      case 1:
        d = *(byte *)svars[i].ptr;
        break;
      case 2:
        d = *(un16 *)svars[i].ptr;
        break;
      case 4:
        d = *(un32 *)svars[i].ptr;
        break;
    }

    header[i][0] = *(un32 *)svars[i].key;
    header[i][1] = LIL(d);
  }

  memcpy(buf + hiofs, hw.ioregs, sizeof hw.ioregs);
  memcpy(buf + palofs, lcd.pal, sizeof lcd.pal);
  memcpy(buf + oamofs, lcd.oam.mem, sizeof lcd.oam);
  memcpy(buf + wavofs, snd.wave, sizeof snd.wave);

  for (int i = 0; blocks[i].ptr != NULL; i++)
  {
    /*if (fwrite(blocks[i].ptr, 4096, blocks[i].len, fp) < 1)
      {
      MESSAGE_ERROR("Write error in block %d\n", i);
      goto _error;
      }*/
    printf("GBC Writing to save file %d bytes.\n", 4096 * blocks[i].len);
    fp.write(blocks[i].ptr, 4096 * blocks[i].len); ///
  }

  /// memory allocated variables NCAT mod
  for (uint8_t i = 0; i < 8; i++) fp.write(gb_hw_rambanks[i], 4096); ///
  fp.write(gb_lcd_vbank[0], 8192); ///
  fp.write(gb_lcd_vbank[1], 8192); ///

  ///fclose(fp);
  fp.close(); ///

  free(buf);

  return 0;

  ///_error_save:
  ///if (fp) fclose(fp);
  ///  if (fp) fp.close(); ///
  ///  if (buf) free(buf);

#endif
#endif

  ///return -1;



}

int8_t GBC_loadstate(char* Filename) {

#if MICROSD
#if GBC_EMULATOR

  ///byte* buf = (byte*)calloc(1, 4096);
#ifndef NCAT_NO_PSRAM  
  byte* buf = (byte*)ps_calloc(1, 4096); ///
#else
  byte* buf = (byte*)calloc(1, 4096); ///
#endif

  if (!buf) return -2;

  ///  FILE *fp = fopen(file, "rb");

  printf("*** GBC loadstate filename: %s ***\n", Filename); ///
  fp = SD.open(Filename); ///

  if (!fp) {
    if (buf) free(buf);
    return -1;
    ///goto _error_load;

  }

  sblock_t blocks[] = {
    {buf, 1},
    ///    {hw.rambanks, hw.cgb ? 8 : 2},
    ///    {lcd.vbank, hw.cgb ? 4 : 2},
    {GBC_cart.rambanks, GBC_cart.ramsize * 2},
    {NULL, 0},
  };

  for (int i = 0; blocks[i].ptr != NULL; i++)
  {
    /*if (fread(blocks[i].ptr, 4096, blocks[i].len, fp) < 1)
      {
      MESSAGE_ERROR("Read error in block %d\n", i);
      goto _error;
      }*/

    printf("GBC Reading from save file %d bytes.\n", (4096 * blocks[i].len));
    for (uint32_t tmp = 0; tmp < (4096 * blocks[i].len); tmp++) ((uint8_t*)blocks[i].ptr)[tmp] = fp.read(); ///

  }

  un32 (*header)[2] = (un32 (*)[2])buf;

  for (int i = 0; svars[i].ptr; i++)
  {
    un32 d = 0;

    for (int j = 0; header[j][0]; j++)
    {
      if (header[j][0] == *(un32 *)svars[i].key)
      {
        d = LIL(header[j][1]);
        break;
      }
    }

    switch (svars[i].len)
    {
      case 1:
        *(byte *)svars[i].ptr = d;
        break;
      case 2:
        *(un16 *)svars[i].ptr = d;
        break;
      case 4:
        *(un32 *)svars[i].ptr = d;
        break;
    }
  }

  if (ver != SAVE_VERSION)
    MESSAGE_ERROR("Save file version mismatch!\n");

  memcpy(hw.ioregs, buf + hiofs, sizeof hw.ioregs);
  memcpy(lcd.pal, buf + palofs, sizeof lcd.pal);
  memcpy(lcd.oam.mem, buf + oamofs, sizeof lcd.oam);
  memcpy(snd.wave, buf + wavofs, sizeof snd.wave);


  /// memory allocated variables NCAT mod
  for (uint8_t i = 0; i < 8; i++) for (uint32_t tmp = 0; tmp < 4096; tmp++) gb_hw_rambanks[i][tmp] = fp.read(); ///
  for (uint32_t tmp = 0; tmp < 8192; tmp++) gb_lcd_vbank[0][tmp] = fp.read(); ///
  for (uint32_t tmp = 0; tmp < 8192; tmp++) gb_lcd_vbank[1][tmp] = fp.read(); ///



  ///fclose(fp);
  fp.close(); ///

  free(buf);

  // Disable BIOS. This is a hack to support old saves
  R_BIOS = 0x1;

  lcd_rebuildpal();
  sound_dirty();
  hw_updatemap();

  return 0;

  ///_error_load:
  ///if (fp) fclose(fp);

  ///if (fp) fp.close(); ///
  ///if (buf) free(buf);

#endif
#endif

  ///  return -1;
}




//________________________________________________________________________________
//
// SMS&GG EMU SECTION
//________________________________________________________________________________

/*#define Tickrate 1000 // 200
  #define SMS_FPS 60

  #define SNDRATE 22080 // 22050
  #define DEFAULT_FRAGSIZE_ SNDRATE / SMS_FPS // 128

  #define SMS_DEFAULT_WIDTH 256
  #define SMS_DEFAULT_HEIGHT 192
*/
#include "SMS.h"
//________________________________________________________________________________
//
// AUDIO PLAYER SECTION
//________________________________________________________________________________

bool PAUSED = false;
#if MICROSD
#include "src/AUDIO_PLAYER/src/Audio.h"

Audio audio;
bool player_I2S_enabled = false;
#endif

//________________________________________________________________________________
//
// VISUALIZATIONS SECTION
//________________________________________________________________________________

#define SAMPLES 128

uint8_t visualyze_READY = 1;

#include "VISUALIZATIONS.h"

//________________________________________________________________________________
//
// FAKE86 EMU SECTION
//________________________________________________________________________________
uint8_t drawready = 1;

#define CPU_V20 /// !!!

#define RAM_MEM_SIZE 0x100000 //1MB
#define VRAM_SIZE 256000 //250kB
#define PORTS_MEM_SIZE 0x10000 //64kB

#define MAX_PIXEL_WIDTH ((uint16_t) 640)
#define MAX_PIXEL_HEIGHT ((uint16_t) 400)
#define MAX_PIXELS_CNT (LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT)

uint32_t MAX_FILE_SIZE_PSRAM = 1474560; // = max. 1440kB for DISKETTE A: DRIVE
uint32_t MAX_FILE_SIZE_HIMEM = 2949120; // = max. 2880kB for DISKETTE B: DRIVE

uint8_t DISKETTE_DRIVE_A_INSERTED = 0;
uint8_t DISKETTE_DRIVE_B_INSERTED = 0;

uint8_t * RAM;
uint8_t * VRAM;
uint8_t * portram;

typedef struct portcallback_t {
  void * (portout_callback[PORTS_MEM_SIZE]);
  void * (portin_callback[PORTS_MEM_SIZE]);
};

portcallback_t * portcallback_; //PORTS

#include "BIOS.h"

#include "FAKE86.h" /// MAIN EMULATOR FILE
//________________________________________________________________________________
//
// KMSWITCH SECTION
//________________________________________________________________________________
//________________________________________________________________________________
//
// OSCILLOSCOPE SECTION
//________________________________________________________________________________
#include "OSCILLOSCOPE.h"
//________________________________________________________________________________
//
// SIGNAL GENERATOR SECTION
//________________________________________________________________________________


//██████████████████████████████████████████████████
//█  USB TASK
//██████████████████████████████████████████████████

uint32_t USBTASKTIME = 0;
uint8_t USB_TASK_BUSY = 0;
//--------------------------------------------------------------------------------

IRAM_ATTR
void USBTASK_() {
#if USBHOST
#if MICROSD
///#if USBHOST_STORAGE
/*
  if (digitalRead(SD_CS_PIN) == 0 || USBSTORAGE_BUSY >= 2) { //can not run Usb.Task when SD card busy or running USB mass storage
    ///vTaskDelay___(1); //can not delay there
    return;
  }
  //THIS IS NOT GOOD IDEA... ahhh this broke USB streams... ?
*/  
  if (USBSTORAGE_BUSY >= 2) { //can not run Usb.Task when SD card busy or running USB mass storage
    ///vTaskDelay___(1); //can not delay there
    return;
  }
///#endif    
#endif  
  //while (digitalRead(5)==0) {vTaskDelay___(1);} //SDCARD BUSY
  if (!USB_TASK_BUSY) {
    USB_TASK_BUSY = 1;
    Usb.Task();
    USBTASKTIME = millis();
    USB_TASK_BUSY = 0;
  } else if (USBTASKTIME + 1000 < millis()) {
    USB_TASK_BUSY = 1;
    Usb.Task();
    USBTASKTIME = millis();
    USB_TASK_BUSY = 0;
  } else {
    return;
  }
#endif
}

IRAM_ATTR
void USBTASK(void * arg) {
  while (1) {
    USBTASK_();
    vTaskDelay___(1);
  }
}


//██████████████████████████████████████████████████
//█  AUDIO TASKS
//██████████████████████████████████████████████████
//uint8_t AUDIOREADY = 0;
///uint32_t AUDIO_TIMER=0;
//--------------------------------------------------------------------------------
//void( * audio_callback)(void * buffer, int length) = NULL; //for NES
//void do_audio_frame(); //for NES
//void play_audio_frame(); //for NES
///int pcm_submit(); //for GBC
void audio_frame_call(void); //for SMS
void sound_mix(); //for GBC

IRAM_ATTR static inline void AUDIOTask() {


  if (AUDIOREADY == 1) {
    AUDIOREADY = 2;

    if (SOUND_ENABLED && audio_callback && POWER == APP_EMU_NES) { //NES AUDIO FRAME
#if NES_EMULATOR
      do_audio_frame();
      play_audio_frame();
#endif
    } else if (SOUND_ENABLED && POWER == APP_EMU_GBC) { //GBC AUDIO FRAME
      ///      sound_mix(); //in GBC can not multitask sound_mix(), it cause after time CRASH or FREEZE ... ahhh...
#if GBC_EMULATOR
      pcm_submit();
#endif
    } else if (SOUND_ENABLED && POWER == APP_EMU_SMS) { //SMS AUDIO FRAME
#if SMS_EMULATOR
      audio_frame_call();
#endif
    }



    AUDIOREADY = 0;
    AUDIO_TIMER = millis___();
  }
}
//--------------------------------------------------------------------------------


//██████████████████████████████████████████████████
//█  DRAW TASKS
//██████████████████████████████████████████████████

//--------------------------------------------------------------------------------
void SETPAL(uint16_t APP_PAL) {
  if (APP_PAL == APP_EMU_NONE || APP_PAL == APP_EMU_GBC)
    for (uint16_t tmp = 0; tmp < 256; tmp++) {
      adj_32bit[tmp] = default_24bit[tmp]; //load default palette for COMPOSITE VIDEO
    } else if (APP_PAL == APP_EMU_NES)
    for (uint16_t tmp = 0; tmp < 256; tmp++) {
      adj_32bit[tmp] = nes_24bit[tmp]; //load NES palette for COMPOSITE VIDEO
    }
}
//--------------------------------------------------------------------------------

//********************************************************************************
//*  DEFAULT DRAW FRAME                                                          *
//********************************************************************************
///IRAM_ATTR
static inline void lcd_write_frame_default() {
  uint16_t* dest;
  uint8_t* src;
  int16_t Y_POS = -1;
  uint16_t X_POS = 0;


#if DEBUG>5
  uint32_t DRAWTIME_METER = millis___();
#endif
  //................................................................................
  while (++Y_POS < LCD_HEIGHT) {
    dest = &SCREENBUFFER[0];
    src = SCREENMEMORY_LINE[Y_POS];
    X_POS = LCD_WIDTH;

    USBTASK_(); //try increase call rate...

    if (DRAWMODE == 4)
      ///    while (--X_POS) *dest++ = lcd.out.cgb_pal[*src++];

      while (--X_POS) {
        *dest++ = default_16bit[*src++];
      }
    else
      while (--X_POS) {
        *dest++ = default_16bit[*src++];
      }


    ///if (DRAWMODE==4) if (cpu__GBC.lcdc <= 0) lcdc_trans(); ///HACK

    if (DRAWMODE == 4) if (AUDIO_TIMER + 0 <= millis___()) AUDIOTask(); //in GBC we need do AUDIOTASK try every 1ms...
    //................................................................................
#if LCD_ENABLED
    tft.pushImage(0, Y_POS, 240, 1, (uint16_t *)(&SCREENBUFFER[0]), true);
#endif
  }
#if DEBUG>5
  printf("DRAWTIME: %d\n", (uint32_t)(millis___() - DRAWTIME_METER));
#endif
}
//********************************************************************************
//*  NES EMU DRAW FRAME                                                          *
//********************************************************************************
///IRAM_ATTR
static inline void lcd_write_frame_NES() {
  uint16_t* dest;
  uint8_t* src;
  int16_t Y_POS = 0; // + 1 vertical pixel for drawing (NES SCROLLING BUFFER FIX)
  uint16_t X_POS = 0;

#if DEBUG>53
  uint32_t DRAWTIME_METER = millis___();
#endif
  //................................................................................
  while (++Y_POS < LCD_HEIGHT) {

    AUDIOTask(); //in NES we need do AUDIOTASK more than every 25ms... (every 16ms)

    USBTASK_(); //try increase call rate...

    
    dest = &SCREENBUFFER[0];
    src = &SCREENMEMORY_LINE[Y_POS][8]; ///center NES
    X_POS = LCD_WIDTH;
    while (X_POS--) *dest++ = nes_16bit[*src++];

    ///AUDIOTask(); //in NES we need do AUDIOTASK more than every 25ms... (every 16ms)
    //................................................................................
#if LCD_ENABLED
    tft.pushImage(0, Y_POS, 240, 1, (uint16_t *)(&SCREENBUFFER[0]), true); // true = enable DMA

    //tft.pushImage(0, Y_POS, 240, 1, (uint16_t *)(&SCREENBUFFER[0]),true); //draw 1 LINE //this takes 24ms (~41.66fps) [but picture is stable]
#endif
  }
#if DEBUG>5
  printf("DRAWTIME: %d\n", (uint32_t)(millis___() - DRAWTIME_METER));
#endif
}
//********************************************************************************
//*  SMS EMU DRAW FRAME                                                      *
//********************************************************************************
//IRAM_ATTR
static inline void lcd_write_frame_SMSGG() {
#if SMS_EMULATOR

  if (!bitmap.data) return;

  //SMS emulator: PALETTE IS CHANGING IN TIME...!
  if (bitmap.pal.update) {
    uint8_t SMS_RED;
    uint8_t SMS_GREEN;
    uint8_t SMS_BLUE;
    uint32_t SMS_COLOR32bit;

    bitmap.pal.update = 0;

    for (uint8_t tmp = 0; tmp < 32; tmp++) {
      SMS_RED = bitmap.pal.color[tmp][0];
      SMS_GREEN = bitmap.pal.color[tmp][1];
      SMS_BLUE = bitmap.pal.color[tmp][2];

      if (SMS_RED == 0 && SMS_GREEN == 0 && SMS_BLUE == 0) {
        SMS_RED = 0x2F;
        SMS_GREEN = 0x2F;
        SMS_BLUE = 0x2F;
      }

      if (SMS_RED == 0b11000000) SMS_RED = 0xFF;
      if (SMS_GREEN == 0b11000000) SMS_GREEN = 0xFF;
      if (SMS_BLUE == 0b11000000) SMS_BLUE = 0xFF;

      if (SMS_RED == 0b10000000) SMS_RED = 0xAF;
      if (SMS_GREEN == 0b10000000) SMS_GREEN = 0xAF;
      if (SMS_BLUE == 0b10000000) SMS_BLUE = 0xAF;

      if (SMS_RED == 0b01000000) SMS_RED = 0x5F;
      if (SMS_GREEN == 0b01000000) SMS_GREEN = 0x5F;
      if (SMS_BLUE == 0b01000000) SMS_BLUE = 0x5F;

      SMS_COLOR32bit = ((((uint8_t) SMS_GREEN << 24) | ((uint8_t) SMS_RED << 16) | ((uint8_t) SMS_BLUE << 8)) & 0xffffff00); ///00GGGGGG 00YYYYYY 00RRRRRR 00BBBBBB

      //save to Composite TV out PALETTE
      adj_32bit[tmp] = SMS_COLOR32bit;
      adj_32bit[tmp + 32] = SMS_COLOR32bit;
    }
  }
  //----------------------------------------------------------------------------------------
  if (cart.type == TYPE_SMS ) {
    uint16_t* dest;
    uint8_t* src;
    uint16_t Y_POS = 0;
    uint16_t X_POS = 0;

    //LCD draw function
    uint8_t rgb[3];
    Y_POS = 0;
    X_POS = 0;

    for (Y_POS = 0; Y_POS < 240; Y_POS++) {

      AUDIOTask(); //in SMS we need do AUDIOTASK more than every 25ms... (every 16ms)

      USBTASK_(); //try increase call rate...

//................................................................................
#if VGA_OUTPUT
      volatile uint8_t *VGA_PTR = VGA.m_viewPort[Y_POS]; //must be aligned in 4 bytes
      VGA_PTR+= ((320-256)/2);
#else
      if (Y_POS % 4 == 0) dest = SCREENBUFFER;
#endif      
//................................................................................

      if (Y_POS < ((240 - 192) / 2) || Y_POS > ((240 - 192) / 2 + 192 - 3)) {
//................................................................................
#if VGA_OUTPUT
#else 
        for (X_POS = 0; X_POS < 256; X_POS++) ///SCREENBUFFER[X_POS] = 0; ///memory correction
          *dest++ = 0;          
#endif
      } else
        for (X_POS = 0; X_POS < 256; X_POS++) { //not 256 the 240
          rgb[0] = bitmap.pal.color[bitmap.data[((Y_POS - ((240 - 192) / 2)) + 0)][(X_POS + 0) + (0)] & PIXEL_MASK][0];
          rgb[1] = bitmap.pal.color[bitmap.data[((Y_POS - ((240 - 192) / 2)) + 0)][(X_POS + 0) + (0)] & PIXEL_MASK][1];
          rgb[2] = bitmap.pal.color[bitmap.data[((Y_POS - ((240 - 192) / 2)) + 0)][(X_POS + 0) + (0)] & PIXEL_MASK][2];

//................................................................................
#if VGA_OUTPUT
  uint8_t c = (((rgb[1] >> 6) << 6) | ((rgb[0] >> 5 ) << 3) | ((rgb[2] >> 5) << 0)); //GreenRedBlue
  if (VGA_INITED==1) {
    if (Y_POS>=0 && Y_POS < LCD_HEIGHT-1 && X_POS>=8 && X_POS < SCREEN_WIDTH - 4 && DRAWMODE!=2) {
    /*  if (X_POS%4==0) VGA.m_viewPort[Y_POS][X_POS+2] = 0b11000000 | default_6bit[c];
      if (X_POS%4==1) VGA.m_viewPort[Y_POS][X_POS+2] = 0b11000000 | default_6bit[c];
      if (X_POS%4==2) VGA.m_viewPort[Y_POS][X_POS-2] = 0b11000000 | default_6bit[c];
      if (X_POS%4==3) VGA.m_viewPort[Y_POS][X_POS-2] = 0b11000000 | default_6bit[c]; */

      if (X_POS%4==0) {
        VGA_PTR+=2;
        *VGA_PTR = 0b11000000 | default_6bit[c];
      } else if (X_POS%4==1) { 
        VGA_PTR++;
        *VGA_PTR = 0b11000000 | default_6bit[c];
      } else if (X_POS%4==2) {
        VGA_PTR-=3;
        *VGA_PTR = 0b11000000 | default_6bit[c];
      } else if (X_POS%4==3) {
        VGA_PTR++;
        *VGA_PTR = 0b11000000 | default_6bit[c]; 
        VGA_PTR+=3;
      }
      



      
    }
  }
//................................................................................
#else
          uint16_t col = ((rgb[2] >> 3) << 8) + ((rgb[0] >> 3) << 3) + ((rgb[1] >> 5) << 0); ///0bGGBBBBBRRRRRGGG
          if (X_POS >= ((256 - 240) / 2) && X_POS < (240 + (256 - 240) / 2)) * dest++ = col;
#endif
//................................................................................
        }
#if LCD_ENABLED
      if (Y_POS % 4 == 3) tft.pushImage(0, Y_POS - 3, 240, 4, (uint16_t * )( &SCREENBUFFER), true); //draw 1 LINE
#endif
    }
    //--------------------------------------------------------------------------------
  }


  else if (cart.type == TYPE_GG) { //GG screen is 160*144 with an offset of +48(X_POS) +24(Y)
    uint16_t* dest;
    uint8_t* src;
    uint16_t Y_POS = 0;
    uint16_t X_POS = 0;

    //LCD draw function
    uint8_t rgb[3];
    Y_POS = 0;
    X_POS = 0;

    for (Y_POS = 0; Y_POS < 240; Y_POS++) {

      AUDIOTask(); //in SMS we need do AUDIOTASK more than every 25ms... (every 16ms)

      USBTASK_(); //try increase call rate...

//................................................................................
#if VGA_OUTPUT
      volatile uint8_t *VGA_PTR = VGA.m_viewPort[Y_POS]; //must be aligned in 4 bytes
      VGA_PTR+= ((320-256)/2);
#else
      if (Y_POS % 4 == 0) dest = SCREENBUFFER;
#endif      
//................................................................................

//................................................................................
      if (Y_POS > 240 - 12) {
#if VGA_OUTPUT
#else 
        for (X_POS = 0; X_POS < 256; X_POS++) ///SCREENBUFFER[X_POS] = 0; ///memory correction
          *dest++ = 0;
#endif      
      } else
        for (X_POS = 0; X_POS < 256; X_POS++) { //not 256 the 240
          rgb[0] = bitmap.pal.color[SCREENMEMORY_LINE[(Y_POS )][(X_POS + 0) + (0)] & PIXEL_MASK][0];
          rgb[1] = bitmap.pal.color[SCREENMEMORY_LINE[(Y_POS )][(X_POS + 0) + (0)] & PIXEL_MASK][1];
          rgb[2] = bitmap.pal.color[SCREENMEMORY_LINE[(Y_POS )][(X_POS + 0) + (0)] & PIXEL_MASK][2];

          uint16_t col = ((rgb[2] >> 3) << 8) + ((rgb[0] >> 3) << 3) + ((rgb[1] >> 5) << 0); ///0bGGBBBBBRRRRRGGG

//................................................................................
#if VGA_OUTPUT
  uint8_t c = (((rgb[1] >> 6) << 6) | ((rgb[0] >> 5 ) << 3) | ((rgb[2] >> 5) << 0)); //GreenRedBlue
  if (VGA_INITED==1) {
    if (Y_POS>=0 && Y_POS < LCD_HEIGHT-1 && X_POS>=0 && X_POS < SCREEN_WIDTH - 4 && X_POS<256-16 &&  DRAWMODE!=2) {
    /*  if (X_POS%4==0) VGA.m_viewPort[Y_POS][X_POS+2] = 0b11000000 | default_6bit[c];
      if (X_POS%4==1) VGA.m_viewPort[Y_POS][X_POS+2] = 0b11000000 | default_6bit[c];
      if (X_POS%4==2) VGA.m_viewPort[Y_POS][X_POS-2] = 0b11000000 | default_6bit[c];
      if (X_POS%4==3) VGA.m_viewPort[Y_POS][X_POS-2] = 0b11000000 | default_6bit[c]; */

      if (X_POS%4==0) {
        VGA_PTR+=2;
        *VGA_PTR = 0b11000000 | default_6bit[c];
      } else if (X_POS%4==1) { 
        VGA_PTR++;
        *VGA_PTR = 0b11000000 | default_6bit[c];
      } else if (X_POS%4==2) {
        VGA_PTR-=3;
        *VGA_PTR = 0b11000000 | default_6bit[c];
      } else if (X_POS%4==3) {
        VGA_PTR++;
        *VGA_PTR = 0b11000000 | default_6bit[c]; 
        VGA_PTR+=3;
      }
      



      
    }
  }
//................................................................................
#else
          if (X_POS >= 0 && X_POS < 240) *dest++ = col;
#endif          
        }
#if LCD_ENABLED
      if (Y_POS % 4 == 3) tft.pushImage(0, Y_POS - 3, 240, 4, (uint16_t * )( &SCREENBUFFER), true); //draw 1 LINE
#endif
    }

  }


















  /*else if (cart.type == TYPE_GG) { //GG screen is 160*144 with an offset of +48(X_POS) +24(Y)
    uint16_t* dest;
    uint8_t* src;
    uint16_t Y_POS=0;
    uint16_t X_POS=0;

    uint32_t pal[2][32];
    for (int X_POS = 0; X_POS < 32; X_POS++) {
      pal[0][X_POS] = ((bitmap.pal.color[X_POS][0] >> 3) << 3) + ((bitmap.pal.color[X_POS][1] >> 5) << 0);
      pal[1][X_POS] = ((bitmap.pal.color[X_POS][1] >> 5) << 0) + ((bitmap.pal.color[X_POS][2] >> 3) << 8);
    }
    for (int Y_POS = 0 - 24; Y_POS < 240 - 24; Y_POS += 2) {

      AUDIOTask(); //in GG we need do AUDIOTASK more than every 25ms... (every 16ms)

      if (Y_POS >= 24 && Y_POS < 144 + 24) {
        for (int X_POS = 8; X_POS < 248; X_POS += 2) {
          if (X_POS >= 48 && X_POS < 160 + 40) {
            uint32_t c1 = pal[0][bitmap.data[(Y_POS)][(X_POS) + (0)] & PIXEL_MASK];
            uint32_t c2 = pal[1][bitmap.data[(Y_POS)][(X_POS + 1) + (0)] & PIXEL_MASK];
            uint32_t c3 = pal[0][bitmap.data[(Y_POS + 1)][(X_POS) + (0)] & PIXEL_MASK];
            uint32_t c4 = pal[1][bitmap.data[(Y_POS + 1)][(X_POS + 1) + (0)] & PIXEL_MASK];

            SCREENBUFFER[X_POS - 8] = (c1);
            SCREENBUFFER[X_POS + 1 - 8] = (c2);
            SCREENBUFFER[X_POS - 8 + 240] = (c3);
            SCREENBUFFER[X_POS + 1 - 8 + 240] = (c4);
          } else {
            SCREENBUFFER[X_POS - 8] = 0;
            SCREENBUFFER[X_POS + 1 - 8] = 0;
            SCREENBUFFER[X_POS - 8 + 240] = 0;
            SCREENBUFFER[X_POS + 1 - 8 + 240] = 0;
          }
        }
        #if LCD_ENABLED
          tft.pushImage(0, Y_POS - 24 + 48, 240, 2, (uint16_t * )(SCREENBUFFER));
        #endif
      } else {
        for (int X_POS = 0; X_POS < 240; X_POS++) SCREENBUFFER[X_POS] = 0;
        #if LCD_ENABLED
          tft.pushImage(0, Y_POS - 24 + 48 + 0, 240, 1, (uint16_t * )( &SCREENBUFFER),true); //draw 1 LINE
          tft.pushImage(0, Y_POS - 24 + 48 + 1, 240, 1, (uint16_t * )( &SCREENBUFFER),true); //draw 1 LINE
        #endif
      }
    }
    }*/
#endif
}


/*#if VGA_OUTPUT
  uint8_t VGA_SCANLINE_BUF[320];
#endif*/
//................................................................................
uint8_t VGA_TASK() {
#if VGA_OUTPUT

     
  int16_t Y_POS=0;
  int16_t X_POS=0;
  while (Y_POS < LCD_HEIGHT-2) {
    X_POS = 0;


    if (DRAWMODE != 2 && DRAWMODE != 4) {
      AUDIOTask();

      USBTASK_(); //try increase call rate...



      volatile uint8_t *VGA_PTR = VGA.m_viewPort[Y_POS];
      VGA_PTR+=VGA_OFFSET;
      
      while (X_POS < LCD_WIDTH-4) {
        if (DRAWMODE == 2) { //NES
/*
           VGA.m_viewPort[Y_POS][X_POS+3]=0b11000000 | nes_6bit[SCREENMEMORY_LINE[Y_POS*12/10][X_POS+1]];
           VGA.m_viewPort[Y_POS][X_POS+2]=0b11000000 | nes_6bit[SCREENMEMORY_LINE[Y_POS*12/10][X_POS+0]];
           VGA.m_viewPort[Y_POS][X_POS+1]=0b11000000 | nes_6bit[SCREENMEMORY_LINE[Y_POS*12/10][X_POS+3]];
           VGA.m_viewPort[Y_POS][X_POS+0]=0b11000000 | nes_6bit[SCREENMEMORY_LINE[Y_POS*12/10][X_POS+2]];
*/
 
           ///VGA.m_viewPort[Y_POS][X_POS]=0b11000000 | nes_6bit[SCREENMEMORY_LINE[Y_POS*12/10][X_POS]];
         } else { //DEFAULT
           ///VGA.setRawPixel(X_POS, Y_POS,(VGA.createBlankRawPixel() | default_6bit[SCREENMEMORY_LINE[Y_POS*12/10][X_POS]] )); //0b00BBGGRR

           //FASTER 
          *VGA_PTR++=0b11000000 | default_6bit[SCREENMEMORY_LINE[Y_POS][X_POS+2]];
          *VGA_PTR++=0b11000000 | default_6bit[SCREENMEMORY_LINE[Y_POS][X_POS+3]];
          *VGA_PTR++=0b11000000 | default_6bit[SCREENMEMORY_LINE[Y_POS][X_POS+0]];
          *VGA_PTR++=0b11000000 | default_6bit[SCREENMEMORY_LINE[Y_POS][X_POS+1]];

          ///VGA_PIXELINROW(VGA.m_viewPort[Y_POS], X_POS) = VGA.packHVSync(false, false) | default_6bit[SCREENMEMORY_LINE[Y_POS*12/10][X_POS]];
          ///VGA_PIXELINROW(VGA.m_viewPort[120], 10) = VGA.preparePixelWithSync((fabgl::RGB222){0x3, 0x3, 0}, false, false);
        }

        X_POS+=4; /// 
        
      }
      Y_POS++;
    }
  }
#endif  
}
//................................................................................



//██████████████████████████████████████████████████
//█  SYSTEM TASK
//██████████████████████████████████████████████████

///uint32_t SYSTEM_TIMER=0;

IRAM_ATTR
void SYSTEM_TASK() {


  USBTASK_();

  if (AUDIO_TIMER + 0 <= millis___()) { //in GBC we need do AUDIOTASK try every 1ms...
    AUDIOTask();
  }
  
  DRAWREADY = 0;

  ///  vPortYield(); ///

  ///  if (SYSTEM_TIMER + 0 < millis___()) {

  if (POWER == APP_AUD_PLAYER) {
    visualyze(); //visualyze when playing audio... must be there
  }
  if (POWER == APP_EMU_FAKE86) {
#if FAKE86_EMULATOR
    Fake86Video(); //FAKE86 DRAW
#endif
  }


#if VGA_OUTPUT
  /*if (VGA_READY && DRAWMODE != 1 && DRAWMODE == 2 && DRAWMODE != 3 && DRAWMODE != 4) {
    VGA_TASK();
    VGA_READY=0;
  } else if (DRAWMODE != 1 && DRAWMODE != 2 && DRAWMODE != 3 && DRAWMODE != 4) {
    VGA_TASK();    
  }
  */
  
  if (DRAWMODE == 0) VGA_TASK();
  else if (DRAWMODE == 3) lcd_write_frame_SMSGG(); // SMS DRAW MODE + AUDIO
#else


  //DRAW and AUDIO tasks
  if (DRAWMODE == 1) lcd_write_frame_default(); // DEFAULT DRAW MODE & NO AUDIO
  else if (DRAWMODE == 2) lcd_write_frame_NES(); // NES DRAW MODE + AUDIO
  else if (DRAWMODE == 3) lcd_write_frame_SMSGG(); // SMS DRAW MODE + AUDIO
  else if (DRAWMODE == 4) lcd_write_frame_default(); // GBC DRAW MODE + AUDIO

  /*else if (DRAWMODE == 4 && fb.enabled && POWER == APP_EMU_GBC) lcd_write_frame_GBC(); // GBC DRAW MODE + AUDIO
    else if (DRAWMODE == 4 && !fb.enabled && POWER == APP_EMU_GBC) { //GBC ONLY AUDIO
    if (AUDIOREADY == 1) {
      AUDIOREADY = 2;
      AUDIOTask();
      AUDIOREADY = 0;
    }*/

  ///      SYSTEM_TIMER = millis___();
  ///}

#endif 

  DRAWREADY = 1;
}

IRAM_ATTR void SYSTEMTASK(void * arg) {
  while (1) {
    if (!xSemaphoreTake(draw_mutex, 1000)) { //10ms wait for draw_mutex
      printf("DRAW: waiting for frame\n");
      continue;
    }
    SYSTEM_TASK();
  }
}




//██████████████████████████████████████████████████
//█  CORE
//██████████████████████████████████████████████████

#include "CORE.h"


//██████████████████████████████████████████████████
//█  SETUP
//██████████████████████████████████████████████████

void setup() {

  MULTITASK_MODE = 0; //0=REDUCED 1=MULTITASKING (glitchy)

  //--------------------------------------------------------------------------------
  // EEPROM LOAD
  //--------------------------------------------------------------------------------

#if SETTINGS_OPTIONS
  EEPROM_load_settings();
  RESET_SETTINGS = settings_set[0];
  Clock_Enabled = settings_set[1];
  Sound_Enabled = settings_set[2];
  Force_Bluetooth = settings_set[3];
  Composite_Enabled = settings_set[4];
  Wallpaper = settings_set[5];
  MicroSD_Required = settings_set[6];
  USB_Host_Required = settings_set[7];
  Clock_Color = settings_set[8];
  InGame_Menu = settings_set[9];
  Overclock_level = settings_set[10];
  LCD_rotation = settings_set[11];
  Extra_apps = settings_set[12];
  Bluetooth_default_security_level = settings_set[13];
  Multiplayer_dev_input_support = settings_set[14];
  USBhost_joysticks = settings_set[15];
  USBhost_keyboard_and_mouse = settings_set[16];
  USBhost_mass_storage = settings_set[17];
  USBhost_bluetooth_PS4joysticks = settings_set[18];
  LCD_off_time = settings_set[19];
  
  if (Overclock_level == 11) OVERCLOCK = 1;
  else if (Overclock_level == 12) OVERCLOCK = 2;
  else if (Overclock_level == 13) OVERCLOCK = 3;
  else if (Overclock_level == 14) OVERCLOCK = 4;
  else OVERCLOCK = 0;
#endif

  //--------------------------------------------------------------------------------
  // CPU INIT
  //--------------------------------------------------------------------------------

#if OVERCLOCK_ALLOWED

  if (OVERCLOCK == 1) TIMING = 1.290; //=SQRT(5/3)
  else if (OVERCLOCK == 2) TIMING = 1.41421356; //=SQRT(2);
  else if (OVERCLOCK == 3) TIMING = 1.447; //=5/3
  else if (OVERCLOCK == 4) TIMING = 1.54044011; //=?
  else if (OVERCLOCK == 5) TIMING = 1.66666667; //=5/3
  else TIMING = 1;

  Serial.begin(115200 / TIMING);
  delay___(100);

  if (OVERCLOCK) {
    Force_Bluetooth = 1; // = DISABLED
  } else {
    //setCpuFrequencyMhz(240);
  }

  if (OVERCLOCK) OVERCLOCK_ME(OVERCLOCK);
  //UNDERVOLT_ME();

#endif

  printf("STARTING NCAT = MEOW :D\n");

  uint32_t CPU_FREQ = ESP.getCpuFreqMHz();
  uint32_t OC_CPU_FREQ = CPU_FREQ * TIMING;

  printf("[!] OVERCLOCK LEVEL: %d\n", OVERCLOCK);
  printf("[!] CPU FREQUENCY ~%dMHz\n", OC_CPU_FREQ);
  delay___(100);


//--------------------------------------------------------------------------------
/*for (uint16_t tmp=0;tmp<256;tmp++) {
  if (tmp%16==0) printf("\n");
  //red green blue
  uint32_t COLOR=( ((nes_24bit[tmp] & 0x00C00000) >> (16+6)) | ((nes_24bit[tmp] & 0xC0000000) >> (24+6-2)) | ((nes_24bit[tmp] & 0x0000C000) >> (8+6-4)) );
  printf("0x%02X, ", COLOR);
}

 printf("\n");  */
//--------------------------------------------------------------------------------
  
//--------------------------------------------------------------------------------
// PSRAM INIT
//--------------------------------------------------------------------------------
#ifndef NCAT_NO_PSRAM


  //esp_spiram_init(); ///Important in some cases!
  if (ESP.getPsramSize() > 0) {
    PSRAM_BASE_SIZE = ESP.getPsramSize();
#if HIMEM
    esp_himem_init();

    printf("Allocating PSRAM HIMEM RAM...");
    PSRAM_HIMEM_SIZE = esp_himem_get_free_size();
    himem_malloc(PSRAM_HIMEM_SIZE);
    printf("[ OK ]\n");
#else
    PSRAM_HIMEM_SIZE = 0;
#endif
  } else {
    PSRAM_BASE_SIZE = 0; //No PSRAM...
    printf("NO PSRAM ENABLED !!! \n");

    while (1) {} //halt
  }

#endif

  //--------------------------------------------------------------------------------
  // SYSTEM VARIABLES INIT
  //--------------------------------------------------------------------------------

  SETPAL(0); ///set default palette

  //rtc_wdt_protect_off();
  //rtc_wdt_disable();

  disableLoopWDT();
  disableCore0WDT(); //Disable Watchdog on CORE 0 (I think it is not good idea... maybe cause blutooth)

#if BLUETOOTH
/*  printf("Releasing ESP_BT_MODE_BLE for free memory...");
  //we are not using BLE... so release it
  esp_bt_controller_mem_release(ESP_BT_MODE_BLE); //ESP_BT_MODE_CLASSIC_BT cannot be released!
  printf("[ OK ]\n");*/
#endif

  MEMORY_STATUS();

#if VGA_OUTPUT
  printf("Allocating VIDEO memory 82kB PSRAM...");
  SCREENMEMORY = (uint8_t * ) ps_malloc(SCREEN_WIDTH * SCREEN_HEIGHT);
#else
  printf("Allocating VIDEO memory 64kB RAM...");
  SCREENMEMORY = (uint8_t * ) heap_caps_malloc(SCREEN_WIDTH * SCREEN_HEIGHT, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
#endif
  
  memset(SCREENMEMORY, 0, (SCREEN_WIDTH * SCREEN_HEIGHT)); //info: NTSC lines is 262
  for (uint32_t tmp = 0; tmp < SCREEN_HEIGHT; tmp++) { //Internal memory is faster than PSRAM
    SCREENMEMORY_LINE[tmp] = &SCREENMEMORY[tmp * SCREEN_WIDTH];
  }
  printf("[ OK ]\n");

  set_font(font_Ishmeria8x16); //Very important

#ifndef NCAT_NO_PSRAM
  printf("Allocating BACKGROUND buffer ~112kB PSRAM...");
  BACKGROUND_BUFFER = (uint16_t * ) ps_malloc(BACKGROUND_BUFFER_SIZE);
  printf("[ OK ]\n");
  
#if JPG_DECODER
  printf("Allocating JPG Decoding buffer ~4kB PSRAM...");
  JPGOUTPUT_BUFFER = (uint8_t * ) ps_malloc(JPG_BUFFER_SIZE);
  printf("[ OK ]\n");
#else
  memset(BACKGROUND_BUFFER, 0, BACKGROUND_BUFFER_SIZE); //black background
#endif

  TEXTBUFFER = (char * ) ps_malloc(256);
  MAINPATH = (char * ) ps_malloc(256);
  DATAPATH1 = (char * ) ps_malloc(256);
  DATAPATH2 = (char * ) ps_malloc(256);
  TRACKNAME = (char * ) ps_malloc(256);
#else
  TEXTBUFFER = (char * ) malloc(64);
  MAINPATH = (char * ) malloc(64);
  DATAPATH1 = (char * ) malloc(64);
  DATAPATH2 = (char * ) malloc(64);
  TRACKNAME = (char * ) malloc(64);
#endif


#ifndef NCAT_NO_PSRAM
  USBKEYBOARD = (uint8_t* ) ps_malloc(256);
  memset(USBKEYBOARD, 0, 256);
  BTKEYBOARD = (uint8_t* ) ps_malloc(256);
  memset(BTKEYBOARD, 0, 256);
  PS2KEYBOARD = (uint8_t* ) ps_malloc(256);
  memset(PS2KEYBOARD, 0, 256);
#else
  USBKEYBOARD = (uint8_t* ) malloc(256);
  memset(USBKEYBOARD, 0, 256);
  BTKEYBOARD = (uint8_t* ) malloc(256);
  memset(BTKEYBOARD, 0, 256);
  PS2KEYBOARD = (uint8_t* ) malloc(256);
  memset(PS2KEYBOARD, 0, 256);
#endif

  /*PS2_keymap=(uint8_t*)ps_malloc(256);
  memset(PS2_keymap, 0, sizeof(256));*/
  
#ifndef NCAT_NO_PSRAM
  keymap = (byte * ) ps_malloc(256); ///malloc array ! (can be used ps_malloc?)
  
  for (uint32_t tmp = 0; tmp < 256; tmp++) keymap[tmp] = 0;
  prevkeytime = (uint32_t * ) ps_malloc(256 * sizeof(uint32_t));
  for (uint32_t tmp = 0; tmp < 256; tmp++) {
    prevkeytime[tmp] = 0;
  }
#else
  keymap = (byte * ) malloc(256); ///malloc array ! (can be used ps_malloc?)
  
  for (uint32_t tmp = 0; tmp < 256; tmp++) keymap[tmp] = 0;
  prevkeytime = (uint32_t * ) malloc(256 * sizeof(uint32_t));
  for (uint32_t tmp = 0; tmp < 256; tmp++) {
    prevkeytime[tmp] = 0;
  }  
#endif

  
  draw_mutex = xSemaphoreCreateMutex();
  xSemaphoreTake(draw_mutex, 0);

  audio_mutex = xSemaphoreCreateMutex();
  xSemaphoreTake(audio_mutex, 0);

  //--------------------------------------------------------------------------------
  // INPUT INIT
  //--------------------------------------------------------------------------------

#ifdef NCAT_BOARD
  pinMode(39, INPUT);
#endif
#if LCD_ENABLED
#if defined(NCAT_BOARD) || defined(NCAT_LITE)
  //pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
#endif
#endif

#if(GPIO_BUTTONS == 1)
  //GPIO Buttons Pins Input Init
  pinMode(DIGITAL_BUTTON_IN, INPUT);
  pinMode(DIGITAL_BUTTON_CALIBRATION, INPUT);
  pinMode(DIGITAL_BUTTON_SWITCH, OUTPUT);
  //adc1_config_width(ADC_WIDTH_BIT_12);
  //adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11); //GPIO35
#endif
#if(GPIO_BUTTONS == 2)
  //Buttons Pins Input Init
  pinMode(PIN_BUS1, INPUT);
  pinMode(PIN_BUS2, INPUT);
  
  pinMode(PIN_UP_SELECT, OUTPUT);
  pinMode(PIN_DOWN_A, OUTPUT);
  pinMode(PIN_RIGHT_START, OUTPUT);
  pinMode(PIN_LEFT_B, OUTPUT);
  digitalWrite(PIN_UP_SELECT, 0);
  digitalWrite(PIN_DOWN_A, 0);
  digitalWrite(PIN_RIGHT_START, 0);
  digitalWrite(PIN_LEFT_B, 0);
  pinMode(PIN_UP_SELECT, INPUT);
  pinMode(PIN_DOWN_A, INPUT);
  pinMode(PIN_RIGHT_START, INPUT);
  pinMode(PIN_LEFT_B, INPUT);
#endif
#if(GPIO_BUTTONS == 3)
   //Buttons Pins Input Init
   pinMode(PIN_A, INPUT);    //A
   pinMode(PIN_B, INPUT);   //B
   pinMode(PIN_SELECT, INPUT);   //SELECT
   pinMode(PIN_START, INPUT);   //START
   pinMode(PIN_UP, INPUT);    //UP
   pinMode(PIN_DOWN, INPUT);   //DOWN  //TCK
   pinMode(PIN_LEFT, INPUT);       //LEFT
   pinMode(PIN_RIGHT, INPUT_PULLDOWN);  //RIGHT
#endif

  PS2_kb_begin(); //Inits PS2 (USB) keyboard when ENABLED...


  //--------------------------------------------------------------------------------
  // TWATCH SETUP
  //--------------------------------------------------------------------------------
#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();

  //ttgo->tft->initDMA(); //VERY IMPORTANT FOR LCD PERFORMANCE 25ms/40fps draw instead of 35ms/28.5fps
  ttgo->enableLDO3();

  //ttgo->rtc;
  //ttgo->enableAudio();
  //ttgo->tft->setTextFont(2);
  //ttgo ->tft->setTextColor(TFT_WHITE, TFT_BLACK);

#if defined(NCAT_TWATCH_V1)
  //Turn off unused power
  ///ttgo -> power -> setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
  //ttgo->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
  ///ttgo -> power -> setPowerOutPut(AXP202_LDO4, AXP202_OFF);

  ttgo -> power -> setPowerOutPut(AXP202_EXTEN, AXP202_ON);
  ttgo -> power -> setPowerOutPut(AXP202_LDO4, AXP202_ON);
  ttgo -> power -> setPowerOutPut(AXP202_DCDC2, AXP202_ON);
  ttgo -> power -> setPowerOutPut(AXP202_LDO3, AXP202_ON);
  ttgo -> power -> setPowerOutPut(AXP202_LDO2, AXP202_ON);

#else
  //Turn on required power
  ttgo -> power -> setPowerOutPut(AXP202_EXTEN, AXP202_ON);
  ttgo -> power -> setPowerOutPut(AXP202_LDO4, AXP202_ON);
  ttgo -> power -> setPowerOutPut(AXP202_DCDC2, AXP202_ON);
  ttgo -> power -> setPowerOutPut(AXP202_LDO3, AXP202_ON);
  ttgo -> power -> setPowerOutPut(AXP202_LDO2, AXP202_ON);
#endif

  ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);

  pinMode(BMA423_INT1, INPUT);
  attachInterrupt(BMA423_INT1, [] {
    TWATCH_irq = 1;
  }, RISING);

  ttgo->bma->begin();
  ttgo->bma->attachInterrupt();

  pinMode(AXP202_INT, INPUT_PULLUP);
  attachInterrupt(AXP202_INT, [] {
    TWATCH_irq = true;
  }, FALLING);

  //!Clear IRQ unprocessed  first
  ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_PEK_LONGPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ, true);
  ttgo->power->clearIRQ();

#endif

  //--------------------------------------------------------------------------------
  // LCD AND COMPOSITE VIDEO INIT
  //--------------------------------------------------------------------------------
#if defined(NCAT_BOARD)
  pinMode(2, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  digitalWrite(2, 0);
  digitalWrite(12, 1);
  digitalWrite(13, 1);
  digitalWrite(14, 1);
  digitalWrite(15, 1);
  delay(100);
  digitalWrite(2, 1);
  digitalWrite(12, 0);
  digitalWrite(13, 0);
  digitalWrite(14, 0);
  digitalWrite(15, 0);
  delay(100);
  digitalWrite(2, 0);
  digitalWrite(12, 1);
  digitalWrite(13, 1);
  digitalWrite(14, 1);
  digitalWrite(15, 1);
  delay(100);
  digitalWrite(2, 1);
  digitalWrite(12, 0);
  digitalWrite(13, 0);
  digitalWrite(14, 0);
  digitalWrite(15, 0);
  delay(100);
#endif

#if LCD_ENABLED

#if LCD_BACKLIGHT_CONTROL
  LCD_BACKLIGHT_PIN_SETUP(LCD_BACKLIGHT); //Set Backglight Level on SERVICE PIN
#endif

#if defined(NCAT_BOARD) || defined(NCAT_LITE) || defined(NCAT_LOW_POWER) || defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2) || defined(NCAT_MODULAR)  
  // INIT ST7789 LCD
  printf("Initializing ST7789 LCD...");

  auto cfg = _panel_instance.config();

#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
  cfg.pin_cs = 5;
  cfg.pin_rst = -1;
  cfg.pin_busy = -1;
  cfg.memory_width = 240;
  cfg.memory_height = 240;
  cfg.panel_width = 240;
  cfg.panel_height = 240;
  cfg.offset_x = 0;
  cfg.offset_y = -80;
  cfg.offset_rotation = 2;
  cfg.dummy_read_pixel = 16;
  cfg.dummy_read_bits = 1;
  cfg.readable = true;
  cfg.invert = true;
  cfg.rgb_order = false;
  cfg.dlen_16bit = false;
  cfg.bus_shared = true;

  if (LCD_rotation == 11) {
    cfg.offset_rotation = 3;
    cfg.offset_y = 0;
  } else if (LCD_rotation == 12) {
    cfg.offset_rotation = 0;
    cfg.offset_y = 0;
  } else if (LCD_rotation == 13) {
    cfg.offset_rotation = 1;
    cfg.offset_y = -80;
  } else {
    cfg.offset_rotation = 2;
    cfg.offset_y = -80;
  }
#elif defined(NCAT_MODULAR)  
  //NCAT LCD setup
  cfg.pin_cs = -1;
  cfg.pin_rst = 19;
  cfg.pin_busy = -1;
  cfg.memory_width = 240;
  cfg.memory_height = 240;
  cfg.panel_width = 240;
  cfg.panel_height = 240;
  cfg.offset_x = 0;
  cfg.offset_y = -0;
  cfg.offset_rotation = 0;
  cfg.dummy_read_pixel = 8;
  cfg.dummy_read_bits = 1;
  cfg.readable = false;
  cfg.invert = true;
  cfg.rgb_order = false;
  cfg.dlen_16bit = false;
  cfg.bus_shared = false;

  if (LCD_rotation == 11) {
    cfg.offset_rotation = 2;
    cfg.offset_y = -80;
  } else if (LCD_rotation == 12) {
    cfg.offset_rotation = 3;
    cfg.offset_y = -80;
  } else if (LCD_rotation == 13) {
    cfg.offset_rotation = 0;
    cfg.offset_y = 0;
  } else {
    cfg.offset_rotation = 1;
    cfg.offset_y = 0;
  } 
#elif defined(NCAT_LITE)  
  //NCAT LCD setup
  cfg.pin_cs = -1;
  cfg.pin_rst = 15;
  cfg.pin_busy = -1;
  cfg.memory_width = 240;
  cfg.memory_height = 240;
  cfg.panel_width = 240;
  cfg.panel_height = 240;
  cfg.offset_x = 0;
  cfg.offset_y = -0;
  cfg.offset_rotation = 0;
  cfg.dummy_read_pixel = 8;
  cfg.dummy_read_bits = 1;
  cfg.readable = false;
  cfg.invert = true;
  cfg.rgb_order = false;
  cfg.dlen_16bit = false;
  cfg.bus_shared = false;

  if (LCD_rotation == 11) {
    cfg.offset_rotation = 2;
    cfg.offset_y = -80;
  } else if (LCD_rotation == 12) {
    cfg.offset_rotation = 3;
    cfg.offset_y = -80;
  } else if (LCD_rotation == 13) {
    cfg.offset_rotation = 0;
    cfg.offset_y = 0;
  } else {
    cfg.offset_rotation = 1;
    cfg.offset_y = 0;
  } 
#else
  //NCAT LCD setup
  cfg.pin_cs = 15;
  cfg.pin_rst = -1;
  cfg.pin_busy = -1;
  cfg.memory_width = 240;
  cfg.memory_height = 240;
  cfg.panel_width = 240;
  cfg.panel_height = 240;
  cfg.offset_x = 0;
  cfg.offset_y = -0;
  cfg.offset_rotation = 0;
  cfg.dummy_read_pixel = 8;
  cfg.dummy_read_bits = 1;
  cfg.readable = false;
  cfg.invert = true;
  cfg.rgb_order = false;
  cfg.dlen_16bit = false;
  cfg.bus_shared = false;

  if (LCD_rotation == 11) {
    cfg.offset_rotation = 2;
    cfg.offset_y = -80;
  } else if (LCD_rotation == 12) {
    cfg.offset_rotation = 3;
    cfg.offset_y = -80;
  } else if (LCD_rotation == 13) {
    cfg.offset_rotation = 0;
    cfg.offset_y = 0;
  } else {
    cfg.offset_rotation = 1;
    cfg.offset_y = 0;
  }
#endif

  _panel_instance.config(cfg);

#if TOUCHPAD_ENABLED
  _panel_instance.setTouch( &_touch_instance);
#endif

  tft.setPanel(&_panel_instance);

  tft.init();

  //tft.init_without_reset();

  tft.initDMA(); //VERY IMPORTANT FOR LCD PERFORMANCE 25ms/40fps draw instead of 35ms/28.5fps

  tft.startWrite(); //TWATCH only one call this...




  printf("[ OK ]\n");
#endif
#endif


#if(COMPOSITE_VIDEO_ISR_CORE == 1 && COMPOSITE_VIDEO == 1)
  if (Composite_Enabled == 2) { //0==NOT SET, 1==DISABLED, 2=ENABLED
    printf("Initializing COMPOSITE VIDEO...");
    COMPOSITE_video_init(COMPOSITE_VIDEO_BYTE_WIDTH, adj_32bit, 1); // start the A/V pump on app on CORE1.
    printf("[ OK ]\n");
  }
#endif


#if VGA_OUTPUT
  printf("Initializing VGA VIDEO...");
  // (R1, R0, G1, G0, B1, B0, H-sync and V-sync)
  VGA.begin(GPIO_NUM_12, GPIO_NUM_2, GPIO_NUM_15, GPIO_NUM_14, GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_25, GPIO_NUM_4);
  ///VGA.begin();

  if (LCD_HEIGHT==200)
    VGA.setResolution(VGA_320x200_75Hz);
  else if (LCD_HEIGHT==240)  
    VGA.setResolution(QVGA_320x240_60Hz); //takes lot of memory
  else 
    VGA.setResolution(VGA_320x200_75Hz);
    

/// VGA.setResolution(VGA_320x200_60HzD); ///not working on my LCD...
/// VGA.setResolution(  VGA_320x200_75HzRetro);
/// VGA.setResolution( "\"320x200@70Hz\" 12.5875 320 328 376 400 200 206 207 224 -HSync -VSync");

  VGA_INITED=1;
  
  printf("[ OK ]\n");
#endif

//--------------------------------------------------------------------------------
#if NES_CONTROLLER
   printf("Initializing NES CONTROLLER...");
   //NES Controller init
   pinMode(NESCTRL_Data, INPUT);
   pinMode(NESCTRL_Clock, OUTPUT); 
   pinMode(NESCTRL_Latch, OUTPUT); //GPIO2 is pulled up...
   digitalWrite(NESCTRL_Clock, 0);
   digitalWrite(NESCTRL_Latch, 0);
#endif
//--------------------------------------------------------------------------------
/*while (1) {
INPUTREAD();
INPUTREPORT();
INPUTCLEAR();
}*/



  screenmemory_fillscreen(0);
  DRAWMODE = 0;
  screenmemory_fillscreen_static(0);
  INPUTCLEAR();

  //--------------------------------------------------------------------------------
  //  INTRO & FALLBACK RESET SETTINGS
  //--------------------------------------------------------------------------------
  screenmemory_fillscreen_static(0);


  if (INPUTREAD()) {
    if (INPUT_A || INPUT_SYSTEM) {
      INPUTCLEAR();
      screenmemory_fillscreen_static(0);
#if JPG_DECODER
      renderJPG_toBUFFER(NULL, (240 - 128) / 2, (240 - 128) / 2 - 32, JPG_WARNINGICON, sizeof(JPG_WARNINGICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
#endif
      set_font_XY(16, 200);
      draw_string_static((char*)"RESET ALL SETTINGS?", 0xffff);
      delay___(500); //wait 250ms to confirm RESET SETTINGS
      while (!INPUTREAD()) {
        delay___(10);
      }
      if (INPUT_A) {
        screenmemory_fillscreen_static(0);
        settings_set[0] = 1; //request reset settings set
        EEPROM_save_settings();
        ESP.restart();
      }
      INPUTCLEAR();
      screenmemory_fillscreen_static(0);
    }
  }
  INPUTCLEAR();

#if JPG_DECODER
  renderJPG_toBUFFER(NULL, (240 - 128) / 2, (240 - 128) / 2 - 32, JPG_NCATSYSICON, sizeof(JPG_NCATSYSICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
#endif

#ifndef NCAT_NO_PSRAM
  set_font_XY(32, 200);
#else
  set_font_XY(32, (240 - 16) /2);
#endif  
  
  set_font_XY((240 - 11*8) /2, 200); 
  draw_string_static((char*)"NCAT SYSTEM", 0xffff);
 
  delay___(1000);

  screenmemory_fillscreen_static(0);

  MENU_POS = 0; //activate MENU at position 0



//--------------------------------------------------------------------------------
// INIT USB HOST
//--------------------------------------------------------------------------------
#if USBHOST
  printf("Initializing USBHOST...");

  ///pinMode(USB_CS, OUTPUT);
  ///digitalWrite(USB_CS, 0);

#if USBHOST_STORAGE

  if (USBhost_mass_storage == 2) {
  for (int i = 0; i < _VOLUMES; i++) {
    Fats[i] = NULL;
    sto[i].private_data = new pvt_t;
    ((pvt_t * ) sto[i].private_data) -> B = 255; // impossible
  }
  // Set this to higher values to enable more debug information
  // minimum 0x00, maximum 0xff
  UsbDEBUGlvl = 0x81;

  // Initialize generic storage. This must be done before USB starts.
  Init_Generic_Storage();
  }
#endif


  if (Usb.Init(1000) == -1) {
#if JPG_DECODER
    renderJPG_toBUFFER(NULL, (240 - 128) / 2, (240 - 128) / 2 - 32, JPG_WARNINGICON, sizeof(JPG_WARNINGICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
#endif
    set_font_XY(16, 200);
    draw_string_static((char * )
                       "USB HOST FAILED. CONTINUE?", 0xffff);
    delay___(250);
    printf("USB: No USB HOST Shield?\n");
  } else {
    printf("[ OK ]\n");
  }
#if(USBHOST == 2)
  while (Usb.Init(1000) == -1 && !INPUTREAD()) {
    printf("USB: No USB HOST Shield?\n");
  }
  printf("[ OK ]\n");
#endif


  delay___(200);






#if USBHOST_HID
  if (USBhost_keyboard_and_mouse == 2) {
  delay___(200);

/*
  Hid3.SetReportParser(0, &KbdPrs);
  Hid3.SetReportParser(1, &MousePrs);
*/



  ///using namespace USB_HID_DEVICES;

  delay___(2000);
    
 /* HidComposite.SetReportParser(0, &KbdPrs);
  HidComposite.SetReportParser(1, &MousePrs);
  HidKeyboard.SetReportParser(0, &KbdPrs);
  HidMouse.SetReportParser(0, &MousePrs);
*/

  
  }
#endif

#if USBHOST_JOYSTICKS
  if (USBhost_joysticks == 2) {
  delay___(200);

  ///using namespace USB_JOYSTICK_DEVICES;
  
  //USB JOYSTICK EXPERIMENT...
  ///if (!Hid1.SetReportParser(0, &Joy1)) ErrorMessage<uint8_t > (PSTR("SetReportParser"), 1);
  ///if (!Hid2.SetReportParser(0, &Joy2)) ErrorMessage<uint8_t > (PSTR("SetReportParser"), 1);

///  if (!Hid.SetReportParser(0, &Uni)) ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1  );
  
  }
#endif



#endif
  //--------------------------------------------------------------------------------


  INPUTCLEAR();
  screenmemory_fillscreen_static(0);
  delay___(100);


  //--------------------------------------------------------------------------------
  // INIT microSD CARD
  //--------------------------------------------------------------------------------
#if MICROSD
  printf("Initializing microSD CARD...");

  if (SD_CS_PIN!=-1) {
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, 0);
  }

  if (!SD.begin(SD_CONFIG)) {
    printf("[fail]\n");
#if JPG_DECODER
    renderJPG_toBUFFER(NULL, (240 - 128) / 2, (240 - 128) / 2 - 32, JPG_WARNINGICON, sizeof(JPG_WARNINGICON), JPGOUTPUT_BUFFER, JPG_BUFFER_SIZE, 0); //data, sizeof(data), scale
#endif
    set_font_XY(16, 200);
    draw_string_static((char * )
                       "MICROSD FAILED. CONTINUE?", 0xffff);
    delay___(250);
  } else {
    printf("[ OK ]\n");
  }
#if(MICROSD == 2)
  while (!SD.begin(SD_CONFIG) && !INPUTREAD()) {
    printf("No MicroSD card detected!\n");
    delay___(1000); //wait 250ms for confirm
  }
  printf("[ OK ]\n");
#endif
#endif

  
  INPUTCLEAR();
  screenmemory_fillscreen_static(0);
  delay___(100);


  //--------------------------------------------------------------------------------
  // MULTITASKING INIT
  //--------------------------------------------------------------------------------
  //MAIN APPS TASK IN CORE 0 = Higher priority better but can not be more than 23 (NCAT LITE and PRO)
  //TWATCH not more than 22 ?


  ///  xTaskCreatePinnedToCore( &mainCore0, "mainCore0", 8192, NULL, 23 | portPRIVILEGE_BIT, NULL, 0); //Core0 (8kB is too low when using BT???)
  xTaskCreatePinnedToCore( &mainCore0, "mainCore0", 8192, NULL, 23 | portPRIVILEGE_BIT, NULL, 0); //Core0

  if (MULTITASK_MODE == 1)
    xTaskCreatePinnedToCore( &SYSTEMTASK, "SYSTEMTASK", 4096, NULL, 20 | portPRIVILEGE_BIT, NULL, 1); //Core1
#if USBHOST
  ///xTaskCreatePinnedToCore( &USBTASK, "USBTASK", 4096, NULL, 20 | portPRIVILEGE_BIT, NULL, 1); //Core1?
#endif



  //--------------------------------------------------------------------------------
  //  BLUETOOTH INIT
  //--------------------------------------------------------------------------------
  BLUETOOTH_APP = -1;
#if BLUETOOTH
  if (Force_Bluetooth != 1) {

  printf("btdm_controller_sleep_mode %d\n",btdm_controller_get_sleep_mode() );
  btdm_controller_set_sleep_mode(0); ///BTDM_MODEM_SLEEP_MODE_NONE
  printf("btdm_controller_sleep_mode %d\n",btdm_controller_get_sleep_mode() );

    
    BLUETOOTH_INIT();

#if defined(BLUTOOTH_POWER_MIN) && defined(BLUTOOTH_POWER_MAX)
    esp_power_level_t min_bt, max_bt;
    //esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV,ESP_PWR_LVL_P9);    //we not use BLE
    //esp_bredr_tx_power_set(ESP_PWR_LVL_P9,ESP_PWR_LVL_P9);
    esp_bredr_tx_power_set(BLUTOOTH_POWER_MIN, BLUTOOTH_POWER_MAX);
    esp_bredr_tx_power_get( &min_bt, &max_bt);
    printf("min %d max %d\n", min_bt, max_bt);
    run_loop_exit_requested = false;
#endif
    //btstack_run_loop_execute();  //execute BTSTACK main loop
  } else {
    BLUETOOTH_CONNECTIONS = -1;
  }

esp_err_t err;

/*
if ((err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "btLS", &s_light_sleep_pm_lock)) != ESP_OK) printf("ERROR WHILE CREATING PM LOCK: ESP_PM_NO_LIGHT_SLEEP\n");
if ((err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "bt", &s_pm_lock)) != ESP_OK)  printf("ERROR WHILE CREATING PM LOCK: ESP_PM_APB_FREQ_MAX\n");

  esp_pm_lock_acquire(s_light_sleep_pm_lock);
  esp_pm_lock_acquire(s_pm_lock);
*/
/*  printf("btdm_controller_sleep_mode %d\n",btdm_controller_get_sleep_mode() );
  btdm_controller_set_sleep_mode(0); ///BTDM_MODEM_SLEEP_MODE_NONE
*/
  btdm_controller_enable_sleep(false);

  printf("btdm_controller_sleep_mode %d\n",btdm_controller_get_sleep_mode() );


#endif




  //--------------------------------------------------------------------------------
  MEMORY_STATUS();
  printf("SETUP DONE...[ OK ]\n");
  //--------------------------------------------------------------------------------


}



//██████████████████████████████████████████████████
//█  LOOP
//██████████████████████████████████████████████████

void loop() {
  ///printf("RAM free:%u\n", ESP.getFreeHeap()); ///

  
#if BLUETOOTH
  if (MULTITASK_MODE == 0) {


    if (Force_Bluetooth == 1) {
      SYSTEM_TASK();

    } else {


      //printf("Force_Bluetooth: %d, BLUETOOTH_APP: %d, BLUETOOTH_CONNECTIONS: %d\n", Force_Bluetooth, BLUETOOTH_APP, BLUETOOTH_CONNECTIONS);

      //********************************************************************************
      //* LITE BLUETOOTH TASK & SYSTEM TASK CALL                                       *
      //********************************************************************************
      if (POWER != APP_AUD_BTSPKER /*&& POWER!=0*/) {


        // poll data sources
        btstack_data_source_t *ds;
        btstack_data_source_t *next;
        for (ds = (btstack_data_source_t *) btstack_run_loop_base_data_sources; ds != NULL ; ds = next) {

          next = (btstack_data_source_t *) ds->item.next; // cache pointer to next data_source to allow data source to remove itself
          if (ds->flags & DATA_SOURCE_CALLBACK_POLL) {
            //ds->process(ds, DATA_SOURCE_CALLBACK_POLL);
            if (transport_signal_sent) {
              transport_signal_sent = 0;
              //transport_notify_packet_send();
              //________________________________________________________________________________
              // notify upper stack that it might be possible to send again
              uint8_t event[] = { HCI_EVENT_TRANSPORT_PACKET_SENT, 0};
              transport_packet_handler(HCI_EVENT_PACKET, &event[0], sizeof(event));
              //________________________________________________________________________________
            }

            if (transport_packets_to_deliver) {
              transport_packets_to_deliver = 0;
              //transport_deliver_packets();
              //________________________________________________________________________________
              vPortYield(); ///
              xSemaphoreTake(ring_buffer_mutex, portMAX_DELAY);
              vPortYield(); ///
              while (btstack_ring_buffer_bytes_available(&hci_ringbuffer)) {
                uint32_t number_read;
                uint8_t len_tag[2];
                btstack_ring_buffer_read(&hci_ringbuffer, len_tag, 2, &number_read);
                uint32_t len = little_endian_read_16(len_tag, 0);
                btstack_ring_buffer_read(&hci_ringbuffer, hci_receive_buffer, len, &number_read);
                xSemaphoreGive(ring_buffer_mutex);
                vPortYield(); ///
                transport_packet_handler(hci_receive_buffer[0], &hci_receive_buffer[1], len - 1);
                vPortYield(); ///
                xSemaphoreTake(ring_buffer_mutex, portMAX_DELAY);
              }
              xSemaphoreGive(ring_buffer_mutex);
              vPortYield(); ///
              //________________________________________________________________________________
            }

          }
        }






        while (true) {
          function_call_t message = { NULL, NULL };
          BaseType_t res = xQueueReceive( btstack_run_loop_queue, &message, 0);
          if (res == pdFALSE) break;
          if (message.fn) {
            message.fn(message.arg);
          }
        }

        uint32_t now___ = btstack_run_loop_freertos_get_time_ms();
        // process timers, exit when timeout is in the future
        while (btstack_run_loop_base_timers) {
          btstack_timer_source_t * ts = (btstack_timer_source_t *) btstack_run_loop_base_timers;
          int32_t delta = btstack_time_delta(ts->timeout, now___);
          if (delta > 0) break;
          btstack_run_loop_base_remove_timer(ts);
          ts->process(ts);
        }
        //--------------------------------------------------------------------------------
        SYSTEM_TASK();
        //--------------------------------------------------------------------------------
      } else {
        //********************************************************************************
        //* DEFAULT BLUETOOTH TASK                                                       *
        //********************************************************************************

        // process data sources
        btstack_run_loop_base_poll_data_sources();

        // process registered function calls on run loop thread
        while (true) {
          function_call_t message = {
            NULL,
            NULL
          };

          BaseType_t res = xQueueReceive(btstack_run_loop_queue, &message, 0);
          if (res == pdFALSE) break;
          if (message.fn) {
            message.fn(message.arg);
          }
        }

        // process timers
        uint32_t now = btstack_run_loop_freertos_get_time_ms();
        btstack_run_loop_base_process_timers(now);

        // wait for timeout or event group/task notification
        int32_t timeout_next_timer_ms = btstack_run_loop_base_get_time_until_timeout(now);

        uint32_t timeout_ms = 10; ///portMAX_DELAY;
        if (timeout_next_timer_ms >= 0) {
          timeout_ms = (uint32_t)(timeout_next_timer_ms);
        }
#ifdef HAVE_FREERTOS_TASK_NOTIFICATIONS
        xTaskNotifyWait(pdFALSE, 0xffffffff, NULL, pdMS_TO_TICKS(timeout_ms));
#else
        xEventGroupWaitBits(btstack_run_loop_event_group, EVENT_GROUP_FLAG_RUN_LOOP, 1, 0, pdMS_TO_TICKS(timeout_ms));
#endif
        //********************************************************************************
      }
      //********************************************************************************
    }
  } else if (MULTITASK_MODE == 1) {
    // execute BTSTACK main loop
    btstack_run_loop_execute();
  }
#else
  SYSTEM_TASK();
#endif

}
//██████████████████████████████████████████████████
//█  MEOW END.
//██████████████████████████████████████████████████
