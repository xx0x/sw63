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
#include <cstdint>

/**
 * @brief Implements hidden configuration and diagnostics layer.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-07
 */
class LayerSecret : public Layer
{
public:
    /**
     * @brief Handles events in secret layer.
     * @param event Event to process.
     */
    void OnEvent(Event event) override;

    /**
     * @brief Updates secret layer state.
     */
    void Update() override;

    /**
     * @brief Reports whether secret layer allows sleep.
     * @return Always false.
     */
    bool SleepAllowed() override { return false; }

private:
    /**
     * @brief Lists selectable items in the secret layer menu.
     */
    enum class Settings
    {
        SPEED,
        VISUAL_STYLE,
        BATTERY_LEVEL,
        EXIT,
        COUNT
    };

    Settings current_setting_ = Settings::SPEED;
    bool setting_opened_ = false;
    float battery_level_ = 0.f;

    /**
     * @brief Executes current short-press action.
     */
    void ShortPressAction();
};