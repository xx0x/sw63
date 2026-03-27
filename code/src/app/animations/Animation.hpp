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

#include "app/Locale.hpp"
#include <cstdint>

/**
 * @brief Defines the base interface for display animations.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class Animation
{
public:
    /**
     * @brief Processes next animation frame.
     * @return Delay in milliseconds before processing next frame.
     */
    virtual uint32_t ProcessNextFrame() = 0;

    /**
     * @brief Resets animation state to initial values.
     */
    virtual void Reset()
    {
        current_step_ = 0;
        finished_ = false;
    }

    /**
     * @brief Sets time parameters used by the animation.
     * @param time_params Processed time parameters.
     */
    virtual void SetTime(const Locale::TimeParameters &time_params)
    {
        time_params_ = time_params;
    }

    /**
     * @brief Reports whether animation is finished.
     * @return True when animation reached final step.
     */
    virtual bool IsFinished() const
    {
        return finished_;
    }

    /**
     * @brief Indicates whether pause should be inserted between frames.
     * @return True when caller should pause between frames.
     */
    virtual bool ShouldPauseBetweenFrames() const
    {
        return false;
    }

protected:
    Locale::TimeParameters time_params_;
    uint32_t current_step_ = 0;
    bool finished_ = false;
};