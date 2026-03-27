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

#include "Layer.hpp"
#include "app/AnimationRunner.hpp"

/**
 * @brief Implements the default time display layer.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class LayerNormal : public Layer
{
public:
    /**
     * @brief Handles events in normal layer.
     * @param event Event to process.
     */
    void OnEvent(Event event) override;

    /**
     * @brief Updates normal layer state.
     */
    void Update() override;

    /**
     * @brief Reports whether normal layer allows sleep.
     * @return True when sleep is allowed.
     */
    bool SleepAllowed() override;

private:
    /**
     * @brief Starts intro animation sequence.
     */
    void IntroAnimation();

    /**
     * @brief Displays current time animation.
     */
    void DisplayTime();

    bool sleep_allowed_ = false;

    AnimationRunner animation_runner_;
};