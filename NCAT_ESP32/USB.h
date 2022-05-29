//********************************************************************************
#if USBHOST
#if FAKE86_EMULATOR
void FAKE86_KEYBOARD();
#endif

extern uint8_t* USBKEYBOARD;

//--------------------------------------------------------------------------------
void KM_task_mouse(uint8_t Buttons, uint8_t Xdiff, uint8_t Ydiff, uint8_t Zdiff );
void KM_task_keyboard(uint8_t KEY, uint8_t STATE);
//--------------------------------------------------------------------------------
uint8_t USB_KEYBOARD_PRESS(uint8_t KEY) {
#if DEBUG>3
  printf("USBKEY PRESS: %d\n",KEY); ///
#endif  

  switch (KEY) {
/// case 1: //KEYBOARD OVERFLOW = not used
///   USBKEYBOARD[]=1;
///   break;
/// case 2:
///   USBKEYBOARD[]=1;
///   break;
/// case 3:
///   USBKEYBOARD[]=1;
///   break;
    case 4: //A
      USBKEYBOARD[0x3c]=1;
      break;
    case 5: //B
      USBKEYBOARD[0x50]=1;
      break;
    case 6: //C
      USBKEYBOARD[0x4e]=1;
      break;
    case 7: //D
      USBKEYBOARD[0x3e]=1;
      break;
    case 8: //E
      USBKEYBOARD[0x29]=1;
      break;
    case 9: //F
      USBKEYBOARD[0x3f]=1;
      break;
    case 10: //G
      USBKEYBOARD[0x40]=1;
      break;
    case 11: //H
      USBKEYBOARD[0x41]=1;
      break;
    case 12: //I
      USBKEYBOARD[0x2e]=1;
      break;
    case 13: //J
      USBKEYBOARD[0x42]=1;
      break;
    case 14: //K
      USBKEYBOARD[0x43]=1;
      break;
    case 15: //L
      USBKEYBOARD[0x44]=1;
      break;
    case 16: //M
      USBKEYBOARD[0x52]=1;
      break;
    case 17: //N
      USBKEYBOARD[0x51]=1;
      break;
    case 18: //O
      USBKEYBOARD[0x2f]=1;
      break;
    case 19: //P
      USBKEYBOARD[0x30]=1;
      break;
    case 20: //Q
      USBKEYBOARD[0x27]=1;
      break;
    case 21: //R
      USBKEYBOARD[0x2a]=1;
      break;
    case 22: //S
      USBKEYBOARD[0x3d]=1;
      break;
    case 23: //T
      USBKEYBOARD[0x2b]=1;
      break;
    case 24: //U
      USBKEYBOARD[0x2d]=1;
      break;
    case 25: //V
      USBKEYBOARD[0x4f]=1;
      break;
    case 26: //W
      USBKEYBOARD[0x28]=1;
      break;
    case 27: //X
      USBKEYBOARD[0x4d]=1;
      break;
    case 28: //Y
      USBKEYBOARD[0x2c]=1;
      break;
    case 29: //Z
      USBKEYBOARD[0x4c]=1;
      break;
    case 30: //1
      USBKEYBOARD[0x12]=1;
      break;
    case 31: //2
      USBKEYBOARD[0x13]=1;
      break;
    case 32: //3
      USBKEYBOARD[0x14]=1;
      break;
    case 33: //4
      USBKEYBOARD[0x15]=1;
      break;
    case 34: //5
      USBKEYBOARD[0x16]=1;
      break;
    case 35: //6
      USBKEYBOARD[0x17]=1;
      break;
    case 36: //7
      USBKEYBOARD[0x18]=1;
      break;
    case 37: //8
      USBKEYBOARD[0x19]=1;
      break;
    case 38: //9
      USBKEYBOARD[0x1a]=1;
      break;
    case 39: //0
      USBKEYBOARD[0x1b]=1;
      break;
   case 40: //ENTER
      USBKEYBOARD[0x47]=1;
      break;
    case 41: //ESC
      USBKEYBOARD[0x01]=1;
      break;
    case 42: //BACKSPACE
      USBKEYBOARD[0x1e]=1;
      break;
    case 43: //TAB
      USBKEYBOARD[0x26]=1;
      break;
    case 44: //SPACEBAR
      USBKEYBOARD[0x5e]=1;
      break;
    case 45: //-
      USBKEYBOARD[0x1c]=1;
      break;
    case 46: //=
      USBKEYBOARD[0x1d]=1;
      break;
    case 47: //[ 
      USBKEYBOARD[0x31]=1;
      break;
    case 48: //]
      USBKEYBOARD[0x32]=1;
      break;
/// case 49:
///   USBKEYBOARD[]=1;
///   break;
    case 50: //\\
      USBKEYBOARD[0x33]=1;
      break;
    case 51: //;
      USBKEYBOARD[0x45]=1;
      break;
    case 52: //'
      USBKEYBOARD[0x46]=1;
      break;
    case 53: //~
      USBKEYBOARD[0x11]=1;
      break;
    case 54: //,
      USBKEYBOARD[0x53]=1;
      break;
    case 55: //.
      USBKEYBOARD[0x54]=1;
      break;
    case 56: ///
      USBKEYBOARD[0x55]=1;
      break;
/// case 57:
///   USBKEYBOARD[]=1;
///   break;
    case 58: //F1
      USBKEYBOARD[2]=1;
      break;
    case 59: //F2
      USBKEYBOARD[3]=1;
      break;
    case 60: //F3
      USBKEYBOARD[4]=1;
      break;
    case 61: //F4
      USBKEYBOARD[5]=1;
      break;
    case 62: //F5
      USBKEYBOARD[6]=1;
      break;
    case 63: //F6
      USBKEYBOARD[7]=1;
      break;
    case 64: //F7
      USBKEYBOARD[8]=1;
      break;
    case 65: //F8
      USBKEYBOARD[9]=1;
      break;
    case 66: //F9
      USBKEYBOARD[0x0a]=1;
      break;
    case 67: //F10
      USBKEYBOARD[0x0b]=1;
      break;
    case 68: //F11
      USBKEYBOARD[0x0c]=1;
      break;
    case 69: //F12
      USBKEYBOARD[0x0d]=1;
      break;
    case 70: //PRINTSCR ?
      USBKEYBOARD[0x0e]=1;
      break;
/// case 71:
///   USBKEYBOARD[]=1;
///   break;
/// case 72:
///   USBKEYBOARD[]=1;
///   break;
    case 73: //INSERT
      USBKEYBOARD[0x1f]=1;
      break;
    case 74: //HOME ?
      USBKEYBOARD[0x20]=1;
      break;
    case 75: //PAGEUP ?
      USBKEYBOARD[0x21]=1;
      break;
    case 76: //DELETE
      USBKEYBOARD[0x34]=1;
      break;
    case 77: //END ?
      USBKEYBOARD[0x35]=1;
      break;
    case 78: //PAGEDOWN ?
      USBKEYBOARD[0x36]=1;
      break;
      
    case 79: //RIGHT
      USBKEYBOARD[0x63]=1;
      break;
    case 80: //LEFT
      USBKEYBOARD[0x61]=1;
      break;
    case 81: //DOWN
      USBKEYBOARD[0x62]=1;
      break;
    case 82: //UP
      USBKEYBOARD[0x57]=1;
      break;
      
    case 100: //\\
      USBKEYBOARD[0x33]=1;
      break;
    default:
      break;
  }  

if ((POWER==0 || POWER==APP_EMU_CLOCK) && KEY==0x47) { //ScrollLock pressed in menu
  MENU_ITEM=8; //Start KM Switch application
  MENU_ACTIVE=2;
}


#if FAKE86_EMULATOR
if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD();
#endif
if (POWER==APP_KMSWITCH) KM_task_keyboard(KEY,1); //PRESS

  return 1;
}
//---------------------------------------------------------------------------------
uint8_t USB_KEYBOARD_RELEASE(uint8_t KEY) {
#if DEBUG>3
  printf("USBKEY RELEASE: %d\n",KEY); ///
#endif  

  
  switch (KEY) {
/// case 1:
///   USBKEYBOARD[]=0;
///   break;
/// case 2:
///   USBKEYBOARD[]=0;
///   break;
/// case 3:
///   USBKEYBOARD[]=0;
///   break;
    case 4: //A
      USBKEYBOARD[0x3c]=0;
      break;
    case 5: //B
      USBKEYBOARD[0x50]=0;
      break;
    case 6: //C
      USBKEYBOARD[0x4e]=0;
      break;
    case 7: //D
      USBKEYBOARD[0x3e]=0;
      break;
    case 8: //E
      USBKEYBOARD[0x29]=0;
      break;
    case 9: //F
      USBKEYBOARD[0x3f]=0;
      break;
    case 10: //G
      USBKEYBOARD[0x40]=0;
      break;
    case 11: //H
      USBKEYBOARD[0x41]=0;
      break;
    case 12: //I
      USBKEYBOARD[0x2e]=0;
      break;
    case 13: //J
      USBKEYBOARD[0x42]=0;
      break;
    case 14: //K
      USBKEYBOARD[0x43]=0;
      break;
    case 15: //L
      USBKEYBOARD[0x44]=0;
      break;
    case 16: //M
      USBKEYBOARD[0x52]=0;
      break;
    case 17: //N
      USBKEYBOARD[0x51]=0;
      break;
    case 18: //O
      USBKEYBOARD[0x2f]=0;
      break;
    case 19: //P
      USBKEYBOARD[0x30]=0;
      break;
    case 20: //Q
      USBKEYBOARD[0x27]=0;
      break;
    case 21: //R
      USBKEYBOARD[0x2a]=0;
      break;
    case 22: //S
      USBKEYBOARD[0x3d]=0;
      break;
    case 23: //T
      USBKEYBOARD[0x2b]=0;
      break;
    case 24: //U
      USBKEYBOARD[0x2d]=0;
      break;
    case 25: //V
      USBKEYBOARD[0x4f]=0;
      break;
    case 26: //W
      USBKEYBOARD[0x28]=0;
      break;
    case 27: //X
      USBKEYBOARD[0x4d]=0;
      break;
    case 28: //Y
      USBKEYBOARD[0x2c]=0;
      break;
    case 29: //Z
      USBKEYBOARD[0x4c]=0;
      break;
    case 30: //1
      USBKEYBOARD[0x12]=0;
      break;
    case 31: //2
      USBKEYBOARD[0x13]=0;
      break;
    case 32: //3
      USBKEYBOARD[0x14]=0;
      break;
    case 33: //4
      USBKEYBOARD[0x15]=0;
      break;
    case 34: //5
      USBKEYBOARD[0x16]=0;
      break;
    case 35: //6
      USBKEYBOARD[0x17]=0;
      break;
    case 36: //7
      USBKEYBOARD[0x18]=0;
      break;
    case 37: //8
      USBKEYBOARD[0x19]=0;
      break;
    case 38: //9
      USBKEYBOARD[0x1a]=0;
      break;
    case 39: //0
      USBKEYBOARD[0x1b]=0;
      break;
   case 40: //ENTER
      USBKEYBOARD[0x47]=0;
      break;
    case 41: //ESC
      USBKEYBOARD[0x01]=0;
      break;
    case 42: //BACKSPACE
      USBKEYBOARD[0x1e]=0;
      break;
    case 43: //TAB
      USBKEYBOARD[0x26]=0;
      break;
    case 44: //SPACEBAR
      USBKEYBOARD[0x5e]=0;
      break;
    case 45: //-
      USBKEYBOARD[0x1c]=0;
      break;
    case 46: //=
      USBKEYBOARD[0x1d]=0;
      break;
    case 47: //[ 
      USBKEYBOARD[0x31]=0;
      break;
    case 48: //]
      USBKEYBOARD[0x32]=0;
      break;
/// case 49:
///   USBKEYBOARD[]=0;
///   break;
    case 50: //\\
      USBKEYBOARD[0x33]=0;
      break;
    case 51: //;
      USBKEYBOARD[0x45]=0;
      break;
    case 52: //'
      USBKEYBOARD[0x46]=0;
      break;
    case 53: //~
      USBKEYBOARD[0x11]=0;
      break;
    case 54: //,
      USBKEYBOARD[0x53]=0;
      break;
    case 55: //.
      USBKEYBOARD[0x54]=0;
      break;
    case 56: ///
      USBKEYBOARD[0x55]=0;
      break;
/// case 57:
///   USBKEYBOARD[]=0;
///   break;
    case 58: //F1
      USBKEYBOARD[2]=0;
      break;
    case 59: //F2
      USBKEYBOARD[3]=0;
      break;
    case 60: //F3
      USBKEYBOARD[4]=0;
      break;
    case 61: //F4
      USBKEYBOARD[5]=0;
      break;
    case 62: //F5
      USBKEYBOARD[6]=0;
      break;
    case 63: //F6
      USBKEYBOARD[7]=0;
      break;
    case 64: //F7
      USBKEYBOARD[8]=0;
      break;
    case 65: //F8
      USBKEYBOARD[9]=0;
      break;
    case 66: //F9
      USBKEYBOARD[0x0a]=0;
      break;
    case 67: //F10
      USBKEYBOARD[0x0b]=0;
      break;
    case 68: //F11
      USBKEYBOARD[0x0c]=0;
      break;
    case 69: //F12
      USBKEYBOARD[0x0d]=0;
      break;
    case 70: //PRINTSCR ?
      USBKEYBOARD[0x0e]=0;
      break;
/// case 71:
///   USBKEYBOARD[]=0;
///   break;
/// case 72:
///   USBKEYBOARD[]=0;
///   break;
    case 73: //INSERT
      USBKEYBOARD[0x1f]=0;
      break;
    case 74: //HOME ?
      USBKEYBOARD[0x20]=0;
      break;
    case 75: //PAGEUP ?
      USBKEYBOARD[0x21]=0;
      break;
    case 76: //DELETE
      USBKEYBOARD[0x34]=0;
      break;
    case 77: //END ?
      USBKEYBOARD[0x35]=0;
      break;
    case 78: //PAGEDOWN ?
      USBKEYBOARD[0x36]=0;
      break;
    case 79: //RIGHT
      USBKEYBOARD[0x63]=0;
      break;
    case 80: //LEFT
      USBKEYBOARD[0x61]=0;
      break;
    case 81: //DOWN
      USBKEYBOARD[0x62]=0;
      break;
    case 82: //UP
      USBKEYBOARD[0x57]=0;
      break;
      
    case 100: //\\
      USBKEYBOARD[0x33]=0;
      break;
    default:
      break;
  }  

#if FAKE86_EMULATOR
if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD();
#endif
if (POWER==APP_KMSWITCH) KM_task_keyboard(KEY,0);

  return 1;
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------


//********************************************************************************
#if !USB_UNIVERSAL_HID                  // USB_UNIVERSAL_HID disabled:
//********************************************************************************
uint8_t KEYBOARD_CONTROL_KEYS=0;
//---------------------------------------------------------------------------------

class MouseRptParser : public MouseReportParser
{
  protected:
    void OnMouseMove(MOUSEINFO *mi);
    void OnLeftButtonUp(MOUSEINFO *mi);
    void OnLeftButtonDown(MOUSEINFO *mi);
    void OnRightButtonUp(MOUSEINFO *mi);
    void OnRightButtonDown(MOUSEINFO *mi);
    void OnMiddleButtonUp(MOUSEINFO *mi);
    void OnMiddleButtonDown(MOUSEINFO *mi);
    void OnWheelMove(MOUSEINFO *mi); ///NCX:
    void CustomParser(uint8_t len, uint8_t *buf); ///NCX:
};


uint8_t mouse_Buttons=0;
int16_t mouse_Xdiff=0;
int16_t mouse_Ydiff=0;
int16_t mouse_Zdiff=0;
//---------------------------------------------------------------------------------
void MouseRptParser::CustomParser(uint8_t len, uint8_t *buf) {
  if (len==4) { // Buttons, X, Y, Z(Wheel) //CLASSIC MOUSE
    mouse_Buttons=buf[0];
    mouse_Xdiff=buf[1];
    mouse_Ydiff=buf[2];
    mouse_Zdiff=buf[3];
    printf("Standard Mouse: Buttons:%d X:%d Y:%d Z:%d \n",mouse_Buttons,mouse_Xdiff,mouse_Ydiff,mouse_Zdiff);          
  }
  if (len>4) { // 1, Buttons, X, Y, Z(Wheel) //NON STANDARD MOUSE
    if (buf[0]==1) { //GENIUS MOUSE... also CoolerMaster mouse
      if (len<8) {  // 1, Buttons, X, Y, Z(Wheel), 0, 0
        mouse_Buttons=buf[1];
        mouse_Xdiff=buf[2];
        mouse_Ydiff=buf[3];
        mouse_Zdiff=buf[4];
      } else { // 1, Buttons, Xlow, Xhigh, Ylow, Yhigh, Z(Wheel), 0
        mouse_Buttons=buf[1];
        mouse_Xdiff=(buf[2] | buf[3]<<8);
        mouse_Ydiff=(buf[4] | buf[5]<<8);
        mouse_Zdiff=buf[6];                
      }
      printf("Advanced Mouse: Buttons:%d X:%d Y:%d Z:%d \n",mouse_Buttons,mouse_Xdiff,mouse_Ydiff,mouse_Zdiff);          
    } else {
      printf("Unknown Mouse.\n");
    }
  }
//---------------------------------------------------------------------------------
  if (DEBUG>5) {
    for (uint8_t i = 0; i<len; i++) printf("%d ",buf[i]);
    printf("\n");
  }
//---------------------------------------------------------------------------------
  KM_task_mouse(mouse_Buttons,mouse_Xdiff,mouse_Ydiff,mouse_Zdiff);
}
//---------------------------------------------------------------------------------
void MouseRptParser::OnMouseMove(MOUSEINFO *mi) {
/*
  Serial.print("dx=");
  Serial.print(mi->dX, DEC);
  Serial.print(" dy=");
  Serial.println(mi->dY, DEC);
*/  
};
void MouseRptParser::OnLeftButtonUp  (MOUSEINFO *mi) {
  //Serial.println("L Butt Up");
};
void MouseRptParser::OnLeftButtonDown (MOUSEINFO *mi) {
  //Serial.println("L Butt Dn");
};
void MouseRptParser::OnRightButtonUp  (MOUSEINFO *mi) {
  //Serial.println("R Butt Up");
};
void MouseRptParser::OnRightButtonDown  (MOUSEINFO *mi) {
  //Serial.println("R Butt Dn");
};
void MouseRptParser::OnMiddleButtonUp (MOUSEINFO *mi) {
  //Serial.println("M Butt Up");
};
void MouseRptParser::OnMiddleButtonDown (MOUSEINFO *mi) {
  //Serial.println("M Butt Dn");
};

void MouseRptParser::OnWheelMove (MOUSEINFO *mi) {
  //Serial.print("dz=");
  //Serial.println(mi->dZ, DEC);
}
//---------------------------------------------------------------------------------
class KbdRptParser : public KeyboardReportParser
{
  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);
    void OnKeyDown  (uint8_t mod, uint8_t key);
    void OnKeyUp  (uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  USB_KEYBOARD_PRESS(key); 
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

  printf("OnControlKeysChanged: before: %d after: %d\n",before,after);
  
  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;


  if (afterMod.bmLeftCtrl) { //LCTRL
    USBKEYBOARD[0x5c]=1;
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,3); //PRESS CONTROL KEY     
  } else if (beforeMod.bmLeftCtrl) { 
    USBKEYBOARD[0x5c]=0;         
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,2); //RELEASE CONTROL KEY     
  }
  if (afterMod.bmLeftShift) { //LSHIFT
    USBKEYBOARD[0x4b]=1;
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,3); //PRESS CONTROL KEY     
  } else if (beforeMod.bmLeftShift) { 
    USBKEYBOARD[0x4b]=0;         
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,2); //RELEASE CONTROL KEY     
  }
  if (afterMod.bmLeftAlt) { //LALT
    USBKEYBOARD[0x5d]=1;
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,3); //PRESS CONTROL KEY     
  } else if (beforeMod.bmLeftAlt) { 
    USBKEYBOARD[0x5d]=0;         
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,2); //RELEASE CONTROL KEY     
  }
  if (afterMod.bmLeftGUI) { //LGUI
    USBKEYBOARD[0x66]=1;
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,3); //PRESS CONTROL KEY     
  } else if (beforeMod.bmLeftGUI) { 
    USBKEYBOARD[0x66]=0;         
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,2); //RELEASE CONTROL KEY     
  }
  if (afterMod.bmRightCtrl) { //RCTRL
    USBKEYBOARD[0x60]=1;
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,3); //PRESS CONTROL KEY     
  } else if (beforeMod.bmRightCtrl) { 
    USBKEYBOARD[0x60]=0;         
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,2); //RELEASE CONTROL KEY     
  }
  if (afterMod.bmRightShift) { //RSHIFT
    USBKEYBOARD[0x56]=1;
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,3); //PRESS CONTROL KEY     
  } else if (beforeMod.bmRightShift) { 
    USBKEYBOARD[0x56]=0;         
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,2); //RELEASE CONTROL KEY     
  }
  if (afterMod.bmRightAlt) { //RALT
    USBKEYBOARD[0x5F]=1;
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,3); //PRESS CONTROL KEY     
  } else if (beforeMod.bmRightAlt) { 
    USBKEYBOARD[0x5F]=0;         
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,2); //RELEASE CONTROL KEY     
  }
  if (afterMod.bmRightGUI) { //RGUI
    USBKEYBOARD[0x67]=1;
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,3); //PRESS CONTROL KEY     
  } else if (beforeMod.bmRightGUI) { 
    USBKEYBOARD[0x67]=0;         
    if (POWER==APP_KMSWITCH) KM_task_keyboard(after,2); //RELEASE CONTROL KEY     
  }
#if FAKE86_EMULATOR
  //if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD();
#endif
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key) {
  USB_KEYBOARD_RELEASE(key);
}

void KbdRptParser::OnKeyPressed(uint8_t key) {
  Serial.print("ASCII: ");
  Serial.println((char)key);
};
//********************************************************************************
#else                                   // USB_UNIVERSAL_HID enabled:
//********************************************************************************

//================================================================================
// MOUSE
//--------------------------------------------------------------------------------
extern int16_t MOUSEX;  
extern int16_t MOUSEY;
extern int16_t MOUSEZ;                         // SCROLL WHEEL
extern uint8_t MOUSEBUTTONS;
extern uint8_t MOUSEBUTTONS_LAST;
//================================================================================





//================================================================================
// USB JOYSTICK 1
//--------------------------------------------------------------------------------
extern uint8_t USB_JOY_UP;
extern uint8_t USB_JOY_DOWN;
extern uint8_t USB_JOY_LEFT;
extern uint8_t USB_JOY_RIGHT;

extern uint8_t USB_JOY_A;
extern uint8_t USB_JOY_B;
extern uint8_t USB_JOY_C;
extern uint8_t USB_JOY_X;
extern uint8_t USB_JOY_Y;
extern uint8_t USB_JOY_Z;
extern uint8_t USB_JOY_L;
extern uint8_t USB_JOY_R;
 
extern uint8_t USB_JOY_START;
extern uint8_t USB_JOY_SELECT;
extern uint8_t USB_JOY_SYSTEM; //0=HOME button OFF, 1=HOME button ON.
//================================================================================

//================================================================================
// USB JOYSTICK 2
//--------------------------------------------------------------------------------
extern uint8_t USB_JOY2_UP;
extern uint8_t USB_JOY2_DOWN;
extern uint8_t USB_JOY2_LEFT;
extern uint8_t USB_JOY2_RIGHT;

extern uint8_t USB_JOY2_A;
extern uint8_t USB_JOY2_B;
extern uint8_t USB_JOY2_C;
extern uint8_t USB_JOY2_X;
extern uint8_t USB_JOY2_Y;
extern uint8_t USB_JOY2_Z;
extern uint8_t USB_JOY2_L;
extern uint8_t USB_JOY2_R;
 
extern uint8_t USB_JOY2_START;
extern uint8_t USB_JOY2_SELECT;
extern uint8_t USB_JOY2_SYSTEM; //0=HOME button OFF, 1=HOME button ON.
//================================================================================












uint8_t PLAYER2_USB_dev_addr = 0; //device assigned as second player

uint8_t USB_KEYBOARD_PRESS(uint8_t KEY);
uint8_t USB_KEYBOARD_RELEASE(uint8_t KEY) ;



uint8_t active_address=0;
uint16_t active_VID=0;
uint16_t active_PID=0;

uint8_t USB_dev_addr1 = 0;
uint8_t USB_dev_addr2 = 0;
uint8_t USB_dev_addr3 = 0;
uint8_t USB_dev_addr4 = 0;

uint8_t USB_dev_VID1 = 0;
uint8_t USB_dev_VID2 = 0;
uint8_t USB_dev_VID3 = 0;
uint8_t USB_dev_VID4 = 0;
uint8_t USB_dev_PID1 = 0;
uint8_t USB_dev_PID2 = 0;
uint8_t USB_dev_PID3 = 0;
uint8_t USB_dev_PID4 = 0;


uint8_t USB_prev_buf[8]={0};

//--------------------------------------------------------------------------------
class UniHidReportParser : public HIDReportParser {
public:
        virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};
//--------------------------------------------------------------------------------
void UniHidReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  //if (DEBUG>5) {
    for (uint8_t i = 0; i < len; i++) printf("%02X ",buf[i]);
    printf("\n");                
  //}        
//................................................................................
  if (active_address==PLAYER2_USB_dev_addr) {
    if (active_VID==2079 && active_PID==58369) { //SNES USB Joystick
#if USBHOST_JOYSTICKS
      if (USBhost_joysticks==2) {

        if (buf[0]>0x7F) USB_JOY2_RIGHT=1;
        else if (buf[0]<0x7F) USB_JOY2_LEFT=1;
        else {
          USB_JOY2_RIGHT=0;
          USB_JOY2_LEFT=0;
        }
        if (buf[1]>0x7F) USB_JOY2_DOWN=1;
        else if (buf[1]<0x7F) USB_JOY2_UP=1;
        else {
          USB_JOY2_DOWN=0;
          USB_JOY2_UP=0;
        }

        if (buf[5]&0x10) {
          USB_JOY2_X=1;          
        } else {
          USB_JOY2_X=0;          
        }
        if (buf[5]&0x20) {
          USB_JOY2_A=1;          
        } else {
          USB_JOY2_A=0;          
        }
        if (buf[5]&0x40) {
          USB_JOY2_B=1;          
        } else {
          USB_JOY2_B=0;          
        }
        if (buf[5]&0x80) {
          USB_JOY2_Y=1;          
        } else {
          USB_JOY2_Y=0;          
        }

        if (buf[6]&0x01) {
          USB_JOY2_L=1;          
        } else {
          USB_JOY2_L=0;          
        }
        if (buf[6]&0x02) {
          USB_JOY2_R=1;          
        } else {
          USB_JOY2_R=0;          
        }
        if (buf[6]&0x10) {
          USB_JOY2_SELECT=1;          
        } else {
          USB_JOY2_SELECT=0;          
        }
        if (buf[6]&0x20) {
          USB_JOY2_START=1;          
        } else {
          USB_JOY2_START=0;          
        }
      }
#endif         
//................................................................................
    } else if (active_VID==1356 && active_PID==2508) { //PS4 USB Joystick
#if USBHOST_JOYSTICKS
      if (USBhost_joysticks==2) {
        if (buf[5]==0) {
          USB_JOY2_UP=1;          
          USB_JOY2_RIGHT=0;          
          USB_JOY2_DOWN=0;         
          USB_JOY2_LEFT=0; 
        } else if (buf[5]==1) {
          USB_JOY2_UP=1; 
          USB_JOY2_RIGHT=1;          
          USB_JOY2_DOWN=0;         
          USB_JOY2_LEFT=0; 
        } else if (buf[5]==2) {
          USB_JOY2_RIGHT=1;          
          USB_JOY2_DOWN=0;         
          USB_JOY2_UP=0;        
          USB_JOY2_LEFT=0; 
        } else if (buf[5]==3) {
          USB_JOY2_RIGHT=1; 
          USB_JOY2_DOWN=1;         
          USB_JOY2_UP=0;        
          USB_JOY2_LEFT=0; 
        } else if (buf[5]==4) {
          USB_JOY2_DOWN=1;          
          USB_JOY2_UP=0;        
          USB_JOY2_RIGHT=0; 
          USB_JOY2_LEFT=0; 
        } else if (buf[5]==5) {
          USB_JOY2_DOWN=1; 
          USB_JOY2_LEFT=1;         
          USB_JOY2_UP=0;        
          USB_JOY2_RIGHT=0; 
        } else if (buf[5]==6) {
          USB_JOY2_LEFT=1;          
          USB_JOY2_UP=0;        
          USB_JOY2_RIGHT=0; 
          USB_JOY2_DOWN=0;         
        } else if (buf[5]==7) {          
          USB_JOY2_LEFT=1;  
          USB_JOY2_UP=1;        
          USB_JOY2_RIGHT=0; 
          USB_JOY2_DOWN=0;         
        } else if (buf[5]==8) {          
          USB_JOY2_LEFT=0;  
          USB_JOY2_UP=0;        
          USB_JOY2_RIGHT=0; 
          USB_JOY2_DOWN=0;         
        } 
        
        if (buf[5]&0x10) { //SQUARE
          USB_JOY2_B=1;          
        } else {
          USB_JOY2_B=0;          
        }
        if (buf[5]&0x20) { //CROSS
          USB_JOY2_A=1;          
        } else {
          USB_JOY2_A=0;          
        }
        if (buf[5]&0x40) { //CIRCLE
          USB_JOY2_Y=1;          
        } else {
          USB_JOY2_Y=0;          
        }
        if (buf[5]&0x80) { //TRIANGLE
          USB_JOY2_X=1;          
        } else {
          USB_JOY2_X=0;          
        }
 
        if (buf[6]&0x01) { //L
          USB_JOY2_L=1;          
        } else {
          USB_JOY2_L=0;          
        }
        if (buf[6]&0x02) { //R
          USB_JOY2_R=1;          
        } else {
          USB_JOY2_R=0;          
        }
        
        if (buf[6]&0x10) { //SHARE
          USB_JOY2_START=1;          
        } else {
          USB_JOY2_START=0;          
        }
        if (buf[6]&0x20) { //OPTIONS
          USB_JOY2_SELECT=1;          
        } else {
          USB_JOY2_SELECT=0;          
        }       
      }
#endif 
    }  
//................................................................................
//................................................................................
//................................................................................
  } else if (active_address==USB_dev_addr1 || active_address==USB_dev_addr2 || active_address==USB_dev_addr3 || active_address==USB_dev_addr4) {
    if (active_VID==2079 && active_PID==58369) { //SNES USB Joystick
#if USBHOST_JOYSTICKS
      if (USBhost_joysticks==2) {
        if (buf[0]>0x7F) USB_JOY_RIGHT=1;
        else if (buf[0]<0x7F) USB_JOY_LEFT=1;
        else {
          USB_JOY_RIGHT=0;
          USB_JOY_LEFT=0;
        }
        if (buf[1]>0x7F) USB_JOY_DOWN=1;
        else if (buf[1]<0x7F) USB_JOY_UP=1;
        else {
          USB_JOY_DOWN=0;
          USB_JOY_UP=0;
        }

        if (buf[5]&0x10) {
          USB_JOY_X=1;          
        } else {
          USB_JOY_X=0;          
        }
        if (buf[5]&0x20) {
          USB_JOY_A=1;          
        } else {
          USB_JOY_A=0;          
        }
        if (buf[5]&0x40) {
          USB_JOY_B=1;          
        } else {
          USB_JOY_B=0;          
        }
        if (buf[5]&0x80) {
          USB_JOY_Y=1;          
        } else {
          USB_JOY_Y=0;          
        }

        if (buf[6]&0x01) {
          USB_JOY_L=1;          
        } else {
          USB_JOY_L=0;          
        }
        if (buf[6]&0x02) {
          USB_JOY_R=1;          
        } else {
          USB_JOY_R=0;          
        }
        if (buf[6]&0x10) {
          USB_JOY_SELECT=1;          
        } else {
          USB_JOY_SELECT=0;          
        }
        if (buf[6]&0x20) {
          USB_JOY_START=1;          
        } else {
          USB_JOY_START=0;          
        }
      }
#endif       
//................................................................................
    } else if (active_VID==1356 && active_PID==2508) { //PS4 USB Joystick
#if USBHOST_JOYSTICKS
      if (USBhost_joysticks==2) {
        if (buf[5]==0) {
          USB_JOY_UP=1;          
          USB_JOY_RIGHT=0;          
          USB_JOY_DOWN=0;         
          USB_JOY_LEFT=0; 
        } else if (buf[5]==1) {
          USB_JOY_UP=1; 
          USB_JOY_RIGHT=1;          
          USB_JOY_DOWN=0;         
          USB_JOY_LEFT=0; 
        } else if (buf[5]==2) {
          USB_JOY_RIGHT=1;          
          USB_JOY_DOWN=0;         
          USB_JOY_UP=0;        
          USB_JOY_LEFT=0; 
        } else if (buf[5]==3) {
          USB_JOY_RIGHT=1; 
          USB_JOY_DOWN=1;         
          USB_JOY_UP=0;        
          USB_JOY_LEFT=0; 
        } else if (buf[5]==4) {
          USB_JOY_DOWN=1;          
          USB_JOY_UP=0;        
          USB_JOY_RIGHT=0; 
          USB_JOY_LEFT=0; 
        } else if (buf[5]==5) {
          USB_JOY_DOWN=1; 
          USB_JOY_LEFT=1;         
          USB_JOY_UP=0;        
          USB_JOY_RIGHT=0; 
        } else if (buf[5]==6) {
          USB_JOY_LEFT=1;          
          USB_JOY_UP=0;        
          USB_JOY_RIGHT=0; 
          USB_JOY_DOWN=0;         
        } else if (buf[5]==7) {          
          USB_JOY_LEFT=1;  
          USB_JOY_UP=1;        
          USB_JOY_RIGHT=0; 
          USB_JOY_DOWN=0;         
        } else if (buf[5]==8) {          
          USB_JOY_LEFT=0;  
          USB_JOY_UP=0;        
          USB_JOY_RIGHT=0; 
          USB_JOY_DOWN=0;         
        }     

        if (buf[5]&0x10) { //SQUARE
          USB_JOY_B=1;          
        } else {
          USB_JOY_B=0;          
        }
        if (buf[5]&0x20) { //CROSS
          USB_JOY_A=1;          
        } else {
          USB_JOY_A=0;          
        }
        if (buf[5]&0x40) { //CIRCLE
          USB_JOY_Y=1;          
        } else {
          USB_JOY_Y=0;          
        }
        if (buf[5]&0x80) { //TRIANGLE
          USB_JOY_X=1;          
        } else {
          USB_JOY_X=0;          
        }
        if (buf[6]&0x01) { //L
          USB_JOY_L=1;          
        } else {
          USB_JOY_L=0;          
        }
        if (buf[6]&0x02) { //R
          USB_JOY_R=1;          
        } else {
          USB_JOY_R=0;          
        }

        if (buf[6]&0x10) { //SHARE
          USB_JOY_START=1;          
        } else {
          USB_JOY_START=0;          
        }
        if (buf[6]&0x20) { //OPTIONS
          USB_JOY_SELECT=1;          
        } else {
          USB_JOY_SELECT=0;          
        }
      }
#endif       
//................................................................................ 
    } else if (len==8) { //PROBABLY KEYBOARD
#if USBHOST_HID


//________________________________________________________________________________      
      if (USB_prev_buf[0]!=buf[0]) {
        if (buf[0]&0x01) { //LEFT CTRL
          USBKEYBOARD[0x5c]=1;
        } else {
          USBKEYBOARD[0x5c]=0;     
        } 
        if (buf[0]&0x02) { //LEFT SHIFT
          USBKEYBOARD[0x4b]=1;
        } else {
          USBKEYBOARD[0x4b]=0;     
        } 
        if (buf[0]&0x04) { //LEFT ALT
          USBKEYBOARD[0x5d]=1;
        } else {
          USBKEYBOARD[0x5d]=0;     
        } 
        if (buf[0]&0x08) { //LEFT GUI
          USBKEYBOARD[0x66]=1;
        } else {
          USBKEYBOARD[0x66]=0;     
        } 
        if (buf[0]&0x10) { //RIGHT CTRL
          USBKEYBOARD[0x60]=1;
        } else {
          USBKEYBOARD[0x60]=0;     
        } 
        if (buf[0]&0x10) { //RIGHT SHIFT
          USBKEYBOARD[0x56]=1;
        } else {
          USBKEYBOARD[0x56]=0;     
        } 
        if (buf[0]&0x10) { //RIGHT ALT
          USBKEYBOARD[0x5F]=1;
        } else {
          USBKEYBOARD[0x5F]=0;     
        } 
        if (buf[0]&0x10) { //RIGHT GUI
          USBKEYBOARD[0x67]=1;
        } else {
          USBKEYBOARD[0x67]=0;     
        } 

#if FAKE86_EMULATOR
        if (POWER==APP_EMU_FAKE86) FAKE86_KEYBOARD();
#endif
        USB_prev_buf[0]=buf[0];
      }     
//________________________________________________________________________________      
      if (USBhost_keyboard_and_mouse==2) {
        if (buf[0]==1 || buf[1]==1) return; //multiple keypress overflow     
        for (uint8_t tmp1=2;tmp1<len && tmp1<8; tmp1++) {
          uint8_t NOTFOUND=tmp1;
          for (uint8_t tmp2=2;tmp2<len && tmp2<8;tmp2++) {
            if (buf[tmp1]==USB_prev_buf[tmp2]) {
              NOTFOUND=0;
              break;
            }         
          }
          if (NOTFOUND) {
            USB_prev_buf[NOTFOUND]=buf[NOTFOUND];
            printf("USB PRESSED KEY: %d\n",buf[NOTFOUND]); 
            USB_KEYBOARD_PRESS(buf[NOTFOUND]);
          }
        }
        for (uint8_t tmp1=2;tmp1<len && tmp1<8; tmp1++) {  
          uint8_t NOTFOUND=tmp1;
          for (uint8_t tmp2=2;tmp2<len && tmp2<8;tmp2++) {
            if (USB_prev_buf[tmp1]==buf[tmp2]) {
              NOTFOUND=0;
              break;
            }         
          }
          if (NOTFOUND) {
            printf("USB RELEASED KEY: %d\n",USB_prev_buf[NOTFOUND]); 
            USB_KEYBOARD_RELEASE(USB_prev_buf[NOTFOUND]);
            USB_prev_buf[NOTFOUND]=0;
          }      
        }
      }
#endif    
//................................................................................
    } else if (len==4) { //PROBABLY MOUSE
#if USBHOST_HID
      if (USBhost_keyboard_and_mouse==2) {
        MOUSEBUTTONS=buf[0];
        MOUSEX = buf[1] * 256; 
        MOUSEY = buf[2] * 256; 
        MOUSEZ = buf[3];      
      }
    }
#endif
  }
//................................................................................  
};
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
class HIDUniversal2 : public HIDUniversal, public UniHidReportParser{
  public:
    HIDUniversal2(USB *usb) : HIDUniversal(usb) {};

    bool connected() {
      return HIDUniversal::isReady();
    };

    uint8_t get_VID() {
      return(HIDUniversal::VID);
    };

    uint8_t get_PID() {
      return(HIDUniversal::PID);
    }

    virtual void ParseHIDData(USBHID *hid __attribute__((unused)), bool is_rpt_id __attribute__((unused)), uint8_t len, uint8_t *buf) {
//................................................................................            
      active_address=hid->GetAddress();
//................................................................................            
      if (USB_dev_addr1!=active_address && USB_dev_addr2!=active_address && USB_dev_addr3!=active_address && USB_dev_addr4!=active_address) {
        if (USB_dev_addr1==0) {
          printf("USB setting address 1: %d \n",active_address);
          USB_dev_addr1=active_address;
          USB_dev_VID1 = HIDUniversal::VID;
          USB_dev_PID1 = HIDUniversal::PID;
        } else if (USB_dev_addr2==0) {
          printf("USB setting address 2: %d \n",active_address);
          USB_dev_addr2=active_address;
          USB_dev_VID2 = HIDUniversal::VID;
          USB_dev_PID2 = HIDUniversal::PID;
          if (Multiplayer_dev_input_support==2) {
            if (USB_dev_VID1==USB_dev_VID2 && USB_dev_PID1==USB_dev_PID2 && PLAYER2_USB_dev_addr==0) { //same kind of device connected
              printf("PLAYER TWO: USB setting address: %d \n",active_address);
              PLAYER2_USB_dev_addr=active_address;
            }
          }
        } else if (USB_dev_addr3==0) {
          printf("USB setting address 3: %d \n",active_address);
          USB_dev_addr3=active_address;
          USB_dev_VID3 = HIDUniversal::VID;
          USB_dev_PID3 = HIDUniversal::PID;
          if (Multiplayer_dev_input_support==2) {
            if (((USB_dev_VID1==USB_dev_VID2 && USB_dev_PID1==USB_dev_PID2) || (USB_dev_VID1==USB_dev_VID3 && USB_dev_PID1==USB_dev_PID3) || (USB_dev_VID2==USB_dev_VID3 && USB_dev_PID2==USB_dev_PID3)) && PLAYER2_USB_dev_addr==0) { //same kind of device connected
              printf("PLAYER TWO: USB setting address: %d \n",active_address);
              PLAYER2_USB_dev_addr=active_address;
            }
          }
        } else if (USB_dev_addr4==0) {
          printf("USB setting address 4: %d \n",active_address);
          USB_dev_addr4=active_address;
          USB_dev_VID4 = HIDUniversal::VID;
          USB_dev_PID4 = HIDUniversal::PID;
          if (Multiplayer_dev_input_support==2) {
            if (((USB_dev_VID1==USB_dev_VID2 && USB_dev_PID1==USB_dev_PID2) || (USB_dev_VID1==USB_dev_VID3 && USB_dev_PID1==USB_dev_PID3) || 
              (USB_dev_VID1==USB_dev_VID4 && USB_dev_PID1==USB_dev_PID4) || (USB_dev_VID2==USB_dev_VID3 && USB_dev_PID2==USB_dev_PID3) || 
              (USB_dev_VID2==USB_dev_VID4 && USB_dev_PID2==USB_dev_PID4) || (USB_dev_VID3==USB_dev_VID4 && USB_dev_PID3==USB_dev_PID4)) && PLAYER2_USB_dev_addr==0) { //same kind of device connected
              printf("PLAYER TWO: USB setting address: %d \n",active_address);
              PLAYER2_USB_dev_addr=active_address;
            }
          }
        } else {
          printf("USB reached maximum HID devices.");
        }
      } else {
      }
//................................................................................
      active_VID=HIDUniversal::VID;
      active_PID=HIDUniversal::PID;
//................................................................................
      if (DEBUG>4) {
        printf("USB ADDRESS: %d ",hid->GetAddress());                     
        printf("VID: %d ",HIDUniversal::VID);
        printf("PID: %d ",HIDUniversal::PID);               
        printf("\n");
      }            
      UniHidReportParser::Parse(hid, is_rpt_id, len, buf);
    };
        
  protected:
    uint8_t OnInitSuccessful();
};
//--------------------------------------------------------------------------------
uint8_t HIDUniversal2::OnInitSuccessful() {
  return 0;
}
//--------------------------------------------------------------------------------










//********************************************************************************
#endif
//********************************************************************************
#endif
