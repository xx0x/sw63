#include "DS3231.hpp"

DS3231::DS3231(I2C_HandleTypeDef *hi2c, uint32_t timeout)
    : hi2c_(hi2c), timeout_(timeout)
{
}

bool DS3231::Init()
{
    if (!hi2c_)
    {
        return false;
    }

    // Check if DS3231 is present by trying to read a register
    uint8_t dummy;
    if (!ReadRegister(REG_SECONDS, &dummy))
    {
        return false;
    }

    // Clear oscillator stop flag if set
    uint8_t status;
    if (ReadRegister(REG_STATUS, &status))
    {
        if (status & STAT_OSF)
        {
            status &= ~STAT_OSF;
            WriteRegister(REG_STATUS, status);
        }
    }

    return true;
}

bool DS3231::SetDateTime(const DS3231::DateTime &date_time)
{
    std::array<uint8_t, 7> data{};

    // Convert to BCD format
    data[0] = DecToBcd(date_time.second);
    data[1] = DecToBcd(date_time.minute);
    data[2] = DecToBcd(date_time.hour); // 24-hour format
    data[3] = DecToBcd(date_time.day_of_week);
    data[4] = DecToBcd(date_time.day);
    data[5] = DecToBcd(date_time.month);
    data[6] = DecToBcd(date_time.year % 100); // Only last two digits

    // Handle century bit for years >= 2000
    if (date_time.year >= 2000)
    {
        data[5] |= CENTURY_BIT;
    }

    return WriteRegisters(REG_SECONDS, data.data(), data.size());
}

std::optional<DS3231::DateTime> DS3231::GetDateTime() const
{
    std::array<uint8_t, 7> data{};

    if (!ReadRegisters(REG_SECONDS, data.data(), data.size()))
    {
        return std::nullopt;
    }

    DateTime date_time{};

    // Convert from BCD format
    date_time.second = BcdToDec(data[0] & 0x7F);
    date_time.minute = BcdToDec(data[1] & 0x7F);
    date_time.hour = BcdToDec(data[2] & 0x3F); // Mask 12/24 hour bit
    date_time.day_of_week = BcdToDec(data[3] & 0x07);
    date_time.day = BcdToDec(data[4] & 0x3F);

    const uint8_t month = data[5];
    date_time.month = BcdToDec(month & 0x1F);
    date_time.year = BcdToDec(data[6]);

    // Handle century bit
    if (month & CENTURY_BIT)
    {
        date_time.year += 2000;
    }
    else
    {
        date_time.year += 1900;
    }

    return date_time;
}

bool DS3231::SetAlarm1(DS3231::AlarmOneType alarm_type, const DS3231::DateTime &date_time, bool enable)
{
    std::array<uint8_t, 4> data{};
    const auto alarmMask = static_cast<uint8_t>(alarm_type);

    // Set alarm 1 registers
    data[0] = DecToBcd(date_time.second) | ((alarmMask & 0x01) ? ALARM_MASK : 0);
    data[1] = DecToBcd(date_time.minute) | ((alarmMask & 0x02) ? ALARM_MASK : 0);
    data[2] = DecToBcd(date_time.hour) | ((alarmMask & 0x04) ? ALARM_MASK : 0);

    // Day/Date field
    if (alarm_type == DS3231::AlarmOneType::DAY)
    {
        data[3] = DecToBcd(date_time.day_of_week) | DYDT_BIT;
    }
    else
    {
        data[3] = DecToBcd(date_time.day);
    }
    data[3] |= ((alarmMask & 0x08) ? ALARM_MASK : 0);

    if (!WriteRegisters(REG_ALARM1_SECONDS, data.data(), data.size()))
    {
        return false;
    }

    // Enable/disable alarm interrupt
    return EnableAlarm1Interrupt(enable);
}

bool DS3231::SetAlarm2(DS3231::AlarmTwoType alarm_type, const DS3231::DateTime &date_time, bool enable)
{
    std::array<uint8_t, 3> data{};
    const auto alarmMask = static_cast<uint8_t>(alarm_type);

    // Set alarm 2 registers (no seconds register for alarm 2)
    data[0] = DecToBcd(date_time.minute) | ((alarmMask & 0x02) ? ALARM_MASK : 0);
    data[1] = DecToBcd(date_time.hour) | ((alarmMask & 0x04) ? ALARM_MASK : 0);

    // Day/Date field
    if (alarm_type == DS3231::AlarmTwoType::DAY)
    {
        data[2] = DecToBcd(date_time.day_of_week) | DYDT_BIT;
    }
    else
    {
        data[2] = DecToBcd(date_time.day);
    }
    data[2] |= ((alarmMask & 0x08) ? ALARM_MASK : 0);

    if (!WriteRegisters(REG_ALARM2_MINUTES, data.data(), data.size()))
    {
        return false;
    }

    // Enable/disable alarm interrupt
    return EnableAlarm2Interrupt(enable);
}

bool DS3231::CheckAlarm1() const
{
    uint8_t status;
    if (!ReadRegister(REG_STATUS, &status))
    {
        return false;
    }
    return (status & STAT_A1F) != 0;
}

bool DS3231::CheckAlarm2() const
{
    uint8_t status;
    if (!ReadRegister(REG_STATUS, &status))
    {
        return false;
    }
    return (status & STAT_A2F) != 0;
}

bool DS3231::ClearAlarm1()
{
    uint8_t status;
    if (!ReadRegister(REG_STATUS, &status))
    {
        return false;
    }
    status &= ~STAT_A1F;
    return WriteRegister(REG_STATUS, status);
}

bool DS3231::ClearAlarm2()
{
    uint8_t status;
    if (!ReadRegister(REG_STATUS, &status))
    {
        return false;
    }
    status &= ~STAT_A2F;
    return WriteRegister(REG_STATUS, status);
}

bool DS3231::EnableAlarm1Interrupt(bool enable)
{
    uint8_t control;
    if (!ReadRegister(REG_CONTROL, &control))
    {
        return false;
    }

    if (enable)
    {
        control |= CTRL_A1IE | CTRL_INTCN; // Enable alarm 1 interrupt and interrupt control
    }
    else
    {
        control &= ~CTRL_A1IE;
    }

    return WriteRegister(REG_CONTROL, control);
}

bool DS3231::EnableAlarm2Interrupt(bool enable)
{
    uint8_t control;
    if (!ReadRegister(REG_CONTROL, &control))
    {
        return false;
    }

    if (enable)
    {
        control |= CTRL_A2IE | CTRL_INTCN; // Enable alarm 2 interrupt and interrupt control
    }
    else
    {
        control &= ~CTRL_A2IE;
    }

    return WriteRegister(REG_CONTROL, control);
}

bool DS3231::SetSquareWave(DS3231::SquareWaveFreq freq)
{
    uint8_t control;
    if (!ReadRegister(REG_CONTROL, &control))
    {
        return false;
    }

    if (freq == DS3231::SquareWaveFreq::DISABLED)
    {
        // Disable square wave output
        control |= CTRL_INTCN;
    }
    else
    {
        // Enable square wave and set frequency
        control &= ~(CTRL_INTCN | CTRL_RS2 | CTRL_RS1);
        control |= static_cast<uint8_t>(freq);
    }

    return WriteRegister(REG_CONTROL, control);
}

bool DS3231::Enable32kHz(bool enable)
{
    uint8_t status;
    if (!ReadRegister(REG_STATUS, &status))
    {
        return false;
    }

    if (enable)
    {
        status |= STAT_EN32KHZ;
    }
    else
    {
        status &= ~STAT_EN32KHZ;
    }

    return WriteRegister(REG_STATUS, status);
}

std::optional<float> DS3231::GetTemperature() const
{
    std::array<uint8_t, 2> data{};

    if (!ReadRegisters(REG_TEMP_MSB, data.data(), data.size()))
    {
        return std::nullopt;
    }

    // Convert temperature (MSB + LSB[7:6] * 0.25)
    auto temp = static_cast<int16_t>((static_cast<int16_t>(data[0]) << 2) | (data[1] >> 6));

    // Handle negative temperatures (two's complement)
    if (data[0] & 0x80)
    {
        temp |= 0xFC00; // Sign extend
    }

    return static_cast<float>(temp) * 0.25f;
}

bool DS3231::HasOscillatorStopped(bool clear_flag)
{
    uint8_t status;
    if (!ReadRegister(REG_STATUS, &status))
    {
        return false;
    }

    const bool stopped = (status & STAT_OSF) != 0;

    if (stopped && clear_flag)
    {
        status &= ~STAT_OSF;
        WriteRegister(REG_STATUS, status);
    }

    return stopped;
}

bool DS3231::ForceTemperatureConversion()
{
    uint8_t control;
    if (!ReadRegister(REG_CONTROL, &control))
    {
        return false;
    }

    control |= CTRL_CONV;
    if (!WriteRegister(REG_CONTROL, control))
    {
        return false;
    }

    // Wait for conversion to complete
    uint32_t startTime = HAL_GetTick();
    while (HAL_GetTick() - startTime < 1000)
    { // 1 second timeout
        if (ReadRegister(REG_CONTROL, &control))
        {
            if (!(control & CTRL_CONV))
            {
                return true; // Conversion complete
            }
        }
        HAL_Delay(10);
    }

    return false; // Timeout
}

bool DS3231::ReadRegister(uint8_t reg, uint8_t *data) const
{
    const HAL_StatusTypeDef result = HAL_I2C_Mem_Read(hi2c_, I2C_ADDRESS << 1, reg,
                                                      I2C_MEMADD_SIZE_8BIT, data, 1, timeout_);
    return result == HAL_OK;
}

bool DS3231::ReadRegisters(uint8_t reg, uint8_t *data, uint8_t length) const
{
    const HAL_StatusTypeDef result = HAL_I2C_Mem_Read(hi2c_, I2C_ADDRESS << 1, reg,
                                                      I2C_MEMADD_SIZE_8BIT, data, length, timeout_);
    return result == HAL_OK;
}

bool DS3231::WriteRegister(uint8_t reg, uint8_t data)
{
    const HAL_StatusTypeDef result = HAL_I2C_Mem_Write(hi2c_, I2C_ADDRESS << 1, reg,
                                                       I2C_MEMADD_SIZE_8BIT, &data, 1, timeout_);
    return result == HAL_OK;
}

bool DS3231::WriteRegisters(uint8_t reg, const uint8_t *data, uint8_t length)
{
    const HAL_StatusTypeDef result = HAL_I2C_Mem_Write(hi2c_, I2C_ADDRESS << 1, reg,
                                                       I2C_MEMADD_SIZE_8BIT, const_cast<uint8_t *>(data),
                                                       length, timeout_);
    return result == HAL_OK;
}
