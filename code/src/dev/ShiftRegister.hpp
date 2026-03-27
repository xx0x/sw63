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

#include <bitset>

#include "Pin.hpp"
#include "stm32l0xx_hal.h"

template <size_t N>
/**
 * @brief Writes bit data to cascaded shift register outputs.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class ShiftRegister
{
public:
    /**
     * @brief Defines GPIO assignments for shift register signals.
     */
    struct Config
    {
        Pin data;
        Pin clock;
        Pin latch;
    };

    /**
     * @brief Initializes shift register GPIO pins.
     * @param config Data, clock, and latch pin configuration.
     * @return True when initialization succeeds.
     */
    bool Init(const Config &config)
    {
        config_ = config;

        // Initialize GPIO pins
        GPIO_InitTypeDef GPIO_InitStruct = {};

        // Data pin
        GPIO_InitStruct.Pin = config_.data.pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(config_.data.port, &GPIO_InitStruct);
        Pin::EnablePort(config_.data);

        // Clock pin
        GPIO_InitStruct.Pin = config_.clock.pin;
        HAL_GPIO_Init(config_.clock.port, &GPIO_InitStruct);
        Pin::EnablePort(config_.clock);

        // Latch pin
        GPIO_InitStruct.Pin = config_.latch.pin;
        HAL_GPIO_Init(config_.latch.port, &GPIO_InitStruct);
        Pin::EnablePort(config_.latch);

        initialized_ = true;
        return true;
    }

    /**
     * @brief Deinitializes shift register GPIO pins.
     * @return True when deinitialization succeeds.
     */
    bool DeInit()
    {
        if (!initialized_)
        {
            return false;
        }
        HAL_GPIO_DeInit(config_.data.port, config_.data.pin);
        HAL_GPIO_DeInit(config_.clock.port, config_.clock.pin);
        HAL_GPIO_DeInit(config_.latch.port, config_.latch.pin);
        initialized_ = false;
        return true;
    }

    /**
     * @brief Shifts one full bitset frame to the output chain.
     * @param data Bitset payload to write.
     */
    void Write(const std::bitset<N> &data)
    {
        if (!initialized_)
        {
            return;
        }
        HAL_GPIO_WritePin(config_.latch.port, config_.latch.pin, GPIO_PIN_RESET); // LATCH low
        // HAL_Delay(1);
        for (size_t i = 0; i < N; ++i)
        {
            HAL_GPIO_WritePin(config_.data.port, config_.data.pin, data[i] ? GPIO_PIN_SET : GPIO_PIN_RESET); // DATA
            HAL_GPIO_WritePin(config_.clock.port, config_.clock.pin, GPIO_PIN_SET);                          // CLOCK high
            // HAL_Delay(1);
            HAL_GPIO_WritePin(config_.clock.port, config_.clock.pin, GPIO_PIN_RESET); // CLOCK low
        }
        HAL_GPIO_WritePin(config_.latch.port, config_.latch.pin, GPIO_PIN_SET); // LATCH high
    }

private:
    Config config_;
    bool initialized_ = false;
};
