#ifndef Animation_h
#define Animation_h

#include "Arduino.h"

class Animation
{
public:
    virtual void setup();
    virtual int drawRegular();

    void setTime(byte hours, byte minutes, byte face, bool pm)
    {
        if (hours == 0)
            hours = 12;
        _hours = hours;
        _minutes = minutes;
        _face = face;
        _pm = pm;
    }

    int draw()
    {
        if (COMPRESSED)
        {
            return drawCompressed();
        }
        return drawRegular();
    }

    int drawCompressed()
    {
        int delayFor = 0;
        switch (currentStep)
        {
        case 0:
            watchFace.updateLeds(false, false, true, false, _hours, _face, _pm); // pul 4 hodiny vecer
            delayFor = HOLD_COMPRESSED;
            break;
        case 1:
            if (sign != 0)
            {
                watchFace.updateLeds(sign == -1, sign == 1, false, true, _minutes, 0, false); // za 8 minut
                delayFor = HOLD_COMPRESSED;
            }
            break;
        }
        currentStep++;
        return delayFor;
    }

protected:
    byte _hours;
    byte _minutes;
    byte _face;
    int8_t sign;
    bool _pm;
    byte currentStep;
};

#endif
