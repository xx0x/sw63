#pragma once

#include "stm32l0xx_hal.h"
#include <array>
#include <cstdint>
#include <optional>

/**
 * DS3231 Real-Time Clock driver class
 */
class DS3231
{
public:
    /**
     * Structure to hold date and time information
     */
    struct DateTime
    {
        uint8_t second;      // 0-59
        uint8_t minute;      // 0-59
        uint8_t hour;        // 0-23 (24-hour format)
        uint8_t day_of_week; // 1-7 (1 = Monday)
        uint8_t day;         // 1-31
        uint8_t month;       // 1-12
        uint16_t year;       // Full year (e.g., 2024)
    };

    /**
     * DS3231 alarm one types
     */
    enum class AlarmOneType : uint8_t
    {
        EVERY_SECOND = 0x0F, // Trigger every second
        SECONDS = 0x0E,      // Specify seconds
        MINUTES = 0x0C,      // Specify minutes and seconds
        HOURS = 0x08,        // Specify hours, minutes, and seconds
        DATE = 0x00,         // Specify date, hours, minutes, and seconds
        DAY = 0x10,          // Specify day of week, hours, minutes, and seconds
    };

    /**
     * DS3231 alarm two types
     */
    enum class AlarmTwoType : uint8_t
    {
        EVERY_MINUTE = 0x8E, // Trigger every minute
        MINUTES = 0x8C,      // Specify minutes (alarm 2)
        HOURS = 0x88,        // Specify hours and minutes (alarm 2)
        DATE = 0x80,         // Specify date, hours, and minutes (alarm 2)
        DAY = 0x90           // Specify day of week, hours, and minutes (alarm 2)
    };

    /**
     * Square wave output frequencies
     */
    enum class SquareWaveFreq : uint8_t
    {
        FREQ_1HZ = 0x00,
        FREQ_1024HZ = 0x08,
        FREQ_4096HZ = 0x10,
        FREQ_8192HZ = 0x18,
        DISABLED = 0xFF
    };

    // DS3231 I2C address
    static constexpr uint8_t I2C_ADDRESS = 0x68;

    // Register addresses
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

    // Control register bits
    static constexpr uint8_t CTRL_EOSC = 0x80;  // Enable oscillator (0 = enabled)
    static constexpr uint8_t CTRL_BBSQW = 0x40; // Battery-backed square wave
    static constexpr uint8_t CTRL_CONV = 0x20;  // Convert temperature
    static constexpr uint8_t CTRL_RS2 = 0x10;   // Rate select bit 2
    static constexpr uint8_t CTRL_RS1 = 0x08;   // Rate select bit 1
    static constexpr uint8_t CTRL_INTCN = 0x04; // Interrupt control
    static constexpr uint8_t CTRL_A2IE = 0x02;  // Alarm 2 interrupt enable
    static constexpr uint8_t CTRL_A1IE = 0x01;  // Alarm 1 interrupt enable

    // Status register bits
    static constexpr uint8_t STAT_OSF = 0x80;     // Oscillator stop flag
    static constexpr uint8_t STAT_EN32KHZ = 0x08; // Enable 32kHz output
    static constexpr uint8_t STAT_BSY = 0x04;     // Busy
    static constexpr uint8_t STAT_A2F = 0x02;     // Alarm 2 flag
    static constexpr uint8_t STAT_A1F = 0x01;     // Alarm 1 flag

    // Other useful bits
    static constexpr uint8_t ALARM_MASK = 0x80;  // Alarm mask bit
    static constexpr uint8_t DYDT_BIT = 0x40;    // Day/Date bit in alarm registers
    static constexpr uint8_t HOUR_12_24 = 0x40;  // 12/24 hour format bit
    static constexpr uint8_t CENTURY_BIT = 0x80; // Century bit in month register

    /**
     * Constructor
     * @param hi2c Pointer to I2C handle
     * @param timeout I2C timeout in milliseconds (default: 1000)
     */
    DS3231(I2C_HandleTypeDef *hi2c, uint32_t timeout = 1000);

    /**
     * Initialize the DS3231
     * @return true if initialization successful
     */
    bool Init();

    /**
     * Set the current date and time
     * @param date_time DateTime structure with the new time
     * @return true if successful
     */
    bool SetDateTime(const DateTime &date_time);

    /**
     * Get the current date and time
     * @return Optional DateTime if successful, nullopt on error
     */
    std::optional<DateTime> GetDateTime() const;

    /**
     * Set alarm 1
     * @param alarm_type Type of alarm to set
     * @param date_time DateTime structure with alarm time
     * @param enable Enable the alarm after setting
     * @return true if successful
     */
    bool SetAlarm1(AlarmOneType alarm_type, const DateTime &date_time, bool enable = true);

    /**
     * Set alarm 2
     * @param alarm_type Type of alarm to set (must be alarm 2 type)
     * @param date_time DateTime structure with alarm time
     * @param enable Enable the alarm after setting
     * @return true if successful
     */
    bool SetAlarm2(AlarmTwoType alarm_type, const DateTime &date_time, bool enable = true);

    /**
     * Check if alarm 1 has triggered
     * @return true if alarm 1 flag is set
     */
    bool CheckAlarm1() const;

    /**
     * Check if alarm 2 has triggered
     * @return true if alarm 2 flag is set
     */
    bool CheckAlarm2() const;

    /**
     * Clear alarm 1 flag
     * @return true if successful
     */
    bool ClearAlarm1();

    /**
     * Clear alarm 2 flag
     * @return true if successful
     */
    bool ClearAlarm2();

    /**
     * Enable or disable alarm 1 interrupt
     * @param enable true to enable, false to disable
     * @return true if successful
     */
    bool EnableAlarm1Interrupt(bool enable);

    /**
     * Enable or disable alarm 2 interrupt
     * @param enable true to enable, false to disable
     * @return true if successful
     */
    bool EnableAlarm2Interrupt(bool enable);

    /**
     * Set square wave output frequency
     * @param freq Desired frequency
     * @return true if successful
     */
    bool SetSquareWave(SquareWaveFreq freq);

    /**
     * Enable or disable 32kHz output
     * @param enable true to enable, false to disable
     * @return true if successful
     */
    bool Enable32kHz(bool enable);

    /**
     * Get temperature from DS3231
     * @return Optional temperature in Celsius (resolution 0.25°C)
     */
    std::optional<float> GetTemperature() const;

    /**
     * Check if the oscillator has stopped
     * @param clear_flag If true, clear the OSF flag after reading
     * @return true if oscillator has stopped
     */
    bool HasOscillatorStopped(bool clear_flag = false);

    /**
     * Force a temperature conversion
     * @return true if successful
     */
    bool ForceTemperatureConversion();

private:
    I2C_HandleTypeDef *const hi2c_;
    const uint32_t timeout_;

    /**
     * Read a single byte from DS3231
     * @param reg Register address
     * @param data Pointer to store the read byte
     * @return true if successful
     */
    bool ReadRegister(uint8_t reg, uint8_t *data) const;

    /**
     * Read multiple bytes from DS3231
     * @param reg Starting register address
     * @param data Pointer to buffer for read data
     * @param length Number of bytes to read
     * @return true if successful
     */
    bool ReadRegisters(uint8_t reg, uint8_t *data, uint8_t length) const;

    /**
     * Write a single byte to DS3231
     * @param reg Register address
     * @param data Byte to write
     * @return true if successful
     */
    bool WriteRegister(uint8_t reg, uint8_t data);

    /**
     * Write multiple bytes to DS3231
     * @param reg Starting register address
     * @param data Pointer to data to write
     * @param length Number of bytes to write
     * @return true if successful
     */
    bool WriteRegisters(uint8_t reg, const uint8_t *data, uint8_t length);

    /**
     * Convert decimal to BCD
     * @param val Decimal value
     * @return BCD value
     */
    static constexpr uint8_t DecToBcd(uint8_t val)
    {
        return ((val / 10) << 4) | (val % 10);
    }

    /**
     * Convert BCD to decimal
     * @param val BCD value
     * @return Decimal value
     */
    static constexpr uint8_t BcdToDec(uint8_t val)
    {
        return ((val >> 4) * 10) + (val & 0x0F);
    }
};
