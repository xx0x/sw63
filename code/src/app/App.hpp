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
#include "app/Settings.hpp"
#include "dev/Battery.hpp"
#include "dev/Button.hpp"
#include "dev/Communication.hpp"
#include "dev/DS3231.hpp"
#include "dev/Display.hpp"
#include "dev/System.hpp"
#include "ui/Layer.hpp"
#include <cstdint>
#include <memory>

#ifdef SW63_BUILD_DATE
#define SW63_VERSION_BUILD_SUFFIX " (build " SW63_BUILD_DATE ")"
#else
#define SW63_VERSION_BUILD_SUFFIX ""
#endif

/**
 * @brief Coordinates application initialization and runtime loop.
 * @author Vaclav Mach (xx0x)
 * @date 2026-02-02
 */
class App
{
public:
    /**
     * @brief Initializes all application subsystems.
     */
    static void Init();

    /**
     * @brief Runs one iteration of the main application loop.
     */
    static void Loop();

    /**
     * @brief Firmware version string exposed by the app to the Web App (not available through the watch UI).
     */
    static constexpr const char *kVersion = "0.9.1" SW63_VERSION_BUILD_SUFFIX;

    /**
     * @brief Display controller.
     */
    inline static Display display;

    /**
     * @brief Settings.
     */
    inline static Settings settings;

    /**
     * @brief Language processing.
     */
    inline static Locale locale;

    /**
     * @brief Clock driver.
     */
    inline static DS3231 rtc = DS3231(&System::hi2c1);

    /**
     * @brief Battery measurement.
     */
    inline static Battery battery;

    /**
     * @brief Button event handler.
     */
    inline static Button button;

    /**
     * @brief USB communication handler.
     */
    inline static Communication com;

    /**
     * @brief Switches active UI layer, updates display brightness, and triggers layer events.
     * @param layer_type Target layer type.
     */
    static void ChangeLayer(Layer::Type layer_type)
    {
        layers_[current_layer_]->OnEvent(Layer::Event::LEAVE);
        current_layer_ = layer_type;

        // Short delay before entering new layer
        // Just a visual thing
        App::display.Clear();
        App::display.Update();
        System::Delay(20);

        // Update brightness on layer change
        display.TriggerAutoBrightness();

        layers_[current_layer_]->OnEvent(Layer::Event::ENTER);
    }

    /**
     * @brief Requests intro animation playback.
     */
    static void TriggerIntro()
    {
        trigger_intro_flag_ = true;
    }

private:
    /**
     * @brief Enters low-power sleep mode.
     */
    static void Sleep();

    static inline EnumArray<Layer::Type, std::unique_ptr<Layer>> layers_;
    static inline Layer::Type current_layer_ = Layer::Type::NORMAL;

    /**
     * @brief Stores timestamp of the latest user interaction. Used for timeout back to NORMAL layer.
     */
    static void UpdateLastInteractionTime()
    {
        last_interaction_time_ = System::Millis();
    }
    static inline uint32_t last_interaction_time_ = 0;

    static inline bool trigger_intro_flag_ = false;
};

#undef SW63_VERSION_BUILD_SUFFIX
