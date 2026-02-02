#pragma once

#include "stm32l0xx_hal.h"

struct Pin
{
    GPIO_TypeDef *port;
    uint16_t pin;

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
