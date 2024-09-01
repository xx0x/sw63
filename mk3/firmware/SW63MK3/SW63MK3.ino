
#include "src/Hardware.h"

SW63::Hardware hw;

void setup()
{
    hw.Init();
}

uint32_t digit;

void loop()
{
    bool button_state = hw.GetButtonState();
    if (button_state)
    {
    }
    else
    {
    }
    hw.AutoBrightness();

    hw.SetLeds(0, 0, 1, 1, digit, SW63::Hardware::CF_DOWN, 1);
    hw.TransferLeds();
    delay(500);

    digit++;
    if (digit > 12)
    {
        digit = 0;
    }
}
