#pragma once

#include <optional>
#include "stm32l0xx_hal.h"

/**
 * @brief Class to manage ADC input for single shot readings
 */
class AdcInput
{
public:
    static constexpr uint16_t kResolution = 4095; // 12-bit max value

    struct Config
    {
        GPIO_TypeDef *port;
        uint16_t pin;
        uint32_t channel;
    };

    /**
     * @brief Initializes ADC for single shot reading
     * @param config ADC configuration (port, pin, channel)
     * @return True if initialization was successful, false otherwise
     */
    bool Init(const Config &config)
    {
        config_ = config;

        if (!InitAdc())
        {
            return false;
        }

        // Configure GPIO pin as analog input (only for external channels)
        if (config_.port != nullptr)
        {
            GPIO_InitTypeDef GPIO_InitStruct = {};
            GPIO_InitStruct.Pin = config_.pin;
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(config_.port, &GPIO_InitStruct);
        }

        initialized_ = true;
        return true;
    }

    /**
     * @brief Deinitializes the ADC
     * @return True if deinitialization was successful, false otherwise
     */
    bool DeInit()
    {
        if (!initialized_)
        {
            return false;
        }

        // Deinitialize GPIO (only for external channels)
        if (config_.port != nullptr)
        {
            HAL_GPIO_DeInit(config_.port, config_.pin);
        }
        DeInitAdc();
        initialized_ = false;
        return true;
    }

    /**
     * @brief Performs a single shot ADC reading
     * @return ADC value (0 to kResolution) or std::nullopt if error
     */
    std::optional<uint16_t> ReadSingleShot()
    {
        if (!initialized_)
        {
            return std::nullopt;
        }

        // Configure ADC channel
        // STM32L0 keeps a sequence of enabled channels in CHSELR.
        // Ensure only the requested channel is selected to avoid reading a different rank.
        hadc_.Instance->CHSELR = 0; // Clear all previously selected channels
        ADC_ChannelConfTypeDef sConfig = {};
        sConfig.Channel = config_.channel;
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        if (HAL_ADC_ConfigChannel(&hadc_, &sConfig) != HAL_OK)
        {
            return std::nullopt;
        }

        // Start ADC conversion
        if (HAL_ADC_Start(&hadc_) != HAL_OK)
        {
            return std::nullopt;
        }

        // Wait for conversion completion (timeout: 10ms)
        if (HAL_ADC_PollForConversion(&hadc_, 10) != HAL_OK)
        {
            HAL_ADC_Stop(&hadc_);
            return std::nullopt;
        }

        // Get conversion result
        uint32_t result = HAL_ADC_GetValue(&hadc_);

        // Stop ADC
        HAL_ADC_Stop(&hadc_);

        return static_cast<uint16_t>(result);
    }

private:
    static bool InitAdc()
    {
        if (init_count_ > 0)
        {
            init_count_++;
            return true;
        }

        // Enable ADC clock
        __HAL_RCC_ADC1_CLK_ENABLE();

        // Configure ADC
        hadc_.Instance = ADC1;
        hadc_.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
        hadc_.Init.Resolution = ADC_RESOLUTION_12B;
        hadc_.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        hadc_.Init.ScanConvMode = 0x00000000U; // ADC_SCAN_DISABLE for STM32L0
        hadc_.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        hadc_.Init.LowPowerAutoWait = DISABLE;
        hadc_.Init.LowPowerAutoPowerOff = DISABLE;
        hadc_.Init.ContinuousConvMode = DISABLE;
        hadc_.Init.DiscontinuousConvMode = DISABLE;
        hadc_.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        hadc_.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        hadc_.Init.DMAContinuousRequests = DISABLE;
        hadc_.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
        hadc_.Init.LowPowerFrequencyMode = DISABLE;
        hadc_.Init.SamplingTime = ADC_SAMPLETIME_79CYCLES_5;
        hadc_.Init.OversamplingMode = DISABLE;

        if (HAL_ADC_Init(&hadc_) != HAL_OK)
        {
            return false;
        }

        // Calibrate ADC
        if (HAL_ADCEx_Calibration_Start(&hadc_, ADC_SINGLE_ENDED) != HAL_OK)
        {
            return false;
        }

        init_count_++;
        return true;
    }

    static void DeInitAdc()
    {
        init_count_--;
        if (init_count_ == 0)
        {
            HAL_ADC_DeInit(&hadc_);
        }
    }

    Config config_;
    bool initialized_ = false;

    static inline ADC_HandleTypeDef hadc_;
    static inline int init_count_;
};