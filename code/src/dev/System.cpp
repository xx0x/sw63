#include "System.hpp"

void System::Init()
{
    // Reset of all peripherals, Initializes the Flash interface and the Systick
    HAL_Init();

    // Configure the system clock to use HSI (16MHz)
    System::ClockConfig();

    // Enable power control clock
    __HAL_RCC_PWR_CLK_ENABLE();

    // Additional power optimizations
    // Disable power voltage detector (PVD) to save power
    HAL_PWR_DisablePVD();

    // Initialize GPIO
    System::GpioInit();

    // Initialize I2C
    System::I2cInit();
}

/**
 * @brief System Clock Configuration using internal HSI oscillator
 * @details Configures the system clock to run at 16MHz using HSI
 *          This is crystalless configuration suitable for low-power applications
 */
void System::ClockConfig()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {};

    // Configure the main internal regulator output voltage
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Initialize the RCC Oscillators according to the specified parameters
    // Use HSI (High Speed Internal) 16MHz oscillator
    // Explicitly disable LSE, LSI, and RTC for minimum power consumption
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;      // Disable LSE for power saving
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;      // Disable LSI for power saving
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // Don't use PLL for simplicity

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        ErrorHandler();
    }

    // Initialize the CPU, AHB and APB bus clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // 16MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  // 16MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // 16MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        ErrorHandler();
    }

    // Ensure RTC is completely disabled
    __HAL_RCC_RTC_DISABLE();
}

/**
 * @brief GPIO Initialization for LED on PA15
 * @details Configures PA15 as digital output for LED control
 */
void System::GpioInit()
{
    GPIO_InitTypeDef GPIO_InitStruct = {};

    // GPIO Ports Clock Enable
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure GPIO pin Output Level - start with LED OFF
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

    // Configure GPIO pin PA15 for LED
    // GPIO_InitStruct.Pin = GPIO_PIN_15;
    // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Push-pull output
    // GPIO_InitStruct.Pull = GPIO_NOPULL;          // No pull-up/pull-down
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Low speed is sufficient for LED
    // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure GPIO pin PA00 for BUTTON with interrupt
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Interrupt on falling edge (button press)
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // Pull-up resistor
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Low speed is sufficient for BUTTON
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    

    // Configure GPIO pin PB08 for CHARGE_STATE with interrupt
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Interrupt on falling edge (charging detect)
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // Pull-up resistor
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Low speed is sufficient for CHARGE_STATE
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Enable EXTI0 interrupt for button
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

    // Enable EXTI4_15 interrupt for CHARGE_STATE
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/**
 * @brief I2C1 Initialization Function
 * @details Configures I2C1 for DS3231 RTC communication
 *          Uses PB6 (SCL) and PB7 (SDA) pins
 */
void System::I2cInit()
{
    GPIO_InitTypeDef GPIO_InitStruct = {};

    // Enable I2C1 and GPIOB clocks
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure I2C1 pins: PB6 (SCL) and PB7 (SDA)
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; // Open-drain for I2C
    GPIO_InitStruct.Pull = GPIO_PULLUP;     // Pull-up resistors
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_I2C1; // I2C1 alternate function
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configure I2C1
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00707CBB; // Standard mode (100kHz) timing for 16MHz clock
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        ErrorHandler();
    }

    // Configure Analog noise filter
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        ErrorHandler();
    }

    // Configure Digital noise filter
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        ErrorHandler();
    }
}

/**
 * @brief Error Handler
 * @details This function is executed in case of error occurrence
 *          In a real application, you might want to implement proper error handling
 */
void System::ErrorHandler()
{
    // User can add his own implementation to report the HAL error return state
    __disable_irq();
    while (1)
    {
        // Infinite loop in case of error
        // You could blink LED in a different pattern to indicate error
    }
}

void System::Sleep()
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);

    // Enter Stop mode
    // The system will wake up on EXTI interrupt (button press)
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    // When we wake up from Stop mode, the system clock needs to be reconfigured
    // because the HSI oscillator was stopped
    System::ClockConfig();
}