#include <driver/adc.h>

//================================================================================
// FASTER digital READ and WRITE
//================================================================================
extern void ___digitalWrite(uint8_t pin, uint8_t val) {
  if(val) {
     if(pin < 32) {
        GPIO.out_w1ts = ((uint32_t)1 << pin);
     } else if(pin < 34) {
        GPIO.out1_w1ts.val = ((uint32_t)1 << (pin - 32));
     }
  } else {
     if(pin < 32) {
        GPIO.out_w1tc = ((uint32_t)1 << pin);
     } else if(pin < 34) {
        GPIO.out1_w1tc.val = ((uint32_t)1 << (pin - 32));
     }
  }
}
//================================================================================
void LOW_ENERGY() {     
  #if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
    if (toggle_screen) {
      printf("LOW_ENERGY.\n");     
      ttgo->closeBL();
      ttgo->bma->enableStepCountInterrupt(false);
      //ttgo->displaySleep();
      toggle_screen = false;        
    } else {
      printf("HIGH_ENERGY.\n");        
      //ttgo->displayWakeup();
      ttgo->openBL();
      ttgo->bma->enableStepCountInterrupt();
      toggle_screen=true;
    }
  #endif    
}
//================================================================================
void _DEEP_SLEEP() {
  #if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2) 
    printf("DEEPSLEEP.\n");
    delay(3000);
    ttgo->powerOff();
    // Use ext1 for external wakeup
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_deep_sleep_start();   
  #endif
}
//--------------------------------------------------------------------------------
byte*    keymap;                          // scancodes keymap array... (ps_malloc 256bytes)
uint32_t keyboardcounter_pause;
uint32_t keyboardcounter_repeat;
#define  KEYBOARD_REPEAT_PAUSE 500        // in milliseconds
#define  KEYBOARD_REPEAT_TIME  50         // in milliseconds
uint32_t* prevkeytime;                    // malloc/ps_malloc 1kB
//--------------------------------------------------------------------------------

#include "bluepad32/uni_hid_parser_generic.h"
#include "bluepad32/hid_usage.h"
#include "bluepad32/uni_debug.h"
#include "bluepad32/uni_hid_device.h"
#include "bluepad32/uni_hid_parser.h"

//================================================================================
int8_t  AXISX=0;                          // X AXIS POSITION
int8_t  AXISY=0;                          // Y AXIS POSITION
//================================================================================
int16_t MOUSEX=0;  
int16_t MOUSEY=0;
int16_t MOUSEZ=0;                         // SCROLL WHEEL
uint8_t MOUSEBUTTONS=0;
uint8_t MOUSEBUTTONS_LAST=0;
//================================================================================
uint8_t BUTTONS=0;                        // MOUSE & JOYSTICK (0bRLZYXCBA)
uint8_t BUTTONS_LAST=0; 
//================================================================================
int16_t TOUCHX=0;                         // TOUCHPAD
int16_t TOUCHY=0;
//================================================================================
uint8_t* BTKEYBOARD;                      // malloc/ps_malloc
uint8_t BTBUFFER_SET[6]= {0};
uint8_t BTBUFFER_CLEAR[6]= {0};
int8_t  BTBUF_POS=0;                      // KEY RELEASE BUFFER POS (STATUS)
//================================================================================
uint8_t* USBKEYBOARD;                     // malloc/ps_malloc
//================================================================================
uint8_t* PS2KEYBOARD;                     // malloc/ps_malloc
//================================================================================
uint8_t JOYBUTTON=0;                      // 4bits
uint8_t JOYBUTTON_LAST=0; 
//--------------------------------------------------------------------------------
uint8_t JOY_UP=0;
uint8_t JOY_DOWN=0;
uint8_t JOY_LEFT=0;
uint8_t JOY_RIGHT=0;

uint8_t JOY_A=0;
uint8_t JOY_B=0;
uint8_t JOY_C=0;
uint8_t JOY_X=0;
uint8_t JOY_Y=0;
uint8_t JOY_Z=0;
uint8_t JOY_L=0;
uint8_t JOY_R=0;
 
uint8_t JOY_START=0;
uint8_t JOY_SELECT=0;
uint8_t JOY_SYSTEM=0; //0=HOME button OFF, 1=HOME button ON.
//================================================================================
// USB JOYSTICK Player 1
//--------------------------------------------------------------------------------
uint8_t USB_JOY_UP=0;
uint8_t USB_JOY_DOWN=0;
uint8_t USB_JOY_LEFT=0;
uint8_t USB_JOY_RIGHT=0;

uint8_t USB_JOY_A=0;
uint8_t USB_JOY_B=0;
uint8_t USB_JOY_C=0;
uint8_t USB_JOY_X=0;
uint8_t USB_JOY_Y=0;
uint8_t USB_JOY_Z=0;
uint8_t USB_JOY_L=0;
uint8_t USB_JOY_R=0;
 
uint8_t USB_JOY_START=0;
uint8_t USB_JOY_SELECT=0;
uint8_t USB_JOY_SYSTEM=0; //0=HOME button OFF, 1=HOME button ON.
//================================================================================
//================================================================================
// USB JOYSTICK Player 2
//--------------------------------------------------------------------------------
uint8_t USB_JOY2_UP=0;
uint8_t USB_JOY2_DOWN=0;
uint8_t USB_JOY2_LEFT=0;
uint8_t USB_JOY2_RIGHT=0;

uint8_t USB_JOY2_A=0;
uint8_t USB_JOY2_B=0;
uint8_t USB_JOY2_C=0;
uint8_t USB_JOY2_X=0;
uint8_t USB_JOY2_Y=0;
uint8_t USB_JOY2_Z=0;
uint8_t USB_JOY2_L=0;
uint8_t USB_JOY2_R=0;
 
uint8_t USB_JOY2_START=0;
uint8_t USB_JOY2_SELECT=0;
uint8_t USB_JOY2_SYSTEM=0; //0=HOME button OFF, 1=HOME button ON.
//================================================================================
//GPIO BUTTONS: 9pcs 
//--------------------------------------------------------------------------------
uint8_t BUTTON_UP=0;
uint8_t BUTTON_DOWN=0;
uint8_t BUTTON_LEFT=0;
uint8_t BUTTON_RIGHT=0;
uint8_t BUTTON_A=0;
uint8_t BUTTON_B=0; 
uint8_t BUTTON_START=0;
uint8_t BUTTON_SELECT=0;
uint8_t BUTTON_SYSTEM=0;
//================================================================================
//NES CONTROLLER BUTTONS: 8 
//--------------------------------------------------------------------------------
uint8_t NESCTRL_UP=0;
uint8_t NESCTRL_DOWN=0;
uint8_t NESCTRL_LEFT=0;
uint8_t NESCTRL_RIGHT=0;
uint8_t NESCTRL_A=0;
uint8_t NESCTRL_B=0; 
uint8_t NESCTRL_START=0;
uint8_t NESCTRL_SELECT=0;
//================================================================================
//TOUCHPAD COMBOS:  
//--------------------------------------------------------------------------------
uint8_t TOUCH_UP=0;
uint8_t TOUCH_DOWN=0;
uint8_t TOUCH_LEFT=0;
uint8_t TOUCH_RIGHT=0;
uint8_t TOUCH_A=0;
uint8_t TOUCH_B=0; 
uint8_t TOUCH_START=0;
uint8_t TOUCH_SELECT=0;
uint8_t TOUCH_SYSTEM=0;
//================================================================================
///GENERAL BUTTONS
//================================================================================
uint8_t INPUT_UP=0;                       /// ↑
uint8_t INPUT_DOWN=0;                     /// ↓
uint8_t INPUT_LEFT=0;                     /// ←
uint8_t INPUT_RIGHT=0;                    /// →
uint8_t INPUT_A=0;                        /// X = ALT = Z
uint8_t INPUT_B=0;                        /// □ = CTRL = X
uint8_t INPUT_X=0;                        /// O
uint8_t INPUT_Y=0;                        /// Δ
uint8_t INPUT_START=0;                    /// ◄ START = ENTER
uint8_t INPUT_SELECT=0;                   /// ► SELECT = BACKSPACE
uint8_t INPUT_SYSTEM=0;                   /// ■ SYSTEM = ESCAPE
//================================================================================
uint8_t Player2_INPUT_UP=0;                       /// ↑
uint8_t Player2_INPUT_DOWN=0;                     /// ↓
uint8_t Player2_INPUT_LEFT=0;                     /// ←
uint8_t Player2_INPUT_RIGHT=0;                    /// →
uint8_t Player2_INPUT_A=0;                        /// X = ALT = Z
uint8_t Player2_INPUT_B=0;                        /// □ = CTRL = X
uint8_t Player2_INPUT_X=0;                        /// O
uint8_t Player2_INPUT_Y=0;                        /// Δ
uint8_t Player2_INPUT_START=0;                    /// ◄ START = ENTER
uint8_t Player2_INPUT_SELECT=0;                   /// ► SELECT = BACKSPACE
uint8_t Player2_INPUT_SYSTEM=0;                   /// ■ SYSTEM = ESCAPE
//================================================================================

uint8_t TOUCHPAD_INPUT();
uint8_t BUTTONSINPUT();
//================================================================================
void EVENTS();
//================================================================================
uint32_t NES_CONTROLLER_TIME=0;
void CLEAR_NES_CONTROLLER() {
#if NES_CONTROLLER  
  NESCTRL_UP=0;
  NESCTRL_DOWN=0;
  NESCTRL_LEFT=0;
  NESCTRL_RIGHT=0;
  NESCTRL_A=0;
  NESCTRL_B=0; 
  NESCTRL_START=0;
  NESCTRL_SELECT=0;
#endif   
}

void NES_CONTROLLER_READ() {
#define NES_CONTROLLER_REFRESH_TIME 40 //every 40ms read JOYSTICK  
#if NES_CONTROLLER  
  if (millis() - NES_CONTROLLER_TIME > NES_CONTROLLER_REFRESH_TIME) { 
    NES_CONTROLLER_TIME=millis();
    CLEAR_NES_CONTROLLER();

    //NES Controller strobe
    digitalWrite(NESCTRL_Latch, 1);
    digitalWrite(NESCTRL_Latch, 0);

    delayMicroseconds(1); //ESP32 is too fast for NES CONTROLLER...

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_A=1; //else NESCTRL_A=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_B=1; //else NESCTRL_B=0; 
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_SELECT=1; //else NESCTRL_SELECT=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_START=1; //else NESCTRL_START=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_UP=1; //else NESCTRL_UP=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_DOWN=1; //else NESCTRL_DOWN=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_LEFT=1; //else NESCTRL_LEFT=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);
    delayMicroseconds(1);

    if (digitalRead(NESCTRL_Data) == 0) NESCTRL_RIGHT=1; //else NESCTRL_RIGHT=0;
    digitalWrite(NESCTRL_Clock, 0);
    digitalWrite(NESCTRL_Clock, 1);  
    delayMicroseconds(1);
  }
#endif  
}

extern uint32_t ON_SECONDS;               //TOTAL ON SCEONDS
extern uint32_t LAST_ACTIVITY_SECONDS;    //LAST ACTIVITY SECONDS TIME
extern uint8_t LCD_BACKLIGHT;
extern uint8_t BACKLIGHT_PIN_LAST_VALUE;

uint8_t wakeup_LCD();
uint8_t INPUTREPORT();
uint8_t INPUTREAD() {
//--------------------------------------------------------------------------------
#if USBHOST
//--------------------------------------------------------------------------------
#if USBHOST_BLUETOOTH
  if (USBhost_bluetooth_PS4joysticks == 2) {
    if (PS4.connected()) {
      if (PS4.getButtonPress(UP)) INPUT_UP=1;
      if (PS4.getButtonPress(RIGHT)) INPUT_RIGHT=1;   
      if (PS4.getButtonPress(DOWN)) INPUT_DOWN=1;
      if (PS4.getButtonPress(LEFT)) INPUT_LEFT=1;
      if (PS4.getButtonPress(TRIANGLE)) INPUT_Y=1; 
      if (PS4.getButtonPress(CIRCLE)) INPUT_X=1; 
      if (PS4.getButtonPress(CROSS)) INPUT_A=1;
      if (PS4.getButtonPress(SQUARE)) INPUT_B=1; 

      if (PS4.getButtonPress(SHARE)) INPUT_START=1;
      if (PS4.getButtonPress(OPTIONS)) INPUT_SELECT=1;
      if (PS4.getButtonPress(PS)) INPUT_SYSTEM=1;
    }   
  }  
#endif
#endif
//--------------------------------------------------------------------------------
  BUTTONSINPUT();
  NES_CONTROLLER_READ();
  TOUCHPAD_INPUT();
 
  if (BUTTON_UP==1 || JOY_UP==1 || USB_JOY_UP==1 || NESCTRL_UP==1 || USBKEYBOARD[0x57] || BTKEYBOARD[0x57] || TOUCH_UP==1 || PS2KEYBOARD[0x57]==1) INPUT_UP=1;
  if (BUTTON_DOWN==1 || JOY_DOWN==1 || USB_JOY_DOWN==1 || NESCTRL_DOWN==1 || USBKEYBOARD[0x62] || BTKEYBOARD[0x62] || TOUCH_DOWN==1 || PS2KEYBOARD[0x62]==1) INPUT_DOWN=1;
  if (BUTTON_LEFT==1 || JOY_LEFT==1 || USB_JOY_LEFT==1 || NESCTRL_LEFT==1 || USBKEYBOARD[0x61] || BTKEYBOARD[0x61] || TOUCH_LEFT==1 || PS2KEYBOARD[0x61]==1) INPUT_LEFT=1;
  if (BUTTON_RIGHT==1 || JOY_RIGHT==1 || USB_JOY_RIGHT==1 || NESCTRL_RIGHT==1 || USBKEYBOARD[0x63] || BTKEYBOARD[0x63] || TOUCH_RIGHT==1 || PS2KEYBOARD[0x63]==1) INPUT_RIGHT=1;
  if (BUTTON_A==1 || JOY_B==1 || USB_JOY_A==1 || NESCTRL_A==1 || USBKEYBOARD[0x5d] || USBKEYBOARD[0x4c] || BTKEYBOARD[0x5d] || BTKEYBOARD[0x4c] || TOUCH_A==1
    || PS2KEYBOARD[0x5d]== 1 || PS2KEYBOARD[0x4c]==1) INPUT_A=1;
    
  if (JOY_X==1 || USB_JOY_Y==1 || USBKEYBOARD[0x3c] || BTKEYBOARD[0x3c] || PS2KEYBOARD[0x3c]) INPUT_Y=1; 
  if (BUTTON_B==1 || JOY_A==1 || USB_JOY_B==1 || NESCTRL_B==1 || USBKEYBOARD[0x5c] || USBKEYBOARD[0x4d] || BTKEYBOARD[0x5c] || BTKEYBOARD[0x4d] ||
    PS2KEYBOARD[0x5c]== 1 || PS2KEYBOARD[0x4d]==1) INPUT_B=1;
    
  if (JOY_C==1 || USB_JOY_X==1 || USBKEYBOARD[0x3d] || BTKEYBOARD[0x3d] || PS2KEYBOARD[0x3d]) INPUT_X=1;
  
  if (BUTTON_START==1 || USB_JOY_START==1 || JOY_START || NESCTRL_START==1 || USBKEYBOARD[0x47] || BTKEYBOARD[0x47] || PS2KEYBOARD[0x47]==1) INPUT_START=1;
  if (BUTTON_SELECT==1 || USB_JOY_SELECT==1 || JOY_SELECT || NESCTRL_SELECT==1 || USBKEYBOARD[0x1e] || BTKEYBOARD[0x1e] || PS2KEYBOARD[0x1e]==1) INPUT_SELECT=1;
  if (JOY_SYSTEM || USB_JOY_SYSTEM || BUTTON_SYSTEM || USBKEYBOARD[0x01] || BTKEYBOARD[0x01] || TOUCH_SYSTEM==1 || PS2KEYBOARD[0x01]==1) INPUT_SYSTEM=1;

  if (DEBUG>4) INPUTREPORT();
//................................................................................
  if (Multiplayer_dev_input_support==2) {
    if (USB_JOY2_UP==1) Player2_INPUT_UP=1;
    if (USB_JOY2_DOWN==1) Player2_INPUT_DOWN=1;
    if (USB_JOY2_LEFT==1) Player2_INPUT_LEFT=1;
    if (USB_JOY2_RIGHT==1) Player2_INPUT_RIGHT=1;

    if (USB_JOY2_A==1) Player2_INPUT_A=1;    
    if (USB_JOY2_Y==1) Player2_INPUT_Y=1; 
    if (USB_JOY2_B==1) Player2_INPUT_B=1;   
    if (USB_JOY2_X==1) Player2_INPUT_X=1;

    if (USB_JOY2_START==1) Player2_INPUT_START=1;
    if (USB_JOY2_SELECT==1) Player2_INPUT_SELECT=1;
  }
//................................................................................
  //return 1=keypressed, 0=no change
  if (INPUT_UP==1||INPUT_DOWN==1||INPUT_LEFT==1||INPUT_RIGHT==1
  ||INPUT_A==1||INPUT_B==1||INPUT_X==1||INPUT_Y==1
  ||INPUT_START==1||INPUT_SELECT==1||INPUT_SYSTEM==1) {

    wakeup_LCD();
    LAST_ACTIVITY_SECONDS=ON_SECONDS;  
    return 1; 
  } else return 0;  
}
//================================================================================
uint8_t INPUTCLEAR() {
  INPUT_UP=0;         /// ↑
  INPUT_DOWN=0;       /// ↓
  INPUT_LEFT=0;       /// ←
  INPUT_RIGHT=0;      /// →
  INPUT_A=0;          /// X = CTRL = X
  INPUT_B=0;          /// □ = ALT = Z
  INPUT_X=0;          /// O
  INPUT_Y=0;          /// Δ
  INPUT_START=0;      /// ◄ START =  ENTER
  INPUT_SELECT=0;     /// ► SELECT = BACKSPACE  
  INPUT_SYSTEM=0;     /// ■ SYSTEM = ESCAPE
//................................................................................
  if (Multiplayer_dev_input_support==2) {
    Player2_INPUT_UP=0;
    Player2_INPUT_DOWN=0;
    Player2_INPUT_LEFT=0;
    Player2_INPUT_RIGHT=0; 
    Player2_INPUT_A=0;    
    Player2_INPUT_Y=0; 
    Player2_INPUT_B=0;   
    Player2_INPUT_X=0;
    Player2_INPUT_START=0;
    Player2_INPUT_SELECT=0;
    Player2_INPUT_SYSTEM=0; 
  }
//................................................................................  
  return 1;
}
//================================================================================
uint8_t INPUTREPORT() {
  if (INPUT_UP) printf("↑"); else printf(" ");
  if (INPUT_DOWN) printf("↓"); else printf(" ");
  if (INPUT_LEFT) printf("←"); else printf(" ");
  if (INPUT_RIGHT) printf("→"); else printf(" ");
  printf(" ");
  if (INPUT_A) printf("A"); else printf(" ");
  if (INPUT_B) printf("B"); else printf(" ");
  if (INPUT_X) printf("X"); else printf(" ");
  if (INPUT_Y) printf("Y"); else printf(" ");
  printf(" ");
  if (INPUT_START) printf("◄"); else printf(" ");
  if (INPUT_SYSTEM) printf("■"); else printf(" ");
  if (INPUT_SELECT) printf("►"); else printf(" ");
  printf("\n");

  return 1;
}
//================================================================================
//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒

//--------------------------------------------------------------------------------
uint8_t digital_IN = 0;
uint16_t digital_CALIBRATION = 0;
uint32_t BatteryLevelCounter=0;

#define MOUSE_COMBO_UPDATE_TIME 10
#define MOUSE_COMBO_DBLCLICK_TIME 160
#define MOUSE_COMBO_REACT_TIME 160
#define MOUSE_COMBO_TIMEOUT_TIME 80
uint32_t MOUSE_COMBO_TOUCH_COUNTER=0;
uint32_t MOUSE_COMBO_COUNTER=0;
uint32_t MOUSE_COMBO_TIMEOUT_COUNTER=0;

int16_t COMBO_MOUSEX=0;  
int16_t COMBO_MOUSEY=0;
int16_t COMBO_MOUSEZ=0;         //SCROLL WHEEL
char DEBUGMOUSEBUF[128];
  
//--------------------------------------------------------------------------------
uint8_t TOUCHPAD_INPUT() {  
#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)  
    if (TWATCH_irq) {
      TWATCH_irq = 0;
      bool  rlst;
      do {
        rlst =  ttgo->bma->readInterrupt();
      } while (!rlst);
      ttgo->power->readIRQ();
      if (ttgo->power->isPEKShortPressIRQ()) {            
        ttgo->power->clearIRQ();           
        TOUCH_SYSTEM=1;
        MOUSE_COMBO_TIMEOUT_COUNTER=millis___(); 
      } 
      if (ttgo->power->isPEKLongtPressIRQ ()) {           
        ttgo->power->clearIRQ();            
        _DEEP_SLEEP();
        MOUSE_COMBO_TIMEOUT_COUNTER=millis___(); 
      }
    }
    if (MOUSE_COMBO_TOUCH_COUNTER+MOUSE_COMBO_UPDATE_TIME<millis___()) {
      if (tft.getTouch(&MOUSEX, &MOUSEY)) { ///    
        sprintf(DEBUGMOUSEBUF, "x:%03d  y:%03d", MOUSEX, MOUSEY);
        if (MOUSE_COMBO_TIMEOUT_COUNTER==0 && MOUSE_COMBO_COUNTER==0) {
           COMBO_MOUSEX=MOUSEX;
           COMBO_MOUSEY=MOUSEY;
           MOUSE_COMBO_COUNTER=millis___();
           printf("ZERO ---> MOUSE X: %d, MOUSE Y: %d, COMBO_MOUSEX: %d, COMBO_MOUSEY: %d\n",MOUSEX,MOUSEY,COMBO_MOUSEX,COMBO_MOUSEY);  //MOUSEMOVE
        }  
        if (MOUSE_COMBO_COUNTER+MOUSE_COMBO_DBLCLICK_TIME<millis___() && abs(COMBO_MOUSEX-MOUSEX)<20 && abs(COMBO_MOUSEY-MOUSEY)<20) {
           MOUSE_COMBO_COUNTER=0;
           COMBO_MOUSEX=0;
           COMBO_MOUSEY=0;
           MOUSEX=0;
           MOUSEY=0;
           printf("SET ---> INPUT_A\n"); 
           TOUCH_A=1;
           MOUSE_COMBO_TIMEOUT_COUNTER=millis___();            
        }
        printf("UPDATE ---> MOUSE X: %d, MOUSE Y: %d, COMBO_MOUSEX: %d, COMBO_MOUSEY: %d\n",MOUSEX,MOUSEY,COMBO_MOUSEX,COMBO_MOUSEY);  //MOUSEMOVE
      } 
      if (MOUSE_COMBO_COUNTER>0 && MOUSE_COMBO_COUNTER+MOUSE_COMBO_REACT_TIME<millis___())    {
        if ((COMBO_MOUSEX>MOUSEX) && (abs(COMBO_MOUSEX-MOUSEX)) > (abs(COMBO_MOUSEY-MOUSEY))) {
          TOUCH_RIGHT=1;
          printf("SET ---> INPUT_RIGHT\n"); 
        }
        if ((COMBO_MOUSEX<MOUSEX) && (abs(MOUSEX-COMBO_MOUSEX)) > (abs(COMBO_MOUSEY-MOUSEY))) {
          TOUCH_LEFT=1;
          printf("SET ---> INPUT_LEFT\n"); 
        }
        if ((COMBO_MOUSEY<MOUSEY) && (abs(MOUSEY-COMBO_MOUSEY)) > (abs(COMBO_MOUSEX-MOUSEX))) {
          TOUCH_DOWN=1;
          printf("SET ---> INPUT_DOWN\n"); 
        }
        if ((MOUSEY<COMBO_MOUSEY) && (abs(COMBO_MOUSEY-MOUSEY)) > (abs(COMBO_MOUSEX-MOUSEX))) {
          TOUCH_UP=1;
          printf("SET ---> INPUT_UP\n"); 
        }       
        printf("SET ---> MOUSE X: %d, MOUSE Y: %d, COMBO_MOUSEX: %d, COMBO_MOUSEY: %d\n",MOUSEX,MOUSEY,COMBO_MOUSEX,COMBO_MOUSEY);  //MOUSEMOVE
        MOUSE_COMBO_COUNTER=0;
        COMBO_MOUSEX=0;
        COMBO_MOUSEY=0;
        MOUSEX=0;
        MOUSEY=0;
        MOUSE_COMBO_TIMEOUT_COUNTER=millis___();                        
      }
      MOUSE_COMBO_TOUCH_COUNTER=millis___();
    }
    if (MOUSE_COMBO_TIMEOUT_COUNTER>0 && MOUSE_COMBO_TIMEOUT_COUNTER+MOUSE_COMBO_TIMEOUT_TIME<millis___()) {
      TOUCH_RIGHT=0;
      TOUCH_LEFT=0;
      TOUCH_UP=0;
      TOUCH_DOWN=0;
      TOUCH_A=0;
      TOUCH_SYSTEM=0;
      MOUSE_COMBO_TIMEOUT_COUNTER=0;
    }    
#endif
  return 1;
}
//--------------------------------------------------------------------------------
uint8_t BUTTONSINPUT() { 
#if (GPIO_BUTTONS==1) // IC 4017 BUTTONS   
  uint8_t CALIBRATED=0;
  for (uint8_t tmp=0;tmp<=10;tmp++) { 
    ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);    
    delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
    ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
    delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
    digital_CALIBRATION=digitalRead(DIGITAL_BUTTON_CALIBRATION);   
    delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
    if (digital_CALIBRATION==1) {
      CALIBRATED=1;
      break;
    }
  }

  if (CALIBRATED) {
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_UP = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_DOWN = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_LEFT = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_RIGHT = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_A = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_B = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_START = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_SELECT = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  ___digitalWrite(DIGITAL_BUTTON_SWITCH,0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  BUTTON_SYSTEM = digitalRead(DIGITAL_BUTTON_IN);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  
  } else {
    //calibration unsuccessful but don reset buttons!
    #if (DEBUG>4)
      printf("BUTTONS CALIBRATION FAILED...\n");
    #endif         
  }
#endif       

#if (GPIO_BUTTONS==2) // BUTTONS MATRIX
  pinMode(PIN_BUS1, INPUT);
  pinMode(PIN_BUS2, INPUT);

  
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  pinMode(PIN_UP_SELECT, OUTPUT); 
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  digitalWrite(PIN_UP_SELECT, 1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  if (digitalRead(PIN_BUS1) == 1) BUTTON_SELECT = 1; else BUTTON_SELECT = 0;
  if (digitalRead(PIN_BUS2) == 1) BUTTON_UP = 1; else BUTTON_UP = 0;
  digitalWrite(PIN_UP_SELECT, 0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  pinMode(PIN_UP_SELECT, INPUT);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);


  pinMode(PIN_DOWN_A, OUTPUT); 
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  digitalWrite(PIN_DOWN_A, 1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  if (digitalRead(PIN_BUS1) == 1) BUTTON_A = 1; else BUTTON_A = 0;
  if (digitalRead(PIN_BUS2) == 1) BUTTON_DOWN = 1; else BUTTON_DOWN = 0;
  digitalWrite(PIN_DOWN_A, 0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  pinMode(PIN_DOWN_A, INPUT);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);

  pinMode(PIN_RIGHT_START, OUTPUT); 
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  digitalWrite(PIN_RIGHT_START, 1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  if (digitalRead(PIN_BUS1) == 1) BUTTON_START = 1; else BUTTON_START = 0;
  if (digitalRead(PIN_BUS2) == 1) BUTTON_RIGHT = 1; else BUTTON_RIGHT = 0;
  digitalWrite(PIN_RIGHT_START, 0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  pinMode(PIN_RIGHT_START, INPUT);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);

  pinMode(PIN_LEFT_B, OUTPUT); 
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  digitalWrite(PIN_LEFT_B, 1);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  if (digitalRead(PIN_BUS1) == 1) BUTTON_B = 1; else BUTTON_B = 0;
  if (digitalRead(PIN_BUS2) == 1) BUTTON_LEFT = 1; else BUTTON_LEFT = 0;
  digitalWrite(PIN_LEFT_B, 0);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);
  pinMode(PIN_LEFT_B, INPUT);
  delayMicroseconds___(DIGITAL_BUTTON_DELAY_us);

#endif  
#if (GPIO_BUTTONS==3) // DIRECT GPIO BUTTONS      
  if (digitalRead(PIN_A)==1) BUTTON_A = 1; else BUTTON_A = 0;  
  if (digitalRead(PIN_B)==1) BUTTON_B = 1; else BUTTON_B = 0;  
  if (digitalRead(PIN_SELECT)==1) BUTTON_SELECT = 1; else BUTTON_SELECT = 0;  
  if (digitalRead(PIN_START)==1) BUTTON_START = 1; else BUTTON_START = 0;  
  if (digitalRead(PIN_UP)==1) BUTTON_UP = 1; else BUTTON_UP = 0;  
  if (digitalRead(PIN_DOWN)==1) BUTTON_DOWN = 1; else BUTTON_DOWN = 0;  
  if (digitalRead(PIN_LEFT)==1) BUTTON_LEFT = 1; else BUTTON_LEFT = 0;  
  if (digitalRead(PIN_RIGHT)==1) BUTTON_RIGHT = 1; else BUTTON_RIGHT = 0;  
#endif        
  return 1;
}
//▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
//================================================================================
const int Service_freq = 5000;    ///5000Hz
const int Service_ledChannel = 0; 
const int Service_resolution = 8;
//--------------------------------------------------------------------------------

uint8_t LCD_BACKLIGHT_PIN_SETUP(uint8_t BACKLIGHT_LEVEL) {
#if LCD_ENABLED
  BACKLIGHT_LEVEL=BACKLIGHT_LEVEL/8; 
  BACKLIGHT_LEVEL=BACKLIGHT_LEVEL<<3;

#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
  ttgo->bl->adjust(BACKLIGHT_LEVEL);
#else     
  tft.setBrightness(BACKLIGHT_LEVEL);
#endif  
#endif  
  return 1;
}  
//--------------------------------------------------------------------------------
void BACKLIGHT_PIN_UPDATE(uint8_t BACKLIGHT_LEVEL) {
#if LCD_BACKLIGHT_CONTROL
  BACKLIGHT_LEVEL=BACKLIGHT_LEVEL/8; 
  BACKLIGHT_LEVEL=BACKLIGHT_LEVEL<<3;
#if defined(NCAT_TWATCH_V1) || defined(NCAT_TWATCH_V2) || defined(HACKED_NCAT_TWATCH_V2)
  ttgo->bl->adjust(BACKLIGHT_LEVEL);
#else  
  tft.setBrightness(BACKLIGHT_LEVEL);
#endif  
#endif
}
 
//--------------------------------------------------------------------------------
#if FAKE86_EMULATOR
void FAKE86_KEYBOARD();
#endif
uint8_t BTKEYBOARD_STATE=0;

uint8_t BLUETOOTH_KEYBOARD_PRESS(uint8_t KEY) { 
#if DEBUG>4
  printf("BTKEY: %d\n",KEY); ///
#endif  
  switch (KEY) {
/// case 1: //KEYBOARD OVERFLOW = not used
///   BTKEYBOARD[]=1;
///   break;
/// case 2:
///   BTKEYBOARD[]=1;
///   break;
/// case 3:
///   BTKEYBOARD[]=1;
///   break;
    case 4: //A
      BTKEYBOARD[0x3c]=1;
      break;
    case 5: //B
      BTKEYBOARD[0x50]=1;
      break;
    case 6: //C
      BTKEYBOARD[0x4e]=1;
      break;
    case 7: //D
      BTKEYBOARD[0x3e]=1;
      break;
    case 8: //E
      BTKEYBOARD[0x29]=1;
      break;
    case 9: //F
      BTKEYBOARD[0x3f]=1;
      break;
    case 10: //G
      BTKEYBOARD[0x40]=1;
      break;
    case 11: //H
      BTKEYBOARD[0x41]=1;
      break;
    case 12: //I
      BTKEYBOARD[0x2e]=1;
      break;
    case 13: //J
      BTKEYBOARD[0x42]=1;
      break;
    case 14: //K
      BTKEYBOARD[0x43]=1;
      break;
    case 15: //L
      BTKEYBOARD[0x44]=1;
      break;
    case 16: //M
      BTKEYBOARD[0x52]=1;
      break;
    case 17: //N
      BTKEYBOARD[0x51]=1;
      break;
    case 18: //O
      BTKEYBOARD[0x2f]=1;
      break;
    case 19: //P
      BTKEYBOARD[0x30]=1;
      break;
    case 20: //Q
      BTKEYBOARD[0x27]=1;
      break;
    case 21: //R
      BTKEYBOARD[0x2a]=1;
      break;
    case 22: //S
      BTKEYBOARD[0x3d]=1;
      break;
    case 23: //T
      BTKEYBOARD[0x2b]=1;
      break;
    case 24: //U
      BTKEYBOARD[0x2d]=1;
      break;
    case 25: //V
      BTKEYBOARD[0x4f]=1;
      break;
    case 26: //W
      BTKEYBOARD[0x28]=1;
      break;
    case 27: //X
      BTKEYBOARD[0x4d]=1;
      break;
    case 28: //Y
      BTKEYBOARD[0x2c]=1;
      break;
    case 29: //Z
      BTKEYBOARD[0x4c]=1;
      break;
    case 30: //1
      BTKEYBOARD[0x12]=1;
      break;
    case 31: //2
      BTKEYBOARD[0x13]=1;
      break;
    case 32: //3
      BTKEYBOARD[0x14]=1;
      break;
    case 33: //4
      BTKEYBOARD[0x15]=1;
      break;
    case 34: //5
      BTKEYBOARD[0x16]=1;
      break;
    case 35: //6
      BTKEYBOARD[0x17]=1;
      break;
    case 36: //7
      BTKEYBOARD[0x18]=1;
      break;
    case 37: //8
      BTKEYBOARD[0x19]=1;
      break;
    case 38: //9
      BTKEYBOARD[0x1a]=1;
      break;
    case 39: //0
      BTKEYBOARD[0x1b]=1;
      break;
   case 40: //ENTER
      BTKEYBOARD[0x47]=1;
      break;
    case 41: //ESC
      BTKEYBOARD[0x01]=1;
      break;
    case 42: //BACKSPACE
      BTKEYBOARD[0x1e]=1;
      break;
    case 43: //TAB
      BTKEYBOARD[0x26]=1;
      break;
    case 44: //SPACEBAR
      BTKEYBOARD[0x5e]=1;
      break;
    case 45: //-
      BTKEYBOARD[0x1c]=1;
      break;
    case 46: //=
      BTKEYBOARD[0x1d]=1;
      break;
    case 47: //[ 
      BTKEYBOARD[0x31]=1;
      break;
    case 48: //]
      BTKEYBOARD[0x32]=1;
      break;
/// case 49:
///   BTKEYBOARD[]=1;
///   break;
    case 50: //\\
      BTKEYBOARD[0x33]=1;
      break;
    case 51: //;
      BTKEYBOARD[0x45]=1;
      break;
    case 52: //'
      BTKEYBOARD[0x46]=1;
      break;
    case 53: //~
      BTKEYBOARD[0x11]=1;
      break;
    case 54: //,
      BTKEYBOARD[0x53]=1;
      break;
    case 55: //.
      BTKEYBOARD[0x54]=1;
      break;
    case 56: ///
      BTKEYBOARD[0x55]=1;
      break;
/// case 57:
///   BTKEYBOARD[]=1;
///   break;
    case 58: //F1
      BTKEYBOARD[2]=1;
      break;
    case 59: //F2
      BTKEYBOARD[3]=1;
      break;
    case 60: //F3
      BTKEYBOARD[4]=1;
      break;
    case 61: //F4
      BTKEYBOARD[5]=1;
      break;
    case 62: //F5
      BTKEYBOARD[6]=1;
      break;
    case 63: //F6
      BTKEYBOARD[7]=1;
      break;
    case 64: //F7
      BTKEYBOARD[8]=1;
      break;
    case 65: //F8
      BTKEYBOARD[9]=1;
      break;
    case 66: //F9
      BTKEYBOARD[0x0a]=1;
      break;
    case 67: //F10
      BTKEYBOARD[0x0b]=1;
      break;
    case 68: //F11
      BTKEYBOARD[0x0c]=1;
      break;
    case 69: //F12
      BTKEYBOARD[0x0d]=1;
      break;
    case 70: //PRINTSCR ?
      BTKEYBOARD[0x0e]=1;
      break;
/// case 71:
///   BTKEYBOARD[]=1;
///   break;
/// case 72:
///   BTKEYBOARD[]=1;
///   break;
    case 73: //INSERT
      BTKEYBOARD[0x1f]=1;
      break;
    case 74: //HOME ?
      BTKEYBOARD[0x20]=1;
      break;
    case 75: //PAGEUP ?
      BTKEYBOARD[0x21]=1;
      break;
    case 76: //DELETE
      BTKEYBOARD[0x34]=1;
      break;
    case 77: //END ?
      BTKEYBOARD[0x35]=1;
      break;
    case 78: //PAGEDOWN ?
      BTKEYBOARD[0x36]=1;
      break;
      
    case 79: //RIGHT
      BTKEYBOARD[0x63]=1;
      break;
    case 80: //LEFT
      BTKEYBOARD[0x61]=1;
      break;
    case 81: //DOWN
      BTKEYBOARD[0x62]=1;
      break;
    case 82: //UP
      BTKEYBOARD[0x57]=1;
      break;
      
    case 100: //\\
      BTKEYBOARD[0x33]=1;
      break;
    default:
      break;
  }  

#if FAKE86_EMULATOR
  if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD();
#endif

  return 1;
}
//---------------------------------------------------------------------------------
uint8_t BLUETOOTH_KEYBOARD_RELEASE(uint8_t KEY) {
  switch (KEY) {
/// case 1:
///   BTKEYBOARD[]=0;
///   break;
/// case 2:
///   BTKEYBOARD[]=0;
///   break;
/// case 3:
///   BTKEYBOARD[]=0;
///   break;
    case 4: //A
      BTKEYBOARD[0x3c]=0;
      break;
    case 5: //B
      BTKEYBOARD[0x50]=0;
      break;
    case 6: //C
      BTKEYBOARD[0x4e]=0;
      break;
    case 7: //D
      BTKEYBOARD[0x3e]=0;
      break;
    case 8: //E
      BTKEYBOARD[0x29]=0;
      break;
    case 9: //F
      BTKEYBOARD[0x3f]=0;
      break;
    case 10: //G
      BTKEYBOARD[0x40]=0;
      break;
    case 11: //H
      BTKEYBOARD[0x41]=0;
      break;
    case 12: //I
      BTKEYBOARD[0x2e]=0;
      break;
    case 13: //J
      BTKEYBOARD[0x42]=0;
      break;
    case 14: //K
      BTKEYBOARD[0x43]=0;
      break;
    case 15: //L
      BTKEYBOARD[0x44]=0;
      break;
    case 16: //M
      BTKEYBOARD[0x52]=0;
      break;
    case 17: //N
      BTKEYBOARD[0x51]=0;
      break;
    case 18: //O
      BTKEYBOARD[0x2f]=0;
      break;
    case 19: //P
      BTKEYBOARD[0x30]=0;
      break;
    case 20: //Q
      BTKEYBOARD[0x27]=0;
      break;
    case 21: //R
      BTKEYBOARD[0x2a]=0;
      break;
    case 22: //S
      BTKEYBOARD[0x3d]=0;
      break;
    case 23: //T
      BTKEYBOARD[0x2b]=0;
      break;
    case 24: //U
      BTKEYBOARD[0x2d]=0;
      break;
    case 25: //V
      BTKEYBOARD[0x4f]=0;
      break;
    case 26: //W
      BTKEYBOARD[0x28]=0;
      break;
    case 27: //X
      BTKEYBOARD[0x4d]=0;
      break;
    case 28: //Y
      BTKEYBOARD[0x2c]=0;
      break;
    case 29: //Z
      BTKEYBOARD[0x4c]=0;
      break;
    case 30: //1
      BTKEYBOARD[0x12]=0;
      break;
    case 31: //2
      BTKEYBOARD[0x13]=0;
      break;
    case 32: //3
      BTKEYBOARD[0x14]=0;
      break;
    case 33: //4
      BTKEYBOARD[0x15]=0;
      break;
    case 34: //5
      BTKEYBOARD[0x16]=0;
      break;
    case 35: //6
      BTKEYBOARD[0x17]=0;
      break;
    case 36: //7
      BTKEYBOARD[0x18]=0;
      break;
    case 37: //8
      BTKEYBOARD[0x19]=0;
      break;
    case 38: //9
      BTKEYBOARD[0x1a]=0;
      break;
    case 39: //0
      BTKEYBOARD[0x1b]=0;
      break;
   case 40: //ENTER
      BTKEYBOARD[0x47]=0;
      break;
    case 41: //ESC
      BTKEYBOARD[0x01]=0;
      break;
    case 42: //BACKSPACE
      BTKEYBOARD[0x1e]=0;
      break;
    case 43: //TAB
      BTKEYBOARD[0x26]=0;
      break;
    case 44: //SPACEBAR
      BTKEYBOARD[0x5e]=0;
      break;
    case 45: //-
      BTKEYBOARD[0x1c]=0;
      break;
    case 46: //=
      BTKEYBOARD[0x1d]=0;
      break;
    case 47: //[ 
      BTKEYBOARD[0x31]=0;
      break;
    case 48: //]
      BTKEYBOARD[0x32]=0;
      break;
/// case 49:
///   BTKEYBOARD[]=0;
///   break;
    case 50: //\\
      BTKEYBOARD[0x33]=0;
      break;
    case 51: //;
      BTKEYBOARD[0x45]=0;
      break;
    case 52: //'
      BTKEYBOARD[0x46]=0;
      break;
    case 53: //~
      BTKEYBOARD[0x11]=0;
      break;
    case 54: //,
      BTKEYBOARD[0x53]=0;
      break;
    case 55: //.
      BTKEYBOARD[0x54]=0;
      break;
    case 56: ///
      BTKEYBOARD[0x55]=0;
      break;
/// case 57:
///   BTKEYBOARD[]=0;
///   break;
    case 58: //F1
      BTKEYBOARD[2]=0;
      break;
    case 59: //F2
      BTKEYBOARD[3]=0;
      break;
    case 60: //F3
      BTKEYBOARD[4]=0;
      break;
    case 61: //F4
      BTKEYBOARD[5]=0;
      break;
    case 62: //F5
      BTKEYBOARD[6]=0;
      break;
    case 63: //F6
      BTKEYBOARD[7]=0;
      break;
    case 64: //F7
      BTKEYBOARD[8]=0;
      break;
    case 65: //F8
      BTKEYBOARD[9]=0;
      break;
    case 66: //F9
      BTKEYBOARD[0x0a]=0;
      break;
    case 67: //F10
      BTKEYBOARD[0x0b]=0;
      break;
    case 68: //F11
      BTKEYBOARD[0x0c]=0;
      break;
    case 69: //F12
      BTKEYBOARD[0x0d]=0;
      break;
    case 70: //PRINTSCR ?
      BTKEYBOARD[0x0e]=0;
      break;
/// case 71:
///   BTKEYBOARD[]=0;
///   break;
/// case 72:
///   BTKEYBOARD[]=0;
///   break;
    case 73: //INSERT
      BTKEYBOARD[0x1f]=0;
      break;
    case 74: //HOME ?
      BTKEYBOARD[0x20]=0;
      break;
    case 75: //PAGEUP ?
      BTKEYBOARD[0x21]=0;
      break;
    case 76: //DELETE
      BTKEYBOARD[0x34]=0;
      break;
    case 77: //END ?
      BTKEYBOARD[0x35]=0;
      break;
    case 78: //PAGEDOWN ?
      BTKEYBOARD[0x36]=0;
      break;
    case 79: //RIGHT
      BTKEYBOARD[0x63]=0;
      break;
    case 80: //LEFT
      BTKEYBOARD[0x61]=0;
      break;
    case 81: //DOWN
      BTKEYBOARD[0x62]=0;
      break;
    case 82: //UP
      BTKEYBOARD[0x57]=0;
      break;
      
    case 100: //\\
      BTKEYBOARD[0x33]=0;
      break;
    default:
      break;
  }  

#if FAKE86_EMULATOR
  if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD();
#endif
  return 1;
}
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
void MY_uni_hid_parser_generic_init_report(uni_hid_device_t* d) {
  // Reset old state. Each report contains a full-state.
  d->gamepad.updated_states = 0;
}

uint8_t prev_key_pos=0;
uint8_t oldfunctkeys=0;

#if BT_RAW_PARSER
//---------------------------------------------------------------------------------
void MY_uni_hid_parser_generic_parse_raw(uni_hid_device_t* d, const uint8_t* report, uint16_t len) {
  if (DEBUG>4) {
    for (uint8_t tmp=0;tmp<len;tmp++) printf("%d ",report[tmp]);
    printf(" LEN: %d\n",len);
  }
//----------------------------------------------------------------------------------------
  if (len==10) { //DS4 JOYSTICK
    if ( report[5] % 8 == 1) {JOY_UP=1; JOY_RIGHT=1;}
    else if ( report[5] % 8 == 2) {JOY_RIGHT=1;}
    else if ( report[5] % 8 == 3) {JOY_RIGHT=1; JOY_DOWN=1;}
    else if ( report[5] % 8 == 4) {JOY_DOWN=1;}
    else if ( report[5] % 8 == 5) {JOY_DOWN=1; JOY_LEFT=1;}
    else if ( report[5] % 8 == 6) {JOY_LEFT=1;}
    else if ( report[5] % 8 == 7) {JOY_LEFT=1; JOY_UP=1;} 
    else if ((report[5] & 8) == 8) {JOY_UP=0;JOY_RIGHT=0;JOY_DOWN=0;JOY_LEFT=0;} 
    else {JOY_UP=1;}
    
    if ((report[5] & 16) == 16) {JOY_A=1;} 
    else {JOY_A=0;}
    if ((report[5] & 32) == 32) {JOY_B=1;} 
    else {JOY_B=0;}
    if ((report[5] & 64) == 64) {JOY_X=1;} 
    else {JOY_X=0;}
    if ((report[5] & 128) == 128) {JOY_Y=1;} 
    else {JOY_Y=0;}

    if ((report[6] & 16) == 16) {JOY_START=1;} 
    else {JOY_START=0;}
    if ((report[6] & 32) == 32) {JOY_SELECT=1;} 
    else {JOY_SELECT=0;}
    
    if ((report[7] & 1) == 1) {JOY_SYSTEM=1;} 
    else {JOY_SYSTEM=0;}
  }
//----------------------------------------------------------------------------------------
  else if (len==9) { //KEYBOARD 
    uint8_t pos=3;
    uint8_t value=0;
    while (report[pos]!=0 && pos<len) {
      if (report[pos]==1) { ///keyboard overflow
          for (uint8_t tmp_a=0;tmp_a<6;tmp_a++) {
            if (BTBUFFER_SET[tmp_a]!=0) {
              //release this key... from buffer
              BLUETOOTH_KEYBOARD_RELEASE(BTBUFFER_SET[tmp_a]); ///
              BTBUFFER_SET[tmp_a]=0;
            }
          }  
          break;
      }
      value=report[pos];
      pos++;
    }
//--------------------------------------------------------------------------------
    if (prev_key_pos>pos) {
      printf(">>> SOME KEY RELEASED.\n");
      uint8_t NOTFOUND=0;
      for (uint8_t tmp_a=0;tmp_a<6;tmp_a++) {
        if (BTBUFFER_SET[tmp_a]==0) continue;
    
        NOTFOUND=BTBUFFER_SET[tmp_a];
    
        for (uint8_t tmp_b=0;tmp_b<6;tmp_b++) {
           if (report[tmp_b+3]==0) break;       
           if (NOTFOUND==report[tmp_b+3]) {
              NOTFOUND=0;    
              break;
           }
        }
        if (NOTFOUND!=0) break;
      }
      if (NOTFOUND!=0) {
        for (uint8_t tmp_a=0;tmp_a<6;tmp_a++) {
          if (BTBUFFER_SET[tmp_a]==NOTFOUND) BTBUFFER_SET[tmp_a]=0;
        }
        printf(">>> RELEASED KEY: %d\n",NOTFOUND);
        BLUETOOTH_KEYBOARD_RELEASE(NOTFOUND); ///
      }
//--------------------------------------------------------------------------------
   } else if (prev_key_pos<pos) {
     printf(">>> SOME KEY PRESSED: %d\n",value);
   
     BLUETOOTH_KEYBOARD_PRESS(value); ///
  
     for (uint8_t tmp_a=0;tmp_a<6;tmp_a++) {
       if (BTBUFFER_SET[tmp_a]==0) {
         BTBUFFER_SET[tmp_a]=value;
         break;
       }
     }  
   }
//--------------------------------------------------------------------------------
   if (report[1]!=oldfunctkeys) {
   
   if ((report[1]&1)==1) { //LCTRL
      BTKEYBOARD[0x5c]=1;
   } else { 
      BTKEYBOARD[0x5c]=0;         
   }
   if ((report[1]&2)==2) { //LSHIFT
      BTKEYBOARD[0x4b]=1;
   } else {
      BTKEYBOARD[0x4b]=0;         
   }
   if ((report[1]&4)==4) { //LALT
      BTKEYBOARD[0x5d]=1;
   } else {
      BTKEYBOARD[0x5d]=0;         
   }
   if ((report[1]&8)==8) { //LGUI
      BTKEYBOARD[0x66]=1;
   } else {
      BTKEYBOARD[0x66]=0;         
   }
   if ((report[1]&16)==16) { //RCTRL
      BTKEYBOARD[0x60]=1;
   } else {
      BTKEYBOARD[0x60]=0;         
   }
   if ((report[1]&32)==32) { //RSHIFT
      BTKEYBOARD[0x56]=1;
   } else {
      BTKEYBOARD[0x56]=0;         
   }
   if ((report[1]&64)==64) { //RALT
      BTKEYBOARD[0x5F]=1;
   } else {
      BTKEYBOARD[0x5F]=0;         
   }
   if ((report[1]&128)==128) { //RGUI
      BTKEYBOARD[0x67]=1;
   } else {
      BTKEYBOARD[0x67]=0;         
   }
#if FAKE86_EMULATOR
   if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD(); ///
#endif
   }
   oldfunctkeys=report[1];
   
   prev_key_pos=pos;
//--------------------------------------------------------------------------------
  }
}
#else 
//---------------------------------------------------------------------------------
void MY_uni_hid_parser_generic_parse_usage(uni_hid_device_t* d, hid_globals_t* globals, uint16_t usage_page, uint16_t usage, int32_t value) {
  uint8_t REPORTID=globals->report_id; //1=JOYSTICK or KEYBOARD, 2=MOUSE 
  switch (usage_page) {
//--------------------------------------------------------------------------------          
    case HID_USAGE_PAGE_GENERIC_DESKTOP:
      switch (usage) {
        case HID_USAGE_AXIS_X:
          if (REPORTID==2) { // = MOUSE   
            MOUSEX+=value; 
          } else { // = JOYSTICK
            AXISX=value;
          }
          break;
        case HID_USAGE_AXIS_Y:
          if (REPORTID==2) { // = MOUSE
            MOUSEY+=value; 
          } else { // = JOYSTICK            
            AXISY=value;
          }
          break;
        case HID_USAGE_AXIS_Z: 
          break;
        case HID_USAGE_AXIS_RX:  // duplicate of AXIS_Z
          break;
        case HID_USAGE_AXIS_RY:  // duplicate of AXIS_RZ
          break;
        case HID_USAGE_AXIS_RZ:
          break;
        case HID_USAGE_HAT:
          if (REPORTID==1) { //JOYSTICK
            JOYBUTTON=value;
            if (JOYBUTTON!=JOYBUTTON_LAST) {JOY_UP=0;JOY_RIGHT=0;JOY_DOWN=0;JOY_LEFT=0;}
            if ( JOYBUTTON<8 && JOYBUTTON % 8 == 0) {JOY_UP=1;}
            if ( JOYBUTTON % 8 == 1) {JOY_UP=1; JOY_RIGHT=1;}
            if ( JOYBUTTON % 8 == 2) {JOY_RIGHT=1;}
            if ( JOYBUTTON % 8 == 3) {JOY_RIGHT=1; JOY_DOWN=1;}
            if ( JOYBUTTON % 8 == 4) {JOY_DOWN=1;}
            if ( JOYBUTTON % 8 == 5) {JOY_DOWN=1; JOY_LEFT=1;}
            if ( JOYBUTTON % 8 == 6) {JOY_LEFT=1;}
            if ( JOYBUTTON % 8 == 7) {JOY_LEFT=1; JOY_UP=1;}            
          }  
          break;
        case HID_USAGE_DPAD_UP:
        case HID_USAGE_DPAD_DOWN:
        case HID_USAGE_DPAD_RIGHT:
        case HID_USAGE_DPAD_LEFT:
          break;
//-------------------------------------------------------------------------------
        case 0x38: //MOUSE WHEEL Z-POSITION
          if ((int8_t)value>0) MOUSEZ+=value; //+1
          else if ((int8_t)value<0) MOUSEZ+=value; //-1
          else if (value==0) {} //MOUSEMOVE do nothing
          break;
//-------------------------------------------------------------------------------
        default:
          logi("Generic: Unsupported page: 0x%04x, usage: 0x%04x, value=0x%x\n", usage_page, usage, value);
          break;
      }
      if (REPORTID==2) {
        //printf("MOUSE X: %d, MOUSE Y: %d, MOUSE Z: %d\n",MOUSEX,MOUSEY,MOUSEZ);  //MOUSEMOVE
      } 
      if (REPORTID==1 && JOYBUTTON!=JOYBUTTON_LAST) { //JOYSTICK
        JOYBUTTON_LAST=JOYBUTTON;     
        //printf("UP: %d, DOWN: %d, LEFT: %d, RIGHT: %d\n",JOY_UP,JOY_DOWN,JOY_LEFT,JOY_RIGHT);  ///JOYSTICK DPAD CHANGE        
      }
      break;
//--------------------------------------------------------------------------------            
    case HID_USAGE_PAGE_SIMULATION_CONTROLS: //PEDALs
      switch (usage) {
        case HID_USAGE_ACCELERATOR: 
          break;
        case HID_USAGE_BRAKE:
          break;
        default:
          logi("Generic: Unsupported page: 0x%04x, usage: 0x%04x, value=0x%x\n", usage_page, usage, value);
          break;
      };
      break;
//--------------------------------------------------------------------------------      
    case HID_USAGE_PAGE_KEYBOARD_KEYPAD: //KEYBOARD 
      switch (usage) {
          case 0: //status is some keys released             
          
            if (BTBUF_POS>=0 && BTBUF_POS<7) BTBUF_POS++;  //
            if (BTBUF_POS>=6) BTBUF_POS=-7;                //KEYBOARD PRESS DONE waiting for release
            if (BTBUF_POS>=-7 && BTBUF_POS<0) BTBUF_POS++; //
            
            //if (BTBUF_POS>=1 && BTBUF_POS<6) {           //NO NEW KEYS PRESSED
            //}
            //if (BTBUF_POS!=-6 && BTBUF_POS<0) {          // SOME KEY RELEASED OR MORE THEN ONE PRESSED 
            //}    
            
            if (BTBUF_POS==5) { //SOME KEYS ACTIVE
              for (uint8_t tmp=0;tmp<6;tmp++) {
                if (BTBUFFER_CLEAR[tmp]!=0) {
                  //printf(">>>>>>> KEY RELEASED: %d \n",BTBUFFER_CLEAR[tmp]); //CLEAR REQUEST BUFFER CHECK                  
                  BLUETOOTH_KEYBOARD_RELEASE(BTBUFFER_CLEAR[tmp]);              //RELEASED KEY

                  BTBUFFER_CLEAR[tmp]=0; //CLEAR LAST VALUE NOTHING TO MOVE
                  BTBUFFER_SET[tmp]=0;                        
                }                 
              }              
            }            
            if (BTBUF_POS==0) { //ALL KEYS RELEASED
              //printf(">>>>>>> ALL KEYS RELEASED \n");
              for (uint8_t tmp=0;tmp<6;tmp++) {
                //if (BTBUFFER_SET[tmp]!=0) printf(">>>>>>> KEY RELEASED: %d \n",BTBUFFER_SET[tmp]); //
                BLUETOOTH_KEYBOARD_RELEASE(BTBUFFER_SET[tmp]); //RELEASED KEY
                  
                BTBUFFER_CLEAR[tmp]=0;
                BTBUFFER_SET[tmp]=0;
              }
            }                               
          break;
        case 0xe0:  // keyboard left control
          if (value) {
            BTKEYBOARD[0x5c]=1;
            logi("Generic keyboard: LEFT CTRL PRESSED.\n"); ///
          } else { //status is released 
            BTKEYBOARD[0x5c]=0;                      
          }  
          break;
        case 0xe1:  // keyboard left shift
          if (value) {
            BTKEYBOARD[0x4b]=1;
            logi("Generic keyboard: LEFT SHIFT PRESSED.\n"); ///
          } else { //status is released 
            BTKEYBOARD[0x4b]=0;
          }  
          break;
        case 0xe2:  // keyboard left alt
          if (value) {
            BTKEYBOARD[0x5d]=1;
            logi("Generic keyboard: LEFT ALT PRESSED.\n"); ///
          } else { //status is released 
            BTKEYBOARD[0x5d]=0;
          }  
          break;
        case 0xe3:  // keyboard left GUI
          if (value) {
            BTKEYBOARD[0x66]=1;
            logi("Generic keyboard: LEFT GUI PRESSED.\n"); ///
          } else { //status is released 
            BTKEYBOARD[0x66]=0;
          }  
          break;
        case 0xe4:  // keyboard right control
          if (value) {
            BTKEYBOARD[0x60]=1;
            logi("Generic keyboard: RIGHT CTRL PRESSED.\n"); ///
          } else { //status is released
            BTKEYBOARD[0x60]=0;
          }  
          break;
        case 0xe5:  // keyboard right shift
          if (value) {
            BTKEYBOARD[0x56]=1;
            logi("Generic keyboard: RIGHT SHIFT PRESSED.\n"); ///
          } else { //status is released 
            BTKEYBOARD[0x56]=0;
          }  
          break;
        case 0xe6:  // keyboard right alt
          if (value) {
            BTKEYBOARD[0x5f]=1;
            logi("Generic keyboard: RIGHT ALT PRESSED.\n"); ///
          } else { //status is released 
            BTKEYBOARD[0x5f]=0;
          }  
          break;
        case 0xe7:  // keyboard right GUI
          if (value) {
            BTKEYBOARD[0x67]=1;
            logi("Generic keyboard: RIGHT GUI PRESSED.\n"); ///
          } else { //status is released 
            BTKEYBOARD[0x67]=0;
          }  
          break;
        case 0x01: //KEYBOARD KEYPRESS OVERFLOW
          break;  
        default: //SEVERAL KEYS
          if (value) {
//________________________________________________________________________________
            if (BTBUF_POS==-6) { /// SOME KEY RELEASED OR MORE THEN ONE PRESSED                               
              //ADD ALL KEYS TO CLEAR REQUEST BUFFER
              for (uint8_t tmp=0;tmp<6;tmp++) BTBUFFER_CLEAR[tmp]=BTBUFFER_SET[tmp]; 
            }   
            
            if (BTBUF_POS<0) BTBUF_POS=0;                 //PRESSED MORE KEYS AND CLEAR BUFFER NOT DONE
            if (BTBUF_POS>=0 && BTBUF_POS<6) BTBUF_POS++; //MORE THAN ONE KEY PRESSED             
            if (BTBUF_POS>=1 && BTBUF_POS<=6) {         /// KEY PRESSED: min. 1 max. 6                
              uint8_t BTBUF_FOUND=0; //0=not found, 1=found
              for (uint8_t tmp=0;tmp<6;tmp++) { //check if exist
                 if (BTBUFFER_SET[tmp]==usage) {
                   BTBUF_FOUND=1;
                   BTBUFFER_CLEAR[tmp]=0; //UNSET CLEAR REQUEST
                   break;
                 } 
              }
              if (BTBUF_FOUND==0) { //not exist                
                for (uint8_t tmp=0;tmp<6;tmp++) {
                  if (BTBUFFER_SET[tmp]==0) {
                    //printf(">>>>>>> KEY PRESSED: %d \n",usage);
                    BLUETOOTH_KEYBOARD_PRESS(usage); //PRESSED NEW KEY
                     
                    BTBUFFER_SET[tmp]=usage;
                    BTBUFFER_CLEAR[tmp]=0; //UNSET CLEAR REQUEST
                    break;
                  }                    
                }                 
              }                
            }                 
          }
          break;    
      }
      break;
//--------------------------------------------------------------------------------            
    case HID_USAGE_PAGE_GENERIC_DEVICE_CONTROLS:
      switch (usage) {
        case HID_USAGE_BATTERY_STRENGHT:
          break;
        default:
          logi("Generic: Unsupported page: 0x%04x, usage: 0x%04x, value=0x%x\n", usage_page, usage, value);
          break;
      }
      break;
//--------------------------------------------------------------------------------            
    case HID_USAGE_PAGE_BUTTON: {
      switch (usage) {
        case 0x01:  // Button A = MOUSE LEFT BUTTON
          if (REPORTID==2) { //MOUSE
            if (value) { 
              MOUSEBUTTONS|=1;
            } else {
              MOUSEBUTTONS&=0b11111110;
            }  
          } else if (REPORTID==1) { //JOYSTICK
            if (value) { 
              BUTTONS|=1;
              JOY_A=1;
            } else {
              BUTTONS&=0b11111110;
              JOY_A=0;
            }  
          }
          break;
        case 0x02:  // Button B = MOUSE RIGHT BUTTON
          if (REPORTID==2) { //MOUSE
            if (value) { 
              MOUSEBUTTONS|=2;
            } else {
              MOUSEBUTTONS&=0b11111101;
            }  
          } else if (REPORTID==1) { //JOYSTICK
            if (value) {
              BUTTONS|=2;
              JOY_B=1;            
            } else {
              BUTTONS&=0b11111101;
              JOY_B=0;            
            }
          }  
          break;
        case 0x03:  // non-existant button C? = MOUSE MIDDLE BUTTON
          if (REPORTID==2) { //MOUSE
            if (value) { 
              MOUSEBUTTONS|=4;
            } else {
              MOUSEBUTTONS&=0b11111011;
            }  
          } else if (REPORTID==1) { //JOYSTICK
            if (value) {
              BUTTONS|=4;
              JOY_C=1;            
            } else {
              BUTTONS&=0b11111011;
              JOY_C=0;            
            }
          }  
          break;
        case 0x04:  // Button X
          if (REPORTID==2) { //MOUSE
            if (value) { 
              MOUSEBUTTONS|=8;
            } else {
              MOUSEBUTTONS&=0b11110111;
            }  
          } else if (REPORTID==1) { //JOYSTICK
            if (value) {
              BUTTONS|=8;
              JOY_X=1;            
            } else {
              BUTTONS&=0b11110111;
              JOY_X=0;            
            }
          }            
          break;
        case 0x05:  // Button Y          
          if (REPORTID==2) { //MOUSE
            if (value) { 
              MOUSEBUTTONS|=16;
            } else {
              MOUSEBUTTONS&=0b11101111;
            }  
          } else if (REPORTID==1) { //JOYSTICK
            if (value) {
              BUTTONS|=16;
              JOY_Y=1;            
            } else {
              BUTTONS&=0b11101111;
              JOY_Y=0;            
            }
          }  
          break;
        case 0x06:  // non-existant button Z?
          if (REPORTID==2) { //MOUSE
            if (value) { 
              MOUSEBUTTONS|=32;
            } else {
              MOUSEBUTTONS&=0b11011111;
            }  
          } else if (REPORTID==1) { //JOYSTICK
            if (value) {
              BUTTONS|=32;
              JOY_Z=1;
            } else {
              BUTTONS&=0b11011111;
              JOY_Z=0; 
            }
          }  
          break;
        case 0x07: //LEFT SHOULDER
          if (REPORTID==2) { //MOUSE
            if (value) { 
              MOUSEBUTTONS|=64;
            } else {
              MOUSEBUTTONS&=0b10111111;
            }  
          } else if (REPORTID==1) { //JOYSTICK
            if (value) {
              BUTTONS|=64;
              JOY_L=1;
            } else {
              BUTTONS&=0b10111111;
              JOY_L=0;
            }
          }       
          break;
        case 0x08: //RIGHT SHOULDER
          if (REPORTID==2) { //MOUSE
            if (value) { 
              MOUSEBUTTONS|=128;
            } else {
              MOUSEBUTTONS&=0b01111111;
            }  
          } else if (REPORTID==1) { //JOYSTICK
            if (value) {
              BUTTONS|=128;
              JOY_R=1;
            } else {
              BUTTONS&=0b01111111;
              JOY_R=0;
            } 
          }  
          break;
        case 0x09:  // ??? START
          if (value) { 
            JOY_START=1;
          } else {
            JOY_START=0;
          }          
          break;
        case 0x0a:  // ??? SELECT
          if (value) { 
            JOY_SELECT=1;
          } else {
            JOY_SELECT=0;
          }          
          break;
        case 0x0b:  // select button ?          
          break;
        case 0x0c:  // start button ? // = TOUCH PAD
          break;
        case 0x0d: //JOYSTICK HOME SYSTEM BUTTON
          if (value) { 
            JOY_SYSTEM=1;
          } else {
            JOY_SYSTEM=0;
          }
          break;
        case 0x0e: //LEFT THUMB 
          break;
        case 0x0f: //RIGHT THUMB 
          break;
        case 0x10: // unsupported
          break;
        default:
          logi("Generic: Unsupported page: 0x%04x, usage: 0x%04x, value=0x%x\n", usage_page, usage, value);
          break;
      }      
      if (BUTTONS!=BUTTONS_LAST) { //check if changed...
        BUTTONS_LAST=BUTTONS;
        //printf("BUTTONS: %d\n",BUTTONS); //BUTTONS KEYPRESS        
      }  
      if (MOUSEBUTTONS!=MOUSEBUTTONS_LAST) { //check if changed...
        MOUSEBUTTONS_LAST=MOUSEBUTTONS;
        //printf("MOUSEBUTTONS: %d\n",MOUSEBUTTONS); //MOUSE BUTTONS KEYPRESS        
      }        
      break;
    }
//--------------------------------------------------------------------------------          
    case HID_USAGE_PAGE_CONSUMER: //?
      switch (usage) {
        case HID_USAGE_FAST_FORWARD:
          break;
        case HID_USAGE_REWIND:
          break;
        case HID_USAGE_PLAY_PAUSE:
          break;
        case HID_USAGE_AC_SEARCH:
          break;
        case HID_USAGE_AC_HOME:
          break;
        case HID_USAGE_AC_BACK:
          break;
//--------------------------------------------------------------------------------               
        case 0x238: //do nothing (MOUSE)
          break;
//--------------------------------------------------------------------------------                
        default:
          logi("Generic: Unsupported page: 0x%04x, usage: 0x%04x, value=0x%x\n", usage_page, usage, value);
          break;
      }
      break;
//--------------------------------------------------------------------------------      
    // unknown usage page
    default:
      logi("Generic: Unsupported page: 0x%04x, usage: 0x%04x, value=0x%x\n", usage_page, usage, value);
      break;
  } 
}
#endif


//********************************************************************************
uint8_t scancode = 0;
boolean keyup = false;

uint8_t PS2_KEYBOARD_PRESS(uint8_t KEY) {
  #if DEBUG>4
    printf("BTKEY: %d\n",KEY); ///
  #endif  
  switch (KEY) {
    case 0x14: //CTRL
      PS2KEYBOARD[0x5C]=1;
      break;
    case 0x11: //ALT
      PS2KEYBOARD[0x5D]=1;
      break;
    case 0x12: //SHIFT
      PS2KEYBOARD[0x4B]=1;
      break;
    
    case 0x1C: //A
      PS2KEYBOARD[0x3c]=1;
      break;
    case 0x32: //B
      PS2KEYBOARD[0x50]=1;
      break;
    case 0x21: //C
      PS2KEYBOARD[0x4e]=1;
      break;
    case 0x23: //D
      PS2KEYBOARD[0x3e]=1;
      break;
    case 0x24: //E
      PS2KEYBOARD[0x29]=1;
      break;
    case 0x2B: //F
      PS2KEYBOARD[0x3f]=1;
      break;
    case 0x34: //G
      PS2KEYBOARD[0x40]=1;
      break;
    case 0x33: //H
      PS2KEYBOARD[0x41]=1;
      break;
    case 0x43: //I
      PS2KEYBOARD[0x2e]=1;
      break;
    case 0x3B: //J
      PS2KEYBOARD[0x42]=1;
      break;
    case 0x42: //K
      PS2KEYBOARD[0x43]=1;
      break;
    case 0x4B: //L
      PS2KEYBOARD[0x44]=1;
      break;
    case 0x3A: //M
      PS2KEYBOARD[0x52]=1;
      break;
    case 0x31: //N
      PS2KEYBOARD[0x51]=1;
      break;
    case 0x44: //O
      PS2KEYBOARD[0x2f]=1;
      break;
    case 0x4D: //P
      PS2KEYBOARD[0x30]=1;
      break;
    case 0x15: //Q
      PS2KEYBOARD[0x27]=1;
      break;
    case 0x2D: //R
      PS2KEYBOARD[0x2a]=1;
      break;
    case 0x1B: //S
      PS2KEYBOARD[0x3d]=1;
      break;
    case 0x2C: //T
      PS2KEYBOARD[0x2b]=1;
      break;
    case 0x3C: //U
      PS2KEYBOARD[0x2d]=1;
      break;
    case 0x2A: //V
      PS2KEYBOARD[0x4f]=1;
      break;
    case 0x1D: //W
      PS2KEYBOARD[0x28]=1;
      break;
    case 0x22: //X
      PS2KEYBOARD[0x4d]=1;
      break;
    case 0x35: //Y
      PS2KEYBOARD[0x2c]=1;
      break;
    case 0x1A: //Z
      PS2KEYBOARD[0x4c]=1;
      break;
    case 0x16: //1
      PS2KEYBOARD[0x12]=1;
      break;
    case 0x1e: //2
      PS2KEYBOARD[0x13]=1;
      break;
    case 0x26: //3
      PS2KEYBOARD[0x14]=1;
      break;
    case 0x25: //4
      PS2KEYBOARD[0x15]=1;
      break;
    case 0x2E: //5
      PS2KEYBOARD[0x16]=1;
      break;
    case 0x36: //6
      PS2KEYBOARD[0x17]=1;
      break;
    case 0x3D: //7
      PS2KEYBOARD[0x18]=1;
      break;
    case 0x3E: //8
      PS2KEYBOARD[0x19]=1;
      break;
    case 0x46: //9
      PS2KEYBOARD[0x1a]=1;
      break;
    case 0x45: //0
      PS2KEYBOARD[0x1b]=1;
      break;
   case 0x5A: //ENTER
      PS2KEYBOARD[0x47]=1;
      break;
    case 0x76: //ESC
      PS2KEYBOARD[0x01]=1;
      break;
    case 0x66: //BACKSPACE
      PS2KEYBOARD[0x1e]=1;
      break;
    case 0x0D: //TAB
      PS2KEYBOARD[0x26]=1;
      break;
    case 0x29: //SPACEBAR
      PS2KEYBOARD[0x5e]=1;
      break;
    case 0x4E: //-
      PS2KEYBOARD[0x1c]=1;
      break;
    case 0x55: //=
      PS2KEYBOARD[0x1d]=1;
      break;
    case 0x54: //[ 
      PS2KEYBOARD[0x31]=1;
      break;
    case 0x5B: //]
      PS2KEYBOARD[0x32]=1;
      break;
    case 0x5D: //\\
      PS2KEYBOARD[0x33]=1;
      break;
    case 0x4C: //;
      PS2KEYBOARD[0x45]=1;
      break;
    case 0x52: //'
      PS2KEYBOARD[0x46]=1;
      break;
    case 0x0E: //~
      PS2KEYBOARD[0x11]=1;
      break;
    case 0x41: //,
      PS2KEYBOARD[0x53]=1;
      break;
    case 0x49: //.
      PS2KEYBOARD[0x54]=1;
      break;
    case 0x4A: ///
      PS2KEYBOARD[0x55]=1;
      break;
    case 0x05: //F1
      PS2KEYBOARD[2]=1;
      break;
    case 0x06: //F2
      PS2KEYBOARD[3]=1;
      break;
    case 0x04: //F3
      PS2KEYBOARD[4]=1;
      break;
    case 0x0C: //F4
      PS2KEYBOARD[5]=1;
      break;
    case 0x03: //F5
      PS2KEYBOARD[6]=1;
      break;
    case 0x0B: //F6
      PS2KEYBOARD[7]=1;
      break;
    case 0x83: //F7
      PS2KEYBOARD[8]=1;
      break;
    case 0x0A: //F8
      PS2KEYBOARD[9]=1;
      break;
    case 0x01: //F9
      PS2KEYBOARD[0x0a]=1;
      break;
    case 0x09: //F10
      PS2KEYBOARD[0x0b]=1;
      break;
    case 0x78: //F11
      PS2KEYBOARD[0x0c]=1;
      break;
    case 0x07: //F12
      PS2KEYBOARD[0x0d]=1;
      break;
//    case 0x7C: //PRINTSCR  0xE012E07C
//      PS2KEYBOARD[0x0e]=1;
//      break;
   case 0x7E: ///SCROLL-LOCK
      PS2KEYBOARD[0xF]=1;
      break;
//   case 72:   //PAUSE-BREAK 0xE11477E1F0C1
//      PS2KEYBOARD[0c10]=1;
//      break;
    case 0x70: //INSERT     //0xE070
      PS2KEYBOARD[0x1f]=1;
      break;
    case 0x6C: //HOME ?    //0xE06C
      PS2KEYBOARD[0x20]=1; 
      break;
    case 0x7D: //PAGEUP ? //0xE07D
      PS2KEYBOARD[0x21]=1;
      break;
    case 0x71: //DELETE  //0xE071
      PS2KEYBOARD[0x34]=1;
      break;
    case 0x69: //END ?  //0xE069
      PS2KEYBOARD[0x35]=1;
      break;
    case 0x7A: //PAGEDOWN //0xE07A
      PS2KEYBOARD[0x36]=1;
      break;
      
    case 0x74: //RIGHT  //0xE074
      PS2KEYBOARD[0x63]=1;
      break;
    case 0x6B: //LEFT  //0xE06V
      PS2KEYBOARD[0x61]=1;
      break;
    case 0x72: //DOWN  //0xE072
      PS2KEYBOARD[0x62]=1;
      break;
    case 0x75: //UP    //0xE075
      PS2KEYBOARD[0x57]=1;
      break;
      
    default:
      break;
  }  
#if FAKE86_EMULATOR
  if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD();
#endif
  return 1;
}
//---------------------------------------------------------------------------------
uint8_t PS2_KEYBOARD_RELEASE(uint8_t KEY) {
  switch (KEY) {
    case 0x14: //CTRL
      PS2KEYBOARD[0x5C]=0;
      break;
    case 0x11: //ALT
      PS2KEYBOARD[0x5D]=0;
      break;
    case 0x12: //SHIFT
      PS2KEYBOARD[0x4B]=0;
      break;
    
    case 0x1C: //A
      PS2KEYBOARD[0x3c]=0;
      break;
    case 0x32: //B
      PS2KEYBOARD[0x50]=0;
      break;
    case 0x21: //C
      PS2KEYBOARD[0x4e]=0;
      break;
    case 0x23: //D
      PS2KEYBOARD[0x3e]=0;
      break;
    case 0x24: //E
      PS2KEYBOARD[0x29]=0;
      break;
    case 0x2B: //F
      PS2KEYBOARD[0x3f]=0;
      break;
    case 0x34: //G
      PS2KEYBOARD[0x40]=0;
      break;
    case 0x33: //H
      PS2KEYBOARD[0x41]=0;
      break;
    case 0x43: //I
      PS2KEYBOARD[0x2e]=0;
      break;
    case 0x3B: //J
      PS2KEYBOARD[0x42]=0;
      break;
    case 0x42: //K
      PS2KEYBOARD[0x43]=0;
      break;
    case 0x4B: //L
      PS2KEYBOARD[0x44]=0;
      break;
    case 0x3A: //M
      PS2KEYBOARD[0x52]=0;
      break;
    case 0x31: //N
      PS2KEYBOARD[0x51]=0;
      break;
    case 0x44: //O
      PS2KEYBOARD[0x2f]=0;
      break;
    case 0x4D: //P
      PS2KEYBOARD[0x30]=0;
      break;
    case 0x15: //Q
      PS2KEYBOARD[0x27]=0;
      break;
    case 0x2D: //R
      PS2KEYBOARD[0x2a]=0;
      break;
    case 0x1B: //S
      PS2KEYBOARD[0x3d]=0;
      break;
    case 0x2C: //T
      PS2KEYBOARD[0x2b]=0;
      break;
    case 0x3C: //U
      PS2KEYBOARD[0x2d]=0;
      break;
    case 0x2A: //V
      PS2KEYBOARD[0x4f]=0;
      break;
    case 0x1D: //W
      PS2KEYBOARD[0x28]=0;
      break;
    case 0x22: //X
      PS2KEYBOARD[0x4d]=0;
      break;
    case 0x35: //Y
      PS2KEYBOARD[0x2c]=0;
      break;
    case 0x1A: //Z
      PS2KEYBOARD[0x4c]=0;
      break;
    case 0x16: //1
      PS2KEYBOARD[0x12]=0;
      break;
    case 0x1e: //2
      PS2KEYBOARD[0x13]=0;
      break;
    case 0x26: //3
      PS2KEYBOARD[0x14]=0;
      break;
    case 0x25: //4
      PS2KEYBOARD[0x15]=0;
      break;
    case 0x2E: //5
      PS2KEYBOARD[0x16]=0;
      break;
    case 0x36: //6
      PS2KEYBOARD[0x17]=0;
      break;
    case 0x3D: //7
      PS2KEYBOARD[0x18]=0;
      break;
    case 0x3E: //8
      PS2KEYBOARD[0x19]=0;
      break;
    case 0x46: //9
      PS2KEYBOARD[0x1a]=0;
      break;
    case 0x45: //0
      PS2KEYBOARD[0x1b]=0;
      break;
   case 0x5A: //ENTER
      PS2KEYBOARD[0x47]=0;
      break;
    case 0x76: //ESC
      PS2KEYBOARD[0x01]=0;
      break;
    case 0x66: //BACKSPACE
      PS2KEYBOARD[0x1e]=0;
      break;
    case 0x0D: //TAB
      PS2KEYBOARD[0x26]=0;
      break;
    case 0x29: //SPACEBAR
      PS2KEYBOARD[0x5e]=0;
      break;
    case 0x4E: //-
      PS2KEYBOARD[0x1c]=0;
      break;
    case 0x55: //=
      PS2KEYBOARD[0x1d]=0;
      break;
    case 0x54: //[ 
      PS2KEYBOARD[0x31]=0;
      break;
    case 0x5B: //]
      PS2KEYBOARD[0x32]=0;
      break;
    case 0x5D: //\\
      PS2KEYBOARD[0x33]=0;
      break;
    case 0x4C: //;
      PS2KEYBOARD[0x45]=0;
      break;
    case 0x52: //'
      PS2KEYBOARD[0x46]=0;
      break;
    case 0x0E: //~
      PS2KEYBOARD[0x11]=0;
      break;
    case 0x41: //,
      PS2KEYBOARD[0x53]=0;
      break;
    case 0x49: //.
      PS2KEYBOARD[0x54]=0;
      break;
    case 0x4A: ///
      PS2KEYBOARD[0x55]=0;
      break;
    case 0x05: //F1
      PS2KEYBOARD[2]=0;
      break;
    case 0x06: //F2
      PS2KEYBOARD[3]=0;
      break;
    case 0x04: //F3
      PS2KEYBOARD[4]=0;
      break;
    case 0x0C: //F4
      PS2KEYBOARD[5]=0;
      break;
    case 0x03: //F5
      PS2KEYBOARD[6]=0;
      break;
    case 0x0B: //F6
      PS2KEYBOARD[7]=0;
      break;
    case 0x83: //F7
      PS2KEYBOARD[8]=0;
      break;
    case 0x0A: //F8
      PS2KEYBOARD[9]=0;
      break;
    case 0x01: //F9
      PS2KEYBOARD[0x0a]=0;
      break;
    case 0x09: //F10
      PS2KEYBOARD[0x0b]=0;
      break;
    case 0x78: //F11
      PS2KEYBOARD[0x0c]=0;
      break;
    case 0x07: //F12
      PS2KEYBOARD[0x0d]=0;
      break;
//    case 0x7C: //PRINTSCR  0xE012E07C
//      PS2KEYBOARD[0x0e]=0;
//      break;
   case 0x7E: ///SCROLL-LOCK
      PS2KEYBOARD[0xF]=0;
      break;
//   case 72:   //PAUSE-BREAK 0xE11477E1F0C1
//      PS2KEYBOARD[0c10]=0;
//      break;
    case 0x70: //INSERT     //0xE070
      PS2KEYBOARD[0x1f]=0;
      break;
    case 0x6C: //HOME ?    //0xE06C
      PS2KEYBOARD[0x20]=0; 
      break;
    case 0x7D: //PAGEUP ? //0xE07D
      PS2KEYBOARD[0x21]=0;
      break;
    case 0x71: //DELETE  //0xE071
      PS2KEYBOARD[0x34]=0;
      break;
    case 0x69: //END ?  //0xE069
      PS2KEYBOARD[0x35]=0;
      break;
    case 0x7A: //PAGEDOWN //0xE07A
      PS2KEYBOARD[0x36]=0;
      break;
      
    case 0x74: //RIGHT  //0xE074
      PS2KEYBOARD[0x63]=0;
      break;
    case 0x6B: //LEFT  //0xE06V
      PS2KEYBOARD[0x61]=0;
      break;
    case 0x72: //DOWN  //0xE072
      PS2KEYBOARD[0x62]=0;
      break;
    case 0x75: //UP    //0xE075
      PS2KEYBOARD[0x57]=0;
      break;
      
    default:
      break;
  }  

#if FAKE86_EMULATOR
  if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD();
#endif
  return 1;
}
//---------------------------------------------------------------------------------

//********************************************************************************
uint8_t KEY_BYTE_VAL=0;
uint8_t KEY_BIT_VAL=0;
uint8_t KEY_BIT_POS=0;
uint32_t KEY_BIT_TIMEOUT=0;
   
void IRAM_ATTR ps2kb_interruptHandler(void) {
#if PS2_KEYBOARD
  if (KEY_BIT_POS>0 && millis() - KEY_BIT_TIMEOUT>5) {
    Serial.println("TIMEOUT");
    KEY_BIT_TIMEOUT=millis();
    KEY_BIT_POS=0;
    KEY_BIT_VAL=0;
    KEY_BYTE_VAL=0;
  }

  int clock_ = digitalRead(PS2_KEYBOARD_CLK);
  if (clock_ == 0) {   
    Serial.println("LOST");
    return;
  }

  KEY_BIT_TIMEOUT=millis();

  if (KEY_BIT_POS>=1 && KEY_BIT_POS<9) {
  
    KEY_BIT_VAL = (digitalRead(PS2_KEYBOARD_DATA) & 1);
    KEY_BYTE_VAL|=(KEY_BIT_VAL<<(KEY_BIT_POS-1));
    Serial.println(KEY_BIT_VAL);
    KEY_BIT_POS++;  
  } else if (KEY_BIT_POS==9) { //PARITY
    KEY_BIT_VAL = (digitalRead(PS2_KEYBOARD_DATA) & 1);
    Serial.print("PARITY:");
    Serial.println(KEY_BIT_VAL);
    KEY_BIT_POS++;  
  } else if (KEY_BIT_POS==10) { //STOPBIT
    KEY_BIT_VAL = (digitalRead(PS2_KEYBOARD_DATA) & 1);
    Serial.print("STOPBIT:");
    Serial.println(KEY_BIT_VAL);

    Serial.print(" {");
    Serial.print(KEY_BYTE_VAL, HEX);
    Serial.println("} ");
  if (keyup==true)
    PS2_KEYBOARD_RELEASE(KEY_BYTE_VAL & 0xff);
  else 
    PS2_KEYBOARD_PRESS(KEY_BYTE_VAL & 0xff);
    if (KEY_BYTE_VAL  == 0xF0  ) {
      keyup = true;
    } else {
      keyup = false;
    }

    KEY_BYTE_VAL=0;
    KEY_BIT_POS=0;

  } else {
    Serial.println("END.");
    KEY_BIT_POS++;
  }

#endif
}

void PS2_kb_begin() {
#if PS2_KEYBOARD
  pinMode(PS2_KEYBOARD_DATA, INPUT_PULLUP);
  pinMode(PS2_KEYBOARD_CLK, INPUT_PULLUP);
  digitalWrite(PS2_KEYBOARD_DATA, 1);
  digitalWrite(PS2_KEYBOARD_CLK, 1);
  attachInterrupt(digitalPinToInterrupt(PS2_KEYBOARD_CLK), ps2kb_interruptHandler, RISING);
#endif
}
//********************************************************************************
