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

/**
 * @brief Provides static system-level hardware initialization and helpers.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class System
{
public:
    // Static member variables
    inline static I2C_HandleTypeDef hi2c1;

    // Static methods
    /**
     * @brief Initializes MCU peripherals and platform services.
     */
    static void Init();

    /**
     * @brief Configures system clock tree.
     */
    static void ClockConfig();

    /**
     * @brief Initializes GPIO peripherals.
     */
    static void GpioInit();

    /**
     * @brief Initializes I2C peripheral.
     */
    static void I2cInit();

    /**
     * @brief Initializes USB peripheral.
     */
    static void UsbInit();

    /**
     * @brief Handles unrecoverable system error.
     */
    static void ErrorHandler();

    /**
     * @brief Enters low-power sleep mode.
     */
    static void Sleep();

    /**
     * @brief Delays execution for requested milliseconds.
     * @param ms Delay duration in milliseconds.
     */
    static inline void Delay(uint32_t ms)
    {
        HAL_Delay(ms);
    }

    /**
     * @brief Returns system tick count.
     * @return Milliseconds since startup.
     */
    static inline uint32_t Millis()
    {
        return HAL_GetTick();
    }

    /**
     * @brief Reads raw button electrical state.
     * @return True when button input is active.
     */
    static bool GetRawButtonState()
    {
        return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET; // Active low
    }

    /**
     * @brief Reads raw charging status input.
     * @return True when charging input is active.
     */
    static bool GetRawChargeState()
    {
        return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_RESET; // Active low
    }

    /**
     * @brief Reads raw USB power presence input.
     * @return True when USB power is present.
     */
    static bool GetRawUsbPowerState()
    {
        return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_SET; // Active high
    }

private:
    System() = delete; // Prevent instantiation
};