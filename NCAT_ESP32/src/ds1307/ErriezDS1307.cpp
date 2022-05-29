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
 * \file ErriezDS1307.cpp
 * \brief DS1307 RTC library for Arduino
 * \details
 *      Source:         https://github.com/Erriez/ErriezDS1307
 *      Documentation:  https://erriez.github.io/ErriezDS1307
 */

#if defined(__AVR__) || defined(ARDUINO_ARCH_SAM)
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include <Wire.h>

#include "ErriezDS1307.h"

/*!
 * \brief Initialize and detect DS1307 RTC.
 * \details
 *      Call this function from setup().
 * \retval true
 *      RTC detected.
 * \retval false
 *      RTC not detected.
 */
bool ErriezDS1307::begin()
{
    uint8_t regDayWeek;

    // Read seconds register
    if (!readBuffer(DS1307_REG_DAY_WEEK, &regDayWeek, 1)) {
        return false;
    }

    // Check zero bits in day week register
    if (regDayWeek & 0xF8) {
        return false;
    }

    // DS1307 detected
    return true;
}

/*!
 * \brief Read RTC CH (Clock Halt) from seconds register.
 * \details
 *      The application is responsible for checking the CH (Clock Halt) bit before reading
 *      date/time date. This function may be used to judge the validity of the date/time registers.
 * \retval true
 *      RTC clock is running.
 * \retval false
 *      The date/time data is invalid when the CH bit is set. The application should enable the
 *      oscillator, or program a new date/time.
 */
bool ErriezDS1307::isRunning()
{
    // Return status CH (Clock Halt) bit from seconds register
    if (readRegister(DS1307_REG_SECONDS) & (1 << DS1307_SEC_CH)) {
        // RTC clock stopped
        return false;
    } else {
        // RTC clock is running
        return true;
    }
}

/*!
 * \brief Enable or disable oscillator.
 * \details
 *      Clear or set CH (Clock Halt) bit to seconds register
 * \param enable
 *      true:  Enable RTC clock.\n
 *      false: Stop RTC clock.
 * \retval true
 *      Success.
 * \retval false
 *      Oscillator enable failed.
 */
bool ErriezDS1307::clockEnable(bool enable)
{
    uint8_t regSeconds;

    // Read seconds register
    regSeconds = readRegister(DS1307_REG_SECONDS);

    // Set or clear HT bit
    if (enable) {
        // Clear to enable
        regSeconds &= ~(1 << DS1307_SEC_CH);
    } else {
        // Set to disable
        regSeconds |= (1 << DS1307_SEC_CH);
    }

    // Write seconds register
    return writeRegister(DS1307_REG_SECONDS, regSeconds);
}

/*!
 * \brief Read Unix UTC epoch time_t
 * \return
 *      Unix epoch time_t seconds since 1970.
 */
time_t ErriezDS1307::getEpoch()
{
    struct tm dt;
    time_t t;

    // Read time structure
    if (!read(&dt)) {
        // RTC read failed
        return 0;
    }

    // Convert date/time struct tm to time_t
    t = mktime(&dt);

    // An offset is needed for AVR target
#ifdef ARDUINO_ARCH_AVR
    t += UNIX_OFFSET;
#endif

    // Return Unix epoch UTC
    return t;
}

/*!
 * \brief Write Unix epoch UTC time to RTC
 * \param t
 *      time_t time
 * \retval true
 *      Success.
 * \retval false
 *      Set epoch failed.
 */
bool ErriezDS1307::setEpoch(time_t t)
{
    struct tm *dt;

    // Subtract UNIX offset for AVR targets
#ifdef ARDUINO_ARCH_AVR
    t -= UNIX_OFFSET;
#endif

    // Convert time_t to date/time struct tm
    dt = gmtime(&t);

    // Write date/time to RTC
    return write(dt);
}

/*!
 * \brief Read date and time from RTC.
 * \details
 *      Read all RTC registers at once to prevent a time/date register change in the middle of the
 *      register read operation.
 * \param dt
 *      Date and time struct tm.
 * \retval true
 *      Success
 * \retval false
 *      Read failed.
 */
bool ErriezDS1307::read(struct tm *dt)
{
    uint8_t buffer[7];

    // Read clock date and time registers
    if (!readBuffer(0x00, buffer, sizeof(buffer))) {
        memset(dt, 0, sizeof(struct tm));
        return false;
    }

    // Clear dt
    memset(dt, 0, sizeof(struct tm));

    // Convert BCD buffer to Decimal
    dt->tm_sec = bcdToDec(buffer[0] & 0x7F);
    dt->tm_min = bcdToDec(buffer[1] & 0x7F);
    dt->tm_hour = bcdToDec(buffer[2] & 0x3F);
    dt->tm_wday = bcdToDec(buffer[3] & 0x07);
    dt->tm_mday = bcdToDec(buffer[4] & 0x3F);
    dt->tm_mon = bcdToDec(buffer[5] & 0x1F);
    dt->tm_year = bcdToDec(buffer[6]) + 100; // 2000-1900

    // Month: 0..11
    if (dt->tm_mon) {
        dt->tm_mon--;
    }

    // Day of the week: 0=Sunday
    if (dt->tm_wday) {
        dt->tm_wday--;
    }

    // Check buffer for valid data
    if ((dt->tm_sec > 59) || (dt->tm_min > 59) || (dt->tm_hour > 23) ||
        (dt->tm_mday < 1) || (dt->tm_mday > 31) || (dt->tm_mon > 11) || (dt->tm_year > 199) ||
        (dt->tm_wday > 6))
    {
        return false;
    }

    return true;
}

/*!
 * \brief Write date and time to RTC.
 * \details
 *      Write all RTC registers at once to prevent a time/date register change in the middle of the
 *      register write operation. This function enables the oscillator.
 * \param dt
 *      Date/time struct tm. Providing invalid date/time data may result in unpredictable behavior.
 * \retval true
 *      Success.
 * \retval false
 *      Write failed.
 */
bool ErriezDS1307::write(const struct tm *dt)
{
    uint8_t buffer[7];

    // Encode date time from decimal to BCD
    buffer[0] = decToBcd(dt->tm_sec) & 0x7F; // Clear CH bit in seconds register
    buffer[1] = decToBcd(dt->tm_min) & 0x7F;
    buffer[2] = decToBcd(dt->tm_hour) & 0x3F;
    buffer[3] = decToBcd(dt->tm_wday + 1) & 0x07;
    buffer[4] = decToBcd(dt->tm_mday) & 0x3F;
    buffer[5] = decToBcd(dt->tm_mon + 1) & 0x1F;
    buffer[6] = decToBcd(dt->tm_year % 100);

    // Write BCD encoded buffer to RTC registers
    return writeBuffer(0x00, buffer, sizeof(buffer));
}

/*!
 * \brief Write time to RTC.
 * \details
 *      Write hour, minute and second registers to RTC.
 * \param hour
 *      Hours 0..23.
 * \param min
 *      Minutes 0..59.
 * \param sec
 *      Seconds 0..59.
 * \retval true
 *      Success.
 * \retval false
 *      Set time failed.
 */
bool ErriezDS1307::setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
    struct tm dt;

    read(&dt);
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    return write(&dt);
}

/*!
 * \brief Read time from RTC.
 * \details
 *      Read hour, minute and second registers from RTC.
 * \param hour
 *      Hours 0..23.
 * \param min
 *      Minutes 0..59.
 * \param sec
 *      Seconds 0..59.
 * \retval true
 *      Success.
 * \retval false
 *      Invalid second, minute or hour read from RTC. The time is set to zero.
 */
bool ErriezDS1307::getTime(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t buffer[3];

    // Read RTC time registers
    if (!readBuffer(0x00, buffer, sizeof(buffer))) {
        return false;
    }

    // Convert BCD buffer to Decimal
    *sec = bcdToDec(buffer[0] & 0x7F); // Without CH bit from seconds register
    *min = bcdToDec(buffer[1] & 0x7F);
    *hour = bcdToDec(buffer[2] & 0x3F);

    // Check buffer for valid data
    if ((*sec > 59) || (*min > 59) || (*hour > 23)) {
        // Invalid time
        *sec = 0x00;
        *min = 0x00;
        *hour = 0x00;

        return false;
    }

    return true;
}

/*!
 * \brief Set date time
 * \param hour
 *      Hours 0..23
 * \param min
 *      Minutes 0..59
 * \param sec
 *      Seconds 0..59
 * \param mday
 *      Day of the month 1..31
 * \param mon
 *      Month 1..12 (1=January)
 * \param year
 *      Year 2000..2099
 * \param wday
 *      Day of the week 0..6 (0=Sunday, .. 6=Saturday)
 * \retval true
 *      Success.
 * \retval false
 *      Set date/time failed.
 */
bool ErriezDS1307::setDateTime(uint8_t hour, uint8_t min, uint8_t sec,
                               uint8_t mday, uint8_t mon, uint16_t year,
                               uint8_t wday)
{
    struct tm dt;

    // Prepare struct tm
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    dt.tm_mday = mday;
    dt.tm_mon = mon - 1;
    dt.tm_year = year - 1900;
    dt.tm_wday = wday;

    // Write date/time to RTC
    return write(&dt);
}

/*!
 * \brief Get date time
 * \param hour
 *      Hours 0..23
 * \param min
 *      Minutes 0..59
 * \param sec
 *      Seconds 0..59
 * \param mday
 *      Day of the month 1..31
 * \param mon
 *      Month 1..12 (1=January)
 * \param year
 *      Year 2000..2099
 * \param wday
 *      Day of the week 0..6 (0=Sunday, .. 6=Saturday)
 * \retval true
 *      Success.
 * \retval false
 *      Get date/time failed.
 */
bool ErriezDS1307::getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec,
                               uint8_t *mday, uint8_t *mon, uint16_t *year,
                               uint8_t *wday)
{
    struct tm dt;

    // Read date/time from RTC
    if (!read(&dt)) {
        return false;
    }

    // Set return values
    *hour = dt.tm_hour;
    *min = dt.tm_min;
    *sec = dt.tm_sec;
    *mday = dt.tm_mday;
    *mon = dt.tm_mon + 1;
    *year = dt.tm_year + 1900;
    *wday = dt.tm_wday;

    return true;
}

/*!
 * \brief Configure SQW (Square Wave) output pin.
 * \details
 *      This will disable or initialize the SQW clock pin.
 * \param squareWave
 *      SquareWave configuration:\n
 *          Disable:    SquareWaveDisable\n
 *          1Hz:        SquareWave1Hz\n
 *          4096Hz:     SquareWave4096Hz\n
 *          8192Hz:     SquareWave8192Hz\n
 *          32748Hz:    SquareWave32768Hz
 * \retval true
 *      Success
 * \retval false
 *      Set squareWave failed.
 */
bool ErriezDS1307::setSquareWave(SquareWave squareWave)
{
    // Write control register
    return writeBuffer(DS1307_REG_CONTROL, &squareWave, sizeof(squareWave));
}

/*!
 * \brief BCD to decimal conversion.
 * \param bcd
 *      BCD encoded value.
 * \return
 *      Decimal value.
 */
uint8_t ErriezDS1307::bcdToDec(uint8_t bcd)
{
    return (uint8_t)(10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

/*!
 * \brief Decimal to BCD conversion.
 * \param dec
 *      Decimal value.
 * \return
 *      BCD encoded value.
 */
uint8_t ErriezDS1307::decToBcd(uint8_t dec)
{
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
}

/*!
 * \brief Read register.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \returns value
 *      8-bit unsigned register value.
 */
uint8_t ErriezDS1307::readRegister(uint8_t reg)
{
    uint8_t value = 0;

    // Read buffer with one 8-bit unsigned value
    readBuffer(reg, &value, 1);

    return value;
}

/*!
 * \brief Write register.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x12.
 * \param value
 *      8-bit unsigned register value.
 * \retval true
 *      Success
 * \retval false
 *      Write register failed
 */
bool ErriezDS1307::writeRegister(uint8_t reg, uint8_t value)
{
    // Write buffer with one 8-bit unsigned value
    return writeBuffer(reg, &value, 1);
}

/*!
 * \brief Write buffer to RTC.
 * \details
 *      Please refer to the RTC datasheet.
 * \param reg
 *      RTC register number 0x00..0x07.
 * \param buffer
 *      Buffer.
 * \param writeLen
 *      Buffer length. Writing is only allowed within valid RTC registers.
 * \retval true
 *      Success
 * \retval false
 *      I2C write failed.
 */
bool ErriezDS1307::writeBuffer(uint8_t reg, void *buffer, uint8_t writeLen)
{
    // Start I2C transfer by writing the I2C address, register number and optional buffer
    Wire.beginTransmission(DS1307_ADDR);
    Wire.write(reg);
    for (uint8_t i = 0; i < writeLen; i++) {
        Wire.write(((uint8_t *)buffer)[i]);
    }
    if (Wire.endTransmission(true) != 0) {
        return false;
    }

    return true;
}

/*!
 * \brief Read buffer from RTC.
 * \param reg
 *      RTC register number 0x00..0x07.
 * \param buffer
 *      Buffer.
 * \param readLen
 *      Buffer length. Reading is only allowed within valid RTC registers.
 * \retval true
 *      Success
 * \retval false
 *      I2C read failed.
 */
bool ErriezDS1307::readBuffer(uint8_t reg, void *buffer, uint8_t readLen)
{
    // Start I2C transfer by writing the I2C address and register number
    Wire.beginTransmission(DS1307_ADDR);
    Wire.write(reg);
    // Generate a repeated start, followed by a read buffer
    if (Wire.endTransmission(false) != 0) {
        return false;
    }
    Wire.requestFrom((uint8_t)DS1307_ADDR, readLen);
    for (uint8_t i = 0; i < readLen; i++) {
        ((uint8_t *)buffer)[i] = (uint8_t)Wire.read();
    }

    return true;
}
