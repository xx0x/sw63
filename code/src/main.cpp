#include "app/App.hpp"
#include "dev/System.hpp"
#include "stm32l052xx.h"
#include "stm32l0xx_hal.h"
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

        // Set button pressed flag?

        // The system will automatically exit from stop mode
        // No additional code needed here for basic wakeup
    }
}

extern "C" void EXTI4_15_IRQHandler()
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
        // Handle charge state change
    }
}