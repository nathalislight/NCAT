/*
 * MIT License
 *
 * Copyright (c) 2020 Erriez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*!
 * \file ErriezDS1307.h
 * \brief DS1307 RTC library for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS1307
 *      Documentation:  https://erriez.github.io/ErriezDS1307
 */

#ifndef ERRIEZ_DS1307_H_
#define ERRIEZ_DS1307_H_

#include <stdint.h>
#include <time.h>

//! DS1307 registers
#define DS1307_REG_SECONDS      0x00    //!< Seconds register
#define DS1307_REG_MINUTES      0x01    //!< Minutes register
#define DS1307_REG_HOURS        0x02    //!< Hours register
#define DS1307_REG_DAY_WEEK     0x03    //!< Day of the week register
#define DS1307_REG_DAY_MONTH    0x04    //!< Day of the month register
#define DS1307_REG_MONTH        0x05    //!< Month register
#define DS1307_REG_YEAR         0x06    //!< Year register
#define DS1307_REG_CONTROL      0x07    //!< Control register

//! DS1307 number of registers
#define DS1307_NUM_REGS         8       //!< 8 RTC register: 0x00..0x07

//! DS1307 register bit defines
#define DS1307_SEC_CH           7       //!< Clock halt
#define DS1307_HOUR_12H_24H     6       //!< 12 or 24 hour mode
#define DS1307_HOUR_AM_PM       5       //!< AM/PM

#define DS1307_CTRL_OUT         7       //!< Enable oscillator
#define DS1307_CTRL_SQWE        4       //!< Square-Wave Enable
#define DS1307_CTRL_RS1         1       //!< Rate Select 1
#define DS1307_CTRL_RS0         0       //!< Rate Select 0

//! DS1307 I2C 7-bit address
#define DS1307_ADDR             (0xD0 >> 1)

/*!
 * \brief Squarewave enum
 */
typedef enum {
    SquareWaveDisable = ((0 << DS1307_CTRL_SQWE) | (0 << DS1307_CTRL_RS1) | (0 << DS1307_CTRL_RS0)),  //!< SQW disable
    SquareWave1Hz = ((1 << DS1307_CTRL_SQWE) | (0 << DS1307_CTRL_RS1) | (0 << DS1307_CTRL_RS0)),      //!< SQW 1Hz
    SquareWave4096Hz = ((1 << DS1307_CTRL_SQWE) | (0 << DS1307_CTRL_RS1) | (1 << DS1307_CTRL_RS0)),   //!< SQW 4096Hz
    SquareWave8192Hz = ((1 << DS1307_CTRL_SQWE) | (1 << DS1307_CTRL_RS1) | (0 << DS1307_CTRL_RS0)),   //!< SQW 8192Hz
    SquareWave32768Hz = ((1 << DS1307_CTRL_SQWE) | (1 << DS1307_CTRL_RS1) | (1 << DS1307_CTRL_RS0)),  //!< SQW 32768Hz
} SquareWave;


/*!
 * \brief DS1307 RTC class
 */
class ErriezDS1307
{
public:
    // Initialize
    bool begin();

    // Oscillator functions
    bool isRunning();
    bool clockEnable(bool enable=true);

    // Set/get date/time
    time_t getEpoch();
    bool setEpoch(time_t t);
    bool read(struct tm *dt);
    bool write(const struct tm *dt);
    bool setTime(uint8_t hour, uint8_t min, uint8_t sec);
    bool getTime(uint8_t *hour, uint8_t *min, uint8_t *sec);
    bool setDateTime(uint8_t hour, uint8_t min, uint8_t sec,
                     uint8_t mday, uint8_t mon, uint16_t year,
                     uint8_t wday);
    bool getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec,
                     uint8_t *mday, uint8_t *mon, uint16_t *year,
                     uint8_t *wday);

    // Output signal control
    bool setSquareWave(SquareWave squareWave);

    // BCD conversions
    uint8_t bcdToDec(uint8_t bcd);
    uint8_t decToBcd(uint8_t dec);

    // Read/write register
    uint8_t readRegister(uint8_t reg);
    bool writeRegister(uint8_t reg, uint8_t value);

    // Read/write buffer
    bool readBuffer(uint8_t reg, void *buffer, uint8_t len);
    bool writeBuffer(uint8_t reg, void *buffer, uint8_t len);
};

#endif // ERRIEZ_DS1307_H_
