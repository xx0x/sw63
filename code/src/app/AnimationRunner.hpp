/*
MIT License

Copyright (c) 2026 Vaclav Mach (xx0x)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Locale.hpp"
#include "animations/Animation.hpp"
#include "utils/EnumTools.hpp"
#include <memory>

/**
 * @brief Runs and coordinates clock animations.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class AnimationRunner
{
public:
    /**
     * @brief Animations available in the runner.
     */
    enum class AnimationType
    {
        INTRO,
        TIME,
        CHARGE,
        COUNT
    };

    /**
     * @brief Constructs the animation runner.
     */
    AnimationRunner();

    /**
     * @brief Runs an animation.
     * @param type Animation type to run.
     */
    void SetAnimation(AnimationType type);

    /**
     * @brief Runs an animation with processed time parameters.
     * @param type Animation type to run.
     * @param time_params Processed time parameters for the animation.
     */
    void SetAnimation(AnimationType type, const Locale::TimeParameters &time_params);

    /**
     * @brief Returns the currently selected animation type.
     * @return Active animation type.
     */
    AnimationType GetAnimationType() const
    {
        return current_type_;
    }

    /**
     * @brief Advances the current animation by one step.
     * @return True while animation is still running, false when finished.
     */
    bool Update();

    /**
     * @brief Cancels the current animation.
     */
    void Cancel()
    {
        prev_type_ = current_type_;
        current_type_ = AnimationType::COUNT;

    }

    /**
     * @brief Returns the previous animation type.
     * @return Previously active animation type.
     */
    AnimationType GetPreviousAnimationType() const
    {
        return prev_type_;
    }

private:
    AnimationType current_type_ = AnimationType::COUNT;
    AnimationType prev_type_ = AnimationType::COUNT;
    EnumArray<AnimationType, std::unique_ptr<Animation>> animations_;

    // Animation timing state
    uint32_t animation_delay_remaining_ = 0;
    uint32_t last_animation_update_ = 0;

    /**
     * @brief Returns a mutable pointer to the current animation object.
     * @return Reference to the current animation pointer.
     */
    std::unique_ptr<Animation> &GetCurrentAnimation();

    /**
     * @brief Returns an immutable pointer to the current animation object.
     * @return Const reference to the current animation pointer.
     */
    const std::unique_ptr<Animation> &GetCurrentAnimation() const;

    /**
     * @brief Processes the next animation frame.
     * @return Delay in milliseconds before the next frame, or 0 if finished.
     */
    uint32_t ProcessNextFrame();

    /**
     * @brief Checks whether the current animation has finished.
     * @return True if animation is finished.
     */
    bool IsFinished() const;

    /**
     * @brief Checks whether frame-to-frame pause should be applied.
     * @return True when inter-frame pause is required.
     */
    bool ShouldPauseBetweenFrames() const;
};