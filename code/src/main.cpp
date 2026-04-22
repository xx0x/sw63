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

#include "app/App.hpp"
#include "dev/System.hpp"
#include "stm32l052xx.h"
#include "stm32l0xx_hal.h"
#include "tusb.h"
#include <bitset>

/**
 * @brief Main program entry point
 * @retval int
 */
int main(void)
{
    // Initialize System
    System::Init();

    // Initialize App
    App::Init();

    // Main loop
    while (1)
    {
        App::Loop();
    }
}

/**
 * @brief SysTick interrupt handler
 * @note This callback is called every 1ms by SysTick interrupt
 *       Required for HAL_Delay() to work properly
 */
extern "C" void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 * @brief Hard Fault interrupt handler
 * @note Basic fault handler - in production code you'd want more sophisticated handling
 */
extern "C" void HardFault_Handler(void)
{
    while (1)
    {
        // Stay here in case of Hard Fault
    }
}

/**
 * @brief Non-Maskable Interrupt handler
 */
extern "C" void NMI_Handler(void)
{
    // Handle NMI if needed
}

/**
 * @brief EXTI0_1 interrupt handler for button wakeup
 * @note Handles external interrupt on PA0 (button) to wake up from stop mode
 */
extern "C" void EXTI0_1_IRQHandler(void)
{
    // Check if the interrupt is from PA0
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET)
    {
        // Clear the interrupt flag
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

        // The system will automatically exit from stop mode
        // No additional code needed here for basic wakeup
    }
}

extern "C" void EXTI4_15_IRQHandler()
{
    // Power detect (power plugged in)
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
    }
}

/**
 * @brief USB interrupt handler
 * @note Handles USB interrupts for TinyUSB stack
 */
extern "C" void USB_IRQHandler(void)
{
    tud_int_handler(0);
}