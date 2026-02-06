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
    data[3] = 0;                        // Day of week not used
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
