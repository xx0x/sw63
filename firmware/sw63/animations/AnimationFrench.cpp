#include <Arduino.h>
#include "../Animation.h"

class AnimationFrench : public Animation
{

public:
    void setup()
    {
        currentStep = 0;
    }

    int draw()
    {
        int delayFor = 0;
        byte h = _hours;

        switch (currentStep)
        {
        case 0:
            if (_minutes > 40)
            {
                h = (h + 1) % 12;
            }
            if (h == 0)
            {
                h = 12;
            }
            watchFace.updateLeds(false, false, false, false, h, 0, false); // 8
            delayFor = HOLD_DIGITS;
            break;
        case 1:
            watchFace.updateLeds(false, false, true, false, 0, 0, false); // h
            delayFor = HOLD;
            break;
        case 2:
            delayFor = SKIP;
            if (_minutes > 10 && _minutes <= 40)
            {
                watchFace.updateLeds(false, true, false, false, 0, 0, false); // +
                delayFor = HOLD;
            }
            else if (_minutes > 40 && _minutes < 50)
            {
                watchFace.updateLeds(true, false, false, false, 0, 0, false); // -
                delayFor = HOLD;
            }

            if (delayFor != SKIP)
            {
                break;
            }
            currentStep++;
        case 3:
            delayFor = SKIP;
            if (_minutes > 10 && _minutes <= 25)
            {
                watchFace.updateLeds(false, false, false, false, 0, 2, false);
                delayFor = HOLD;
            }
            else if (_minutes > 25 && _minutes <= 40)
            {
                delayFor = HOLD;
                watchFace.updateLeds(false, false, false, false, 0, 3, false);
            }
            else if (_minutes > 40 && _minutes < 50)
            {
                delayFor = HOLD;
                watchFace.updateLeds(false, false, false, false, 0, 4, false);
            }

            if (delayFor != SKIP)
            {
                break;
            }
            currentStep++;
        case 4:
            delayFor = SKIP;
            if ((_minutes > 0 && _minutes <= 10) || (_minutes > 15 && _minutes <= 25) || (_minutes > 30 && _minutes <= 40) || (_minutes > 45 && _minutes < 50))
            {
                watchFace.updateLeds(false, true, false, false, 0, 0, false); // +
                delayFor = HOLD;
            }
            else if ((_minutes > 10 && _minutes < 15) || (_minutes > 25 && _minutes < 30) || (_minutes > 40 && _minutes < 44) || (_minutes < 60 && _minutes >= 50))
            {
                watchFace.updateLeds(true, false, false, false, 0, 0, false); // -
                delayFor = HOLD;
            }
            if (delayFor != SKIP)
            {
                break;
            }
            currentStep++;
        case 5:
            delayFor = HOLD_DIGITS;
            if (_minutes == 0)
            {
                delayFor = SKIP;
            }
            else if (_minutes <= 10)
            {
                watchFace.updateLeds(false, false, false, false, _minutes, 0, false);
            }
            else if (_minutes < 15)
            {
                watchFace.updateLeds(false, false, false, false, 15 - _minutes, 0, false);
            }
            else if (_minutes == 15)
            {
                delayFor = SKIP;
            }
            else if (_minutes <= 25)
            {
                watchFace.updateLeds(false, false, false, false, _minutes - 15, 0, false);
            }
            else if (_minutes < 30)
            {
                watchFace.updateLeds(false, false, false, false, 30 - _minutes, 0, false);
            }
            else if (_minutes == 30)
            {
                delayFor = SKIP;
            }
            else if (_minutes <= 40)
            {
                watchFace.updateLeds(false, false, false, false, _minutes - 30, 0, false);
            }
            else if (_minutes < 45)
            {
                watchFace.updateLeds(false, false, false, false, 45 - _minutes, 0, false);
            }
            else if (_minutes == 45)
            {
                delayFor = SKIP;
            }
            else if (_minutes < 50)
            {
                watchFace.updateLeds(false, false, false, false, _minutes - 45, 0, false);
            }
            else if (_minutes < 60)
            {
                watchFace.updateLeds(false, false, false, false, 60 - _minutes, 0, false);
            }

            if (delayFor != SKIP)
            {
                break;
            }
            currentStep++;
        case 6:

            if (_minutes % 15 != 0)
            {

                delayFor = HOLD;
                watchFace.updateLeds(false, false, false, true, 0, 0, false);
                break;
            }

            currentStep++;
        case 7:
            if (_pm)
            {
                watchFace.updateLeds(false, false, false, false, 0, 0, true); // PM
                delayFor = HOLD;
            }
            break;
        }
        currentStep++;
        return delayFor;
    }

private:
    bool showMinutes = true;
    byte currentStep;
};
