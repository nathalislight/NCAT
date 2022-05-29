#include "PS2Keyboard.h"

// Pins for PS/2 Interface (some USB keyboards WORK)
static const int DATA_PIN = 33;  // USB D-;
static const int CLOCK_PIN = 34; // USB D+;

PS2Keyboard kbd(DATA_PIN, CLOCK_PIN);

void setup() {
    Serial.begin(115200);
    delay(5000);  // Wait a long time for esp32 to have Serial print statements work right.
    kbd.begin();
}

void loop() {
   while(kbd.available()){
    uint16_t scancode=0;
    uint8_t scancode_HIGH = kbd.read();
    delay(5);
    if (kbd.available()) {
       uint8_t scancode_LOW = kbd.read();
       scancode=scancode_HIGH<<8 | scancode_LOW;
    } else scancode=scancode_HIGH;
    
    Serial.println(scancode,HEX);
  }
}
