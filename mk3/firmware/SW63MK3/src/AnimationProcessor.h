#ifndef SW63_ANIMATION_PROCESSOR_H
#define SW63_ANIMATION_PROCESSOR_H

#include "animations/Animation.h"
#include "animations/AnimationIntro.h"
#include "animations/AnimationExact.h"
#include "animations/AnimationTo.h"
#include "animations/AnimationPast.h"
#include "animations/AnimationCharge.h"
#include "types.h"

namespace SW63
{
    class AnimationProcessor
    {
    public:
        void Init(Hardware *hw);
        void SetAnimation(Animation::Type type);
        void SetAnimation(Animation::Type type, uint32_t hours, uint32_t minutes, ClockFace face, bool pm);
        uint32_t Process();
        void Reset();
        bool IsFinished()
        {
            return animations_[animation_type_]->IsFinished();
        }
        bool PauseBetweenFrames()
        {
            return IsShowingTime();
        }
        bool IsShowingTime()
        {
            return animation_type_ == Animation::EXACT ||
                   animation_type_ == Animation::TO ||
                   animation_type_ == Animation::PAST;
        }

    private:
        Animation::Type animation_type_;
        Animation *animations_[Animation::COUNT] = {
            new AnimationIntro(),
            new AnimationExact(),
            new AnimationTo(),
            new AnimationPast(),
            new AnimationCharge()};
    };

}

#endif