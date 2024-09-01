#ifndef SW63_APPLICATION_H
#define SW63_APPLICATION_H

#include "Arduino.h"
#include "Hardware.h"
#include "animations/Animation.h"
#include "animations/AnimationIntro.h"
#include "animations/AnimationExact.h"
#include "animations/AnimationTo.h"
#include "animations/AnimationPast.h"
#include "types.h"

namespace SW63
{
    class Application
    {
    public:
        void Init(voidFuncPtr button_callback);
        void Process();
        void ButtonCallback();

        void SetAnimation(AnimationType type);

    private:
        Hardware hw_;
        Animation *current_animation_;
        Animation *animations_[AT_COUNT] = {
            new AnimationIntro(),
            new AnimationExact(),
            new AnimationTo(),
            new AnimationPast()};
    };

}

#endif