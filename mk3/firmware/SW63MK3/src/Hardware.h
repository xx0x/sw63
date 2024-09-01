#ifndef SW63_HARDWARE_H
#define SW63_HARDWARE_H

namespace SW63
{
    class Hardware
    {
    public:
        enum Pins
        {
            PIN_BTN,
            PIN_UNUSED1,
            PIN_LIGHT_SENS,
            PIN_ACTIVE,
            PIN_VBUS_DTCT,
            PIN_LAT,
            PIN_DAT,
            PIN_CLK,
            PIN_SDA,
            PIN_SCL,
            PIN_CHARGING,
            PIN_UNUSED2,
            PIN_UNUSED3,
            PIN_UNUSED4,
            PIN_UNUSED5,
            PIN_UNUSED6,
            PIN_BRIGHTNESS,
        };

        enum BitPositions
        {
            BP_PM,
            BP_LEFT,
            BP_DOWN,
            BP_RIGHT,
            BP_UP,
            BP_UNUSED2,
            BP_UNUSED1,
            BP_UNUSED0,
            BP_12,
            BP_11,
            BP_10,
            BP_9,
            BP_HOURS,
            BP_MINUTES,
            BP_PLUS,
            BP_MINUS,
            BP_8,
            BP_7,
            BP_6,
            BP_5,
            BP_1,
            BP_2,
            BP_3,
            BP_4,
        };

        enum ClockFace
        {
            CF_NONE,
            CF_UP,
            CF_RIGHT,
            CF_DOWN,
            CF_LEFT
        };

        void Init();
        void SetLeds(bool minus, bool plus, bool hours, bool minutes, uint32_t digit, ClockFace face, bool pm);
        void ClearLeds();
        void TransferLeds();
        void SetActive(bool active);
        bool GetButtonState();
        void AutoBrightness();

    private:
        uint32_t led_buffer_ = 0;
        void SetBrightness(uint8_t brightness);
        uint16_t GetLightSensorValue();
    };

}

#endif