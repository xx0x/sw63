#include "Hardware.h"

namespace SW63
{
    void Hardware::Init(voidFuncPtr button_callback, voidFuncPtr charge_callback)
    {
        InitPins(true);
        SetActive(true);
        SetBrightness(128);

        // Set up button interrupt and sleep
        byte buttonInterruptPin = digitalPinToInterrupt(PIN_BTN);
        attachInterrupt(buttonInterruptPin, button_callback, FALLING);
        byte chargeInterruptPin = digitalPinToInterrupt(PIN_CHARGING);
        attachInterrupt(chargeInterruptPin, charge_callback, FALLING);

        // enable EIC clock
        GCLK->CLKCTRL.bit.CLKEN = 0; // disable GCLK module
        while (GCLK->STATUS.bit.SYNCBUSY)
            ;

        GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK6 | GCLK_CLKCTRL_ID(GCM_EIC)); // EIC clock switched on GCLK6
        while (GCLK->STATUS.bit.SYNCBUSY)
            ;

        GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(6)); // source for GCLK6 is OSCULP32K
        while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
            ;

        GCLK->GENCTRL.bit.RUNSTDBY = 1; // GCLK6 run standby
        while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
            ;

        // Enable wakeup capability on pin in case being used during sleep
        EIC->WAKEUP.reg |= (1 << buttonInterruptPin);
        EIC->WAKEUP.reg |= (1 << chargeInterruptPin);
    }

    void Hardware::InitPins(bool init_interrupts)
    {
        if (init_interrupts)
        {
            pinMode(PIN_BTN, INPUT_PULLUP);
            pinMode(PIN_CHARGING, INPUT_PULLUP);
        }
        pinMode(PIN_VBUS_DTCT, INPUT);
        pinMode(PIN_ACTIVE, OUTPUT);
        pinMode(PIN_LAT, OUTPUT);
        pinMode(PIN_DAT, OUTPUT);
        pinMode(PIN_CLK, OUTPUT);
        pinMode(PIN_BRIGHTNESS, OUTPUT);
        pinMode(PIN_LIGHT_SENS, INPUT);
    }

    void Hardware::SetLeds(bool minus,
                           bool plus,
                           bool hours,
                           bool minutes,
                           uint32_t digit,
                           ClockFace face,
                           bool pm)
    {
        bitWrite(led_buffer_, BP_MINUS, minus);
        bitWrite(led_buffer_, BP_PLUS, plus);
        bitWrite(led_buffer_, BP_HOURS, hours);
        bitWrite(led_buffer_, BP_MINUTES, minutes);
        bitWrite(led_buffer_, BP_1, (digit >= 1));
        bitWrite(led_buffer_, BP_2, (digit >= 2));
        bitWrite(led_buffer_, BP_3, (digit >= 3));
        bitWrite(led_buffer_, BP_4, (digit >= 4));
        bitWrite(led_buffer_, BP_5, (digit >= 5));
        bitWrite(led_buffer_, BP_6, (digit >= 6));
        bitWrite(led_buffer_, BP_7, (digit >= 7));
        bitWrite(led_buffer_, BP_8, (digit >= 8));
        bitWrite(led_buffer_, BP_9, (digit >= 9));
        bitWrite(led_buffer_, BP_10, (digit >= 10));
        bitWrite(led_buffer_, BP_11, (digit >= 11));
        bitWrite(led_buffer_, BP_12, (digit >= 12));
        bitWrite(led_buffer_, BP_UP, face == CF_UP);
        bitWrite(led_buffer_, BP_RIGHT, face == CF_RIGHT);
        bitWrite(led_buffer_, BP_DOWN, face == CF_DOWN);
        bitWrite(led_buffer_, BP_LEFT, face == CF_LEFT);
        bitWrite(led_buffer_, BP_PM, pm);
    }

    void Hardware::ClearLeds()
    {
        led_buffer_ = 0;
    }

    void Hardware::TransferLeds()
    {
        digitalWrite(PIN_LAT, 0);
        for (uint32_t i = 0; i < 24; i++)
        {
            digitalWrite(PIN_DAT, (led_buffer_ & (1 << i)));
            digitalWrite(PIN_CLK, 1);
            digitalWrite(PIN_CLK, 0);
        }
        digitalWrite(PIN_LAT, 1);
    }

    void Hardware::SetActive(bool active)
    {
        digitalWrite(PIN_ACTIVE, active);
    }

    void Hardware::SetBrightness(uint8_t brightness)
    {
        analogWrite(PIN_BRIGHTNESS, 255 - brightness);
    }

    bool Hardware::GetRawButtonState()
    {
        return !digitalRead(PIN_BTN);
    }

    uint16_t Hardware::GetLightSensorValue()
    {
        return analogRead(PIN_LIGHT_SENS);
    }

    void Hardware::AutoBrightness()
    {
        // Clear LEDs to get precise light sensor value
        ClearLeds();
        TransferLeds();
        delay(5);

        // Read light sensor value
        uint16_t light = GetLightSensorValue();

        // Add more brightness for the lowest light
        light += 2;
        uint8_t brightness = map(light, 0, 1023, 1, 255);
        SetBrightness(brightness);
    }

    void Hardware::Sleep()
    {
        // Set all pins (not BUTTON) to input to reduce power consumption
        pinMode(PIN_LAT, INPUT);
        pinMode(PIN_DAT, INPUT);
        pinMode(PIN_CLK, INPUT);
        pinMode(PIN_BRIGHTNESS, INPUT);
        pinMode(PIN_LIGHT_SENS, INPUT);
        pinMode(PIN_ACTIVE, INPUT);
        pinMode(PIN_VBUS_DTCT, INPUT);
        pinMode(PIN_SCL, INPUT);
        pinMode(PIN_SDA, INPUT);

        // Wait a bit
        delay(100);

        // Go to sleep
        USBDevice.detach();
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
        __DSB();
        __WFI();

        // Wake up
        USBDevice.attach();
        InitPins(false);
        SetActive(true);
    }

    bool Hardware::IsCharging()
    {
        return !digitalRead(PIN_CHARGING);
    }

    bool Hardware::IsChargerConnected()
    {
        return digitalRead(PIN_VBUS_DTCT);
    }
}