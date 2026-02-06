#pragma once

#include "dev/System.hpp"
#include "utils/EnumTools.hpp"
#include <cstdint>

class Button
{
public:
    enum class Event
    {
        SHORT_PRESS,
        MEDIUM_PRESS,
        LONG_PRESS,
        MULTI_PRESS,
        COUNT
    };

    void Update();

    bool Happened(Event event);
    bool PreventSleep();

private:
    enum class State
    {
        IDLE,
        PRESSED,
        RELEASED,
        WAITING_FOR_MULTI
    };

    static constexpr uint32_t kDebounceTimeMs = 20;           // 20ms debounce
    static constexpr uint32_t kShortPressMaxMs = 2000;        // Max duration for short press
    static constexpr uint32_t kMediumPressThresholdMs = 2000; // Min duration for medium press
    static constexpr uint32_t kLongPressThresholdMs = 5000;   // Min duration for long press
    static constexpr uint32_t kMultiPressWindowMs = 500;      // Max time between multi presses
    static constexpr uint8_t kMultiPressCount = 4;            // Number of presses for multi

    State state_ = State::IDLE;
    bool previous_raw_state_ = false;
    bool now_pressed_ = false;
    bool prev_pressed_ = false;
    uint32_t state_change_time_ = 0;
    uint32_t press_start_time_ = 0;
    uint32_t release_time_ = 0;
    uint32_t first_press_time_ = 0;

    EnumArray<Event, bool> events_{};

    bool medium_triggered_ = false;
    bool long_triggered_ = false;

    uint8_t press_count_ = 0;

    void ResetEvents();
    void ResetState();
};