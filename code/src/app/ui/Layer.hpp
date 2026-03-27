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

/**
 * @brief Defines the interface for application UI layers.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class Layer
{
public:
    /**
     * @brief Lists available UI layer types.
     */
    enum class Type
    {
        NORMAL,
        SETTINGS,
        SECRET,
        COUNT
    };

    /**
     * @brief Lists events delivered to layers.
     */
    enum class Event
    {
        INIT,
        ENTER,
        LEAVE,
        JUST_PRESSED,
        JUST_RELEASED,
        MEDIUM_PRESS,
        LONG_PRESS,
        MULTI_PRESS,
        COUNT
    };

    /**
     * @brief Handles a layer lifecycle or input event.
     * @param event Event to process.
     */
    virtual void OnEvent(Event event) = 0;

    /**
     * @brief Updates layer state.
     */
    virtual void Update() = 0;

    /**
     * @brief Reports whether sleep is currently allowed.
     * @return True if application may enter sleep mode.
     */
    virtual bool SleepAllowed() = 0;
};