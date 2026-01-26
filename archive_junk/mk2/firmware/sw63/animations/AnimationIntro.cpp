#include <Arduino.h>
#include "../Animation.h"

class AnimationIntro : public Animation
{

public:
    void setup()
    {
        currentStep = 0;
    }

    int drawRegular()
    {
        if (currentStep < 12)
        {
            watchFace.updateLeds(currentStep % 2 == 0, currentStep % 5 == 0, currentStep % 3 == 0, currentStep % 4 == 0, currentStep + 1, 0, false);
        }
        else if (currentStep < 16)
        {
            watchFace.updateLeds(false, false, false, false, 0, (currentStep - 11), currentStep % 2 == 0);
        }

        if (currentStep < 16)
        {
            currentStep++;
            return 120;
        }

        if (currentStep < 26)
        {
            if (currentStep % 2 == 0)
            {

                watchFace.updateLeds(false, false, false, false, language, false, false);
            }
            else
            {
                watchFace.clearLeds();
            }
            currentStep++;
            return 100;
        }

        return 0;
    }

};
