#pragma once

#include <bitset>

#include "Pin.hpp"
#include "stm32l0xx_hal.h"

template <size_t N>
class ShiftRegister
{
public:
    struct Config
    {
        Pin data;
        Pin clock;
        Pin latch;
    };

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
