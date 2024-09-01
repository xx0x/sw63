#include "Arduino.h"
#include "Hardware.h"

namespace SW63
{
    void Hardware::Init()
    {
        pinMode(PIN_BTN, INPUT_PULLUP);
        pinMode(PIN_ACTIVE, OUTPUT);
        pinMode(PIN_LAT, OUTPUT);
        pinMode(PIN_DAT, OUTPUT);
        pinMode(PIN_CLK, OUTPUT);
        pinMode(PIN_BRIGHTNESS, OUTPUT);
        pinMode(PIN_LIGHT_SENS, INPUT);

        SetActive(true);
        SetBrightness(128);
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
        for (size_t i = 0; i < 24; i++)
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

    bool Hardware::GetButtonState()
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
}