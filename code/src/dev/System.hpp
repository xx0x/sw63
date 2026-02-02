#pragma once

#include "stm32l0xx_hal.h"

class System
{
public:
    // Static member variables
    inline static I2C_HandleTypeDef hi2c1;

    // Static methods
    static void Init();
    static void ClockConfig();
    static void GpioInit();
    static void I2cInit();
    static void ErrorHandler();
    static void Sleep();
    static inline void Delay(uint32_t ms)
    {
        HAL_Delay(ms);
    }

    static bool GetRawButtonState()
    {
        return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET; // Active low
    }

    static bool GetRawChargeState()
    {
        return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_RESET; // Active low
    }

private:
    System() = delete; // Prevent instantiation
};