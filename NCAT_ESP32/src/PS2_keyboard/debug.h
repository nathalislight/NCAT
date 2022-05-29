#ifndef DEBUG_H
#define DEBUG_H
#include <stdarg.h>
#include "PrintHex8.h"


#define DEBUG_ENABLE                   false  // If set to 'true' enables debug output


#if DEBUG_ENABLE

    const int DEBUG_MAX_LEN = 128;

    inline void dprintf(const char *fmt, ...) {
        
        char buf[DEBUG_MAX_LEN]; // resulting string limited to 128 chars
        
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, DEBUG_MAX_LEN, fmt, args);
        va_end (args);
        Serial.print(buf);
    }

    inline void dprint(const char *s) {
        Serial.print(s);
    }

    #define dmsg(s)             dprintf("%s at %s: %S\n", __FILE__, __LINE__, s)

    inline void dprint8(const char *msg, uint8_t c) {

      char buf[128];
      char *ptr = buf;

      strcpy(ptr, msg);
      ptr += strlen(ptr);
      strcpy(ptr, ": ");
      ptr += strlen(ptr);
      
      ptr = CharPrintHex8(ptr , c);

      strcpy(ptr, "\n");
      ptr += strlen(ptr);
      
      Serial.print(buf);
    }

#else

    inline void dprintf(const char *fmt, ...) {}
    inline void dprint(const char *s) {}

    #define dmsg(s)

    inline void dprint8(const char *msg, uint8_t c) {}

#endif


#endif      // DEBUG_H
