#ifndef SW63_ANIMATION_PROCESSOR_H
#define SW63_ANIMATION_PROCESSOR_H

#include "animations/Animation.h"
#include "animations/AnimationIntro.h"
#include "animations/AnimationExact.h"
#include "animations/AnimationTo.h"
#include "animations/AnimationPast.h"
#include "types.h"

namespace SW63
{
    class AnimationProcessor
    {
    public:
        void Init(Hardware *hw);
        void SetAnimation(AnimationType type);
        void SetAnimation(AnimationType type, uint32_t hours, uint32_t minutes, ClockFace face, bool pm);
        uint32_t Process();
        void Reset();
        AnimationType GetAnimationType();
        bool PauseBetweenFrames()
        {
            return animation_type_ != AT_INTRO;
        }

    private:
        AnimationType animation_type_;
        Animation *animations_[AT_COUNT] = {
            new AnimationIntro(),
            new AnimationExact(),
            new AnimationTo(),
            new AnimationPast()};
    };

}

#endif