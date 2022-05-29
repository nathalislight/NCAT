#ifndef __config_h_
#define __config_h_

#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
 // on the the D21 and ESP32 the C-Runtime uses already UNIX time
 #define CONFIG_UNIXTIME
#else
// comment this out if you need unixtime support
// this will add about 324bytes to your firmware
//#define CONFIG_UNIXTIME
#endif

#endif
