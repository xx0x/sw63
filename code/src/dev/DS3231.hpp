/*
MIT License

Copyright (c) 2026 Vaclav Mach (xx0x)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "stm32l0xx_hal.h"
#include <array>
#include <cstdint>
#include <optional>

/**
 * @brief Provides DS3231 RTC read/write operations over I2C.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class DS3231
{
public:
    /**
     * @brief Structure to hold date and time information
     * @note This structure is packed to ensure consistent memory layout
     *       for serial communication. Total size: 7 bytes (5×uint8_t + 1×uint16_t)
     *       without padding for proper protocol compatibility.
     */
    struct __attribute__((packed)) DateTime
    {
        uint8_t hour;   // 0-23 (24-hour format)
        uint8_t minute; // 0-59
        uint8_t second; // 0-59
        uint8_t day;    // 1-31
        uint8_t month;  // 1-12
        uint16_t year;  // Full year (e.g., 2024)
    };

    /**
     * @brief 7-bit I2C address of DS3231.
     */
    static constexpr uint8_t I2C_ADDRESS = 0x68;

    /**
     * @brief DS3231 register address constants.
     */
    static constexpr uint8_t REG_SECONDS = 0x00;
    static constexpr uint8_t REG_MINUTES = 0x01;
    static constexpr uint8_t REG_HOURS = 0x02;
    static constexpr uint8_t REG_DAY = 0x03;
    static constexpr uint8_t REG_DATE = 0x04;
    static constexpr uint8_t REG_MONTH = 0x05;
    static constexpr uint8_t REG_YEAR = 0x06;
    static constexpr uint8_t REG_ALARM1_SECONDS = 0x07;
    static constexpr uint8_t REG_ALARM1_MINUTES = 0x08;
    static constexpr uint8_t REG_ALARM1_HOURS = 0x09;
    static constexpr uint8_t REG_ALARM1_DAY_DATE = 0x0A;
    static constexpr uint8_t REG_ALARM2_MINUTES = 0x0B;
    static constexpr uint8_t REG_ALARM2_HOURS = 0x0C;
    static constexpr uint8_t REG_ALARM2_DAY_DATE = 0x0D;
    static constexpr uint8_t REG_CONTROL = 0x0E;
    static constexpr uint8_t REG_STATUS = 0x0F;
    static constexpr uint8_t REG_AGING_OFFSET = 0x10;
    static constexpr uint8_t REG_TEMP_MSB = 0x11;
    static constexpr uint8_t REG_TEMP_LSB = 0x12;

    /**
     * @brief Control register bit masks.
     */
    static constexpr uint8_t CTRL_EOSC = 0x80;  // Enable oscillator (0 = enabled)
    static constexpr uint8_t CTRL_BBSQW = 0x40; // Battery-backed square wave
    static constexpr uint8_t CTRL_CONV = 0x20;  // Convert temperature
    static constexpr uint8_t CTRL_RS2 = 0x10;   // Rate select bit 2
    static constexpr uint8_t CTRL_RS1 = 0x08;   // Rate select bit 1
    static constexpr uint8_t CTRL_INTCN = 0x04; // Interrupt control
    static constexpr uint8_t CTRL_A2IE = 0x02;  // Alarm 2 interrupt enable
    static constexpr uint8_t CTRL_A1IE = 0x01;  // Alarm 1 interrupt enable

    /**
     * @brief Status register bit masks.
     */
    static constexpr uint8_t STAT_OSF = 0x80;     // Oscillator stop flag
    static constexpr uint8_t STAT_EN32KHZ = 0x08; // Enable 32kHz output
    static constexpr uint8_t STAT_BSY = 0x04;     // Busy
    static constexpr uint8_t STAT_A2F = 0x02;     // Alarm 2 flag
    static constexpr uint8_t STAT_A1F = 0x01;     // Alarm 1 flag

    /**
     * @brief Additional DS3231 bit masks used by the driver.
     */
    static constexpr uint8_t ALARM_MASK = 0x80;  // Alarm mask bit
    static constexpr uint8_t DYDT_BIT = 0x40;    // Day/Date bit in alarm registers
    static constexpr uint8_t HOUR_12_24 = 0x40;  // 12/24 hour format bit
    static constexpr uint8_t CENTURY_BIT = 0x80; // Century bit in month register

    /**
     * @brief Constructs DS3231 driver instance.
     * @param hi2c Pointer to I2C peripheral handle.
     * @param timeout I2C timeout in milliseconds.
     */
    DS3231(I2C_HandleTypeDef *hi2c, uint32_t timeout = 1000);

    /**
     * @brief Initializes DS3231 operating mode.
     * @return True when initialization succeeds.
     */
    bool Init();

    /**
     * @brief Writes current date and time to DS3231.
     * @param date_time New date-time values.
     * @return True when write succeeds.
     */
    bool SetDateTime(const DateTime &date_time);

    /**
     * @brief Reads current date and time from DS3231.
     * @return Date-time value or std::nullopt on failure.
     */
    std::optional<DateTime> GetDateTime() const;

    /**
     * @brief Reads internal DS3231 temperature sensor.
     * @return Temperature in Celsius or std::nullopt on failure.
     */
    std::optional<float> GetTemperature() const;

    /**
     * @brief Checks oscillator stop flag state.
     * @param clear_flag Clears oscillator stop flag when true.
     * @return True when oscillator stop flag is set.
     */
    bool HasOscillatorStopped(bool clear_flag = false);

    /**
     * @brief Triggers immediate DS3231 temperature conversion.
     * @return True when conversion command succeeds.
     */
    bool ForceTemperatureConversion();

private:
    I2C_HandleTypeDef *const hi2c_;
    const uint32_t timeout_;

    /**
     * @brief Reads one byte from DS3231 register.
     * @param reg Register address.
     * @param data Pointer to output byte.
     * @return True when read succeeds.
     */
    bool ReadRegister(uint8_t reg, uint8_t *data) const;

    /**
     * @brief Reads multiple bytes from DS3231.
     * @param reg Starting register address.
     * @param data Pointer to output buffer.
     * @param length Number of bytes to read.
     * @return True when read succeeds.
     */
    bool ReadRegisters(uint8_t reg, uint8_t *data, uint8_t length) const;

    /**
     * @brief Writes one byte to DS3231 register.
     * @param reg Register address.
     * @param data Byte value to write.
     * @return True when write succeeds.
     */
    bool WriteRegister(uint8_t reg, uint8_t data);

    /**
     * @brief Writes multiple bytes to DS3231.
     * @param reg Starting register address.
     * @param data Pointer to input data buffer.
     * @param length Number of bytes to write.
     * @return True when write succeeds.
     */
    bool WriteRegisters(uint8_t reg, const uint8_t *data, uint8_t length);

    /**
     * @brief Converts decimal byte to BCD format.
     * @param val Decimal value.
     * @return BCD-encoded value.
     */
    static constexpr uint8_t DecToBcd(uint8_t val)
    {
        return ((val / 10) << 4) | (val % 10);
    }

    /**
     * @brief Converts BCD byte to decimal format.
     * @param val BCD-encoded value.
     * @return Decimal value.
     */
    static constexpr uint8_t BcdToDec(uint8_t val)
    {
        return ((val >> 4) * 10) + (val & 0x0F);
    }
};
