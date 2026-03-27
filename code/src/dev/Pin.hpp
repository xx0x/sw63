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
 * @brief Stores GPIO port and pin pair.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
struct Pin
{
    GPIO_TypeDef *port;
    uint16_t pin;

    /**
     * @brief Enables RCC clock for selected GPIO port.
     * @param p GPIO pin descriptor.
     */
    static void EnablePort(const Pin &p)
    {
        // Enable GPIO port clock based on port
        if (p.port == GPIOA)
        {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        }
        else if (p.port == GPIOB)
        {
            __HAL_RCC_GPIOB_CLK_ENABLE();
        }
        else if (p.port == GPIOC)
        {
            __HAL_RCC_GPIOC_CLK_ENABLE();
        }
        else if (p.port == GPIOD)
        {
            __HAL_RCC_GPIOD_CLK_ENABLE();
        }
    }
};
