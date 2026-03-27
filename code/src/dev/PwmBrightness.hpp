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
 * @brief Drives display brightness with PWM on the OE pin.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class PwmBrightness
{
public:
    /**
     * @brief Maximum 12-bit PWM compare value.
     */
    static constexpr uint16_t kResolution = 4095; // 12-bit max value

    /**
     * @brief Preconfigures OE pin to avoid startup display glitches.
     */
    void PreInit()
    {
        GPIO_InitTypeDef GPIO_InitStruct = {};
        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    }

    /**
     * @brief Initializes TIM22 PWM channel for brightness control.
     * @return True when initialization succeeds.
     */
    bool Init()
    {
        // Clear the preinit
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);

        TIM_OC_InitTypeDef sConfigOC = {};
        GPIO_InitTypeDef GPIO_InitStruct = {};

        // Enable TIM22 clock
        __HAL_RCC_TIM22_CLK_ENABLE();

        // Configure PA6 as TIM22_CH1 (AF5)
        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_TIM22;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // Configure TIM22 for PWM
        // With 16MHz clock, prescaler 15, and period 4095:
        // PWM frequency = 16,000,000 / (7+1) / (4095+1) = ~488 Hz
        htim_.Instance = TIM22;
        htim_.Init.Prescaler = 7; // Divide by 8 to reduce EMI interference with shift register
        htim_.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim_.Init.Period = kResolution;
        htim_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim_.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        if (HAL_TIM_PWM_Init(&htim_) != HAL_OK)
        {
            return false;
        }

        // Configure PWM channel 1
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = 0; // Start with 0% duty cycle
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

        if (HAL_TIM_PWM_ConfigChannel(&htim_, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
        {
            return false;
        }

        // Start PWM generation
        if (HAL_TIM_PWM_Start(&htim_, TIM_CHANNEL_1) != HAL_OK)
        {
            return false;
        }

        // Initialization successful
        initialized_ = true;
        return true;
    }

    /**
     * @brief Stops PWM output and releases related resources.
     * @return True when deinitialization succeeds.
     */
    bool DeInit()
    {
        if (!initialized_)
        {
            return false;
        }
        HAL_TIM_PWM_Stop(&htim_, TIM_CHANNEL_1);
        HAL_TIM_PWM_DeInit(&htim_);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
        initialized_ = false;
        return true;
    }

    /**
     * @brief Sets display brightness level.
     * @param brightness Brightness value in range 0 to kResolution.
     */
    void Set(uint16_t brightness)
    {
        if (!initialized_)
        {
            return;
        }

        // Clamp brightness to 10-bit range
        if (brightness > kResolution)
        {
            brightness = kResolution;
        }

        // Invert brightness for OE pin (kResolution - brightness)
        // This way: 0 = LEDs off, kResolution = LEDs full brightness
        uint16_t inverted_brightness = kResolution - brightness;

        // Update PWM duty cycle
        __HAL_TIM_SET_COMPARE(&htim_, TIM_CHANNEL_1, inverted_brightness);
    }

    /**
     * @brief Sets minimum brightness level.
     */
    void Min()
    {
        Set(0);
    }

    /**
     * @brief Sets maximum brightness level.
     */
    void Max()
    {
        Set(kResolution);
    }

private:
    TIM_HandleTypeDef htim_;
    bool initialized_ = false;
};
