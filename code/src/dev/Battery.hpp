#pragma once
#include "dev/AdcInput.hpp"
#include "dev/Pin.hpp"
#include "utils/math_utils.hpp"
#include "stm32l0xx_hal.h"
#include <algorithm>

class Battery
{

public:
    void Init()
    {
        // Initialize battery ADC (PA3)
        battery_adc_.Init({.port = GPIOA,
                           .pin = GPIO_PIN_3,
                           .channel = ADC_CHANNEL_3});

        // Initialize internal voltage reference ADC
        vrefint_adc_.Init({.port = nullptr, // Internal channel, no external pin
                           .pin = 0,
                           .channel = ADC_CHANNEL_VREFINT});
    }

    float GetLevel()
    {
        // Initialize GPIO for battery measurement enable (PB1)
        GPIO_InitTypeDef GPIO_InitStruct = {
            .Pin = GPIO_PIN_1,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = 0,
        };
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // Enable measurement (make divider active)
        Pin::EnablePort({GPIOB, GPIO_PIN_1});

        // Wait for stabilization
        HAL_Delay(5);

        // Read battery voltage via ADC
        auto adc_result = battery_adc_.ReadSingleShot();
        if (!adc_result.has_value())
        {
            // ADC read failed, deactivate divider and return invalid voltage
            HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);
            return 0.0f;
        }
        uint32_t adc_value = adc_result.value();

        // We are done, deactivate divider
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);

        // Get the actual ADC reference voltage (compensated for supply voltage drop)
        float actual_vref = GetActualVref();

        // Convert ADC value to voltage at measurement point using actual reference
        // ADC: 12-bit (0-4095), 0-actual_vref range
        float measured_voltage = (static_cast<float>(adc_value) / static_cast<float>(AdcInput::kResolution)) * actual_vref;

        // Compensation (measured from real use)
        measured_voltage *= 1.02f;

        // Convert to actual battery voltage using voltage divider ratio
        // Divider: VBAT - R1 - measurement - R2 - GND
        float battery_voltage = measured_voltage / VOLTAGE_DIVIDER_RATIO;

        // Map battery voltage range to 0.0-1.0
        // 4.2V = full (1.0), 2.8V = empty (0.0)
        float level = mapf(battery_voltage, 2.8f, 4.15f, 0.0f, 1.0f);

        // Clamp to valid range
        return std::clamp(level, 0.0f, 1.0f);
    }

    void DeInit()
    {
        // Deactivate ADCs
        battery_adc_.DeInit();
        vrefint_adc_.DeInit();

        // Prevents crashes (IDK why exactly, probably ADC issues)
        HAL_Delay(1);
    }

private:
    // Voltage divider resistors (in ohms)
    static constexpr float R1 = 100000.0f; // 100kΩ (high side)
    static constexpr float R2 = 220000.0f; // 220kΩ (low side to ground)

    // Voltage divider ratio (measurement point voltage / battery voltage)
    static constexpr float VOLTAGE_DIVIDER_RATIO = R2 / (R1 + R2);

    // Internal voltage reference specifications for STM32L0
    static constexpr float VREFINT_VOLTAGE = 1.224f; // Typical VREFINT voltage in volts

    float GetActualVref()
    {
        // Read the internal voltage reference
        auto vrefint_result = vrefint_adc_.ReadSingleShot();
        if (!vrefint_result.has_value())
        {
            // Return default VREF value if ADC read fails
            return 3.3f;
        }
        uint32_t vrefint_adc_value = vrefint_result.value();

        // Calculate actual ADC reference voltage
        // Formula: VREF_ACTUAL = (VREFINT_VOLTAGE * ADC_RESOLUTION) / VREFINT_ADC_VALUE
        float actual_vref = (VREFINT_VOLTAGE * static_cast<float>(AdcInput::kResolution)) / static_cast<float>(vrefint_adc_value);
        return actual_vref;
    }

    AdcInput battery_adc_;
    AdcInput vrefint_adc_;
};
