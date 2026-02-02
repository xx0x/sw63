#pragma once

#include "dev/System.hpp"
#include <cstdint>

class Button
{
public:
    void Update();
    
    bool ShortPressed();
    bool MediumPressed();
    bool LongPressed();
    bool DoublePressed();
    bool MultiPressed();
    bool PreventSleep();

private:
    enum class State
    {
        IDLE,
        PRESSED,
        RELEASED,
        WAITING_FOR_DOUBLE
    };

    static constexpr uint32_t kDebounceTimeMs = 20;        // 20ms debounce
    static constexpr uint32_t kShortPressMaxMs = 2000;     // Max duration for short press
    static constexpr uint32_t kMediumPressThresholdMs = 2000; // Min duration for medium press
    static constexpr uint32_t kLongPressThresholdMs = 5000;   // Min duration for long press
    static constexpr uint32_t kDoublePressWindowMs = 500;     // Max time between double presses
    static constexpr uint8_t kDoublePressCount = 2;          // Number of presses for double
    static constexpr uint8_t kMultiPressCount = 4;           // Number of presses for multi
    
    State state_ = State::IDLE;
    bool previous_raw_state_ = false;
    bool now_pressed_ = false;
    bool prev_pressed_ = false;
    uint32_t state_change_time_ = 0;
    uint32_t press_start_time_ = 0;
    uint32_t release_time_ = 0;
    uint32_t first_press_time_ = 0;
    
    bool short_press_event_ = false;
    bool medium_press_event_ = false;
    bool long_press_event_ = false;
    bool double_press_event_ = false;
    bool multi_press_event_ = false;
    
    bool medium_triggered_ = false;
    bool long_triggered_ = false;
    
    uint8_t press_count_ = 0;
    
    void ResetEvents();
    void ResetState();
};