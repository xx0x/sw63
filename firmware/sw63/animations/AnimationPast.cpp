#include <Arduino.h>
#include "../Animation.h"

class AnimationPast : public Animation
{

public:
    void setup()
    {
        currentStep = 0;
        sign = 1;
    }

    int drawRegular()
    {
        int delayFor = 0;

        if (language == HUNGARIAN)
        {
            switch (currentStep)
            {
            case 0:
                watchFace.updateLeds(false, true, false, false, 0, 0, false); // a
                delayFor = HOLD;
                break;
            case 1:
                watchFace.updateLeds(false, false, false, false, _minutes, 0, false); // osm
                delayFor = HOLD_DIGITS;
                break;
            case 2:
                watchFace.updateLeds(false, false, false, true, 0, 0, false); // minut
                delayFor = HOLD;
                break;
            case 3:
                watchFace.updateLeds(false, false, false, false, 0, _face, false); // pul
                delayFor = HOLD;
                break;
            case 4:
                watchFace.updateLeds(false, false, false, false, _hours, 0, false); // ctvrte
                delayFor = HOLD_DIGITS;
                break;
            case 5:
                watchFace.updateLeds(false, false, true, false, 0, 0, false); // hodiny
                delayFor = HOLD;
                break;

            case 6:
                if (_pm)
                {
                    watchFace.updateLeds(false, false, false, false, 0, 0, true); // PM
                    delayFor = HOLD;
                }
                break;
            }
        }
        else
        {
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
                watchFace.updateLeds(false, true, false, false, 0, 0, false); // a
                delayFor = HOLD;
                break;
            case 4:
                watchFace.updateLeds(false, false, false, false, _minutes, 0, false); // osm
                delayFor = HOLD_DIGITS;
                break;
            case 5:
                watchFace.updateLeds(false, false, false, true, 0, 0, false); // minut
                delayFor = HOLD;
                break;
            case 6:
                if (_pm)
                {
                    watchFace.updateLeds(false, false, false, false, 0, 0, true); // PM
                    delayFor = HOLD;
                }
                break;
            }
        }
        currentStep++;
        return delayFor;
    }

};
