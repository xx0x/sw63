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

#include "Animation.hpp"
#include "ClockFrame.hpp"

/**
 * @brief Renders time animation using generated frame sequence.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class AnimationTime : public Animation
{
public:
    /**
     * @brief Constructs a time animation instance.
     */
    AnimationTime() = default;

    /**
     * @brief Processes next frame of time animation.
     * @return Delay in milliseconds before the next frame.
     */
    uint32_t ProcessNextFrame() override;

    /**
     * @brief Resets sequence state for a new playback.
     */
    void Reset() override;

    /**
     * @brief Enables pause between time animation frames.
     * @return True because this animation pauses between frames.
     */
    bool ShouldPauseBetweenFrames() const override
    {
        return true;
    }

private:
    FrameSequence sequence_;

    /**
     * @brief Processes one frame element from the sequence.
     * @param frame Frame element to apply.
     * @return Delay in milliseconds before processing the next element.
     */
    uint32_t ProcessFrameElement(ClockFrame frame);
};