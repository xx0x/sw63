#include "Arduino.h"
#include "../Animation.h"


class AnimationExact : public Animation
{

public:
    void setup()
    {
        currentStep = 0;
    }

    int draw()
    {
        int delayFor = 0;

        switch (currentStep)
        {
        case 0:
            watchFace.updateLeds(false, false, false, false, 0, _face, false); // pul
            delayFor = HOLD;
            break;
        case 1:
            watchFace.updateLeds(false, false, false, false, _hours, 0, false); // ctvrte
            delayFor = HOLD_DIGITS;
            break;
        case 2:
            watchFace.updateLeds(false, false, true, false, 0, 0, false); // hodiny
            delayFor = HOLD;
            break;
        case 3:
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
    byte currentStep;
};
