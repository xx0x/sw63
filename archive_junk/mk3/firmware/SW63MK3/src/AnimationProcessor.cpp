#include "AnimationProcessor.h"

namespace SW63
{
    void AnimationProcessor::Init(Hardware *hw)
    {
        for (int i = 0; i < Animation::COUNT; i++)
        {
            animations_[i]->Init(hw);
        }
    }

    void AnimationProcessor::SetAnimation(Animation::Type type)
    {
        animation_type_ = type;
        animations_[animation_type_]->Reset();
    }

    void AnimationProcessor::SetAnimation(Animation::Type type, uint32_t hours, uint32_t minutes, ClockFace face, bool pm)
    {
        SetAnimation(type);
        animations_[animation_type_]->SetTime(hours, minutes, face, pm);
    }

    uint32_t AnimationProcessor::Process()
    {
        return animations_[animation_type_]->Process();
    }

    void AnimationProcessor::Reset()
    {
        animations_[animation_type_]->Reset();
    }

}