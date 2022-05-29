#if USBHOST

//********************************************************************************
#if !USB_UNIVERSAL_HID                  // USB_UNIVERSAL_HID disabled:
//********************************************************************************
#if USBHOST_JOYSTICKS

//********************************************************************************
// USB JOYSTICK (SNES USB JOYSTICK TEST)
//********************************************************************************

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
extern uint8_t USB_JOY_SYSTEM; 
//================================================================================
struct GamePadEventData {
  uint8_t X, Y, Z1, Z2, Rz;
};

class JoystickEvents {
public:
  virtual void OnGamePadChanged(const GamePadEventData *evt);
  virtual void OnHatSwitch(uint8_t hat);
  virtual void OnButtonUp(uint8_t but_id);
  virtual void OnButtonDn(uint8_t but_id);
};

#define RPT_GEMEPAD_LEN    5

class JoystickReportParser : public HIDReportParser {
  JoystickEvents *joyEvents;
  uint8_t oldPad[RPT_GEMEPAD_LEN];
  uint8_t oldHat;
  uint16_t oldButtons;
public:
  JoystickReportParser(JoystickEvents *evt);
  virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};
//--------------------------------------------------------------------------------
JoystickReportParser::JoystickReportParser(JoystickEvents *evt) :
  joyEvents(evt),
  oldHat(0xDE),
  oldButtons(0) {
  for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = 0xD;
}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  bool match = true;

  // Checking if there are changes in report since the method was last called
  for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) if (buf[i] != oldPad[i]) {
    match = false;
    break;
  }
  // Calling Game Pad event handler
  if (!match && joyEvents) {
    joyEvents->OnGamePadChanged((const GamePadEventData*)buf);
    for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = buf[i];
  }
  uint8_t hat = (buf[5] & 0xF);
  // Calling Hat Switch event handler
  if (hat != oldHat && joyEvents) {
    joyEvents->OnHatSwitch(hat);
    oldHat = hat;
  }
  uint16_t buttons = (0x0000 | buf[6]);
  buttons <<= 4;
  buttons |= (buf[5] >> 4);
  uint16_t changes = (buttons ^ oldButtons);
  // Calling Button Event Handler for every button changed
  if (changes) {
    for (uint8_t i = 0; i < 0x0C; i++) {
      uint16_t mask = (0x0001 << i);
      if (((mask & changes) > 0) && joyEvents) {
        if ((buttons & mask) > 0)
          joyEvents->OnButtonDn(i + 1);
        else
          joyEvents->OnButtonUp(i + 1);
      }
    }
    oldButtons = buttons;
  }
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt) {
  Serial.print("X1: ");
  PrintHex<uint8_t > (evt->X, 0x80);
  Serial.print("\tY1: ");
  PrintHex<uint8_t > (evt->Y, 0x80);
  Serial.print("\tX2: ");
  PrintHex<uint8_t > (evt->Z1, 0x80);
  Serial.print("\tY2: ");
  PrintHex<uint8_t > (evt->Z2, 0x80);
  Serial.print("\tRz: ");
  PrintHex<uint8_t > (evt->Rz, 0x80);
  Serial.println("");
//................................................................................
  if (evt->X>0x7F) USB_JOY_RIGHT=1;
  else if (evt->X<0x7F) USB_JOY_LEFT=1;
  else {
    USB_JOY_RIGHT=0;
    USB_JOY_LEFT=0;
  }
  if (evt->Y>0x7F) USB_JOY_DOWN=1;
  else if (evt->Y<0x7F) USB_JOY_UP=1;
  else {
    USB_JOY_DOWN=0;
    USB_JOY_UP=0;
  }
//................................................................................
}

void JoystickEvents::OnHatSwitch(uint8_t hat) {
  Serial.print("Hat Switch: ");
  PrintHex<uint8_t > (hat, 0x80);
  Serial.println("");
}

void JoystickEvents::OnButtonUp(uint8_t but_id) {
  Serial.print("Up: ");
  Serial.println(but_id, DEC);
//................................................................................
  if (but_id==2) USB_JOY_A=0;
  if (but_id==3) USB_JOY_B=0;
  if (but_id==1) USB_JOY_X=0;
  if (but_id==4) USB_JOY_Y=0;
  if (but_id==5) USB_JOY_L=0;
  if (but_id==6) USB_JOY_R=0;
  if (but_id==10) USB_JOY_START=0;
  if (but_id==9) USB_JOY_SELECT=0;       
//................................................................................
}

void JoystickEvents::OnButtonDn(uint8_t but_id) {
  Serial.print("Dn: ");
  Serial.println(but_id, DEC);
//................................................................................
  if (but_id==2) USB_JOY_A=1;
  if (but_id==3) USB_JOY_B=1;
  if (but_id==1) USB_JOY_X=1;
  if (but_id==4) USB_JOY_Y=1;
  if (but_id==5) USB_JOY_L=1;
  if (but_id==6) USB_JOY_R=1;
  if (but_id==10) USB_JOY_START=1;
  if (but_id==9) USB_JOY_SELECT=1;       
//................................................................................
}

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
//--------------------------------------------------------------------------------

struct GamePadEventData2 {
  uint8_t X, Y, Z1, Z2, Rz;
};

class JoystickEvents2 {
public:
  virtual void OnGamePadChanged(const GamePadEventData2 *evt);
  virtual void OnHatSwitch(uint8_t hat);
  virtual void OnButtonUp(uint8_t but_id);
  virtual void OnButtonDn(uint8_t but_id);
};

#define RPT_GEMEPAD_LEN    5

class JoystickReportParser2 : public HIDReportParser {
  JoystickEvents2 *joyEvents;
  uint8_t oldPad[RPT_GEMEPAD_LEN];
  uint8_t oldHat;
  uint16_t oldButtons;
public:
  JoystickReportParser2(JoystickEvents2 *evt);
  virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};
//--------------------------------------------------------------------------------

JoystickReportParser2::JoystickReportParser2(JoystickEvents2 *evt) :
  joyEvents(evt),
  oldHat(0xDE),
  oldButtons(0) {
  for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = 0xD;
}

void JoystickReportParser2::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  bool match = true;

  // Checking if there are changes in report since the method was last called
  for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
    if (buf[i] != oldPad[i]) {
      match = false;
      break;
    }

  // Calling Game Pad event handler
  if (!match && joyEvents) {
    joyEvents->OnGamePadChanged((const GamePadEventData2*)buf);
    for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = buf[i];
  }

  uint8_t hat = (buf[5] & 0xF);

  // Calling Hat Switch event handler
  if (hat != oldHat && joyEvents) {
    joyEvents->OnHatSwitch(hat);
    oldHat = hat;
  }

  uint16_t buttons = (0x0000 | buf[6]);
  buttons <<= 4;
  buttons |= (buf[5] >> 4);
  uint16_t changes = (buttons ^ oldButtons);

  // Calling Button Event Handler for every button changed
  if (changes) {
    for (uint8_t i = 0; i < 0x0C; i++) {
      uint16_t mask = (0x0001 << i);

      if (((mask & changes) > 0) && joyEvents) {
        if ((buttons & mask) > 0)
          joyEvents->OnButtonDn(i + 1);
        else
          joyEvents->OnButtonUp(i + 1);
      }
    }
    oldButtons = buttons;
  }
}

void JoystickEvents2::OnGamePadChanged(const GamePadEventData2 *evt) {
  Serial.print("X1: ");
  PrintHex<uint8_t > (evt->X, 0x80);
  Serial.print("\tY1: ");
  PrintHex<uint8_t > (evt->Y, 0x80);
  Serial.print("\tX2: ");
  PrintHex<uint8_t > (evt->Z1, 0x80);
  Serial.print("\tY2: ");
  PrintHex<uint8_t > (evt->Z2, 0x80);
  Serial.print("\tRz: ");
  PrintHex<uint8_t > (evt->Rz, 0x80);
  Serial.println("");
//................................................................................
  if (evt->X>0x7F) USB_JOY2_RIGHT=1;
  else if (evt->X<0x7F) USB_JOY2_LEFT=1;
  else {
    USB_JOY2_RIGHT=0;
    USB_JOY2_LEFT=0;
  }
  if (evt->Y>0x7F) USB_JOY2_DOWN=1;
  else if (evt->Y<0x7F) USB_JOY2_UP=1;
  else {
    USB_JOY2_DOWN=0;
    USB_JOY2_UP=0;
  }
//................................................................................
}

void JoystickEvents2::OnHatSwitch(uint8_t hat) {
  Serial.print("Hat Switch: ");
  PrintHex<uint8_t > (hat, 0x80);
  Serial.println("");
}

void JoystickEvents2::OnButtonUp(uint8_t but_id) {
  Serial.print("Up: ");
  Serial.println(but_id, DEC);
//................................................................................
  if (but_id==2) USB_JOY2_A=0;
  if (but_id==3) USB_JOY2_B=0;
  if (but_id==1) USB_JOY2_X=0;
  if (but_id==4) USB_JOY2_Y=0;
  if (but_id==5) USB_JOY2_L=0;
  if (but_id==6) USB_JOY2_R=0;
  if (but_id==10) USB_JOY2_START=0;
  if (but_id==9) USB_JOY2_SELECT=0;       
//................................................................................
}

void JoystickEvents2::OnButtonDn(uint8_t but_id) {
  Serial.print("Dn: ");
  Serial.println(but_id, DEC);
//................................................................................
  if (but_id==2) USB_JOY2_A=1;
  if (but_id==3) USB_JOY2_B=1;
  if (but_id==1) USB_JOY2_X=1;
  if (but_id==4) USB_JOY2_Y=1;
  if (but_id==5) USB_JOY2_L=1;
  if (but_id==6) USB_JOY2_R=1;
  if (but_id==10) USB_JOY2_START=1;
  if (but_id==9) USB_JOY2_SELECT=1;       
//................................................................................
}


#endif
//********************************************************************************
#endif
//********************************************************************************
#endif
















