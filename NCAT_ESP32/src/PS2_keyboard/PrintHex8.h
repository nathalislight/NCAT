#ifndef PRINTHEX8_H
#define PRINTHEX8_H

    // WARNING!  This is a dangerous function because it assumes the destination has 3 bytes of space available.
    // No other checking is performed.  The caller must ensure that the space is there.
    inline char * CharPrintHex8(char *dst, uint8_t data)
    {
        byte first = (data >> 4) & 0x0f;
        byte second = data & 0x0f;
    
        // base for converting single digit numbers to ASCII is 48
        // base for 10-16 to become lower-case characters a-f is 87
        // note: difference is 39
        dst[0] = first+48;
        dst[1] = second+48;
        if (first > 9) dst[0] += 39;
        if (second > 9) dst[1] += 39;
    
         dst[2] = 0;
    //     Serial.print(tmp);
         return dst + 2;
    }

#endif      // PRINTHEX8_H
