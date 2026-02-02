#include "Button.hpp"
#include "dev/System.hpp"

void Button::Update()
{
    now_pressed_ = System::GetRawButtonState();
    uint32_t current_time = HAL_GetTick();

    // Reset event flags at the beginning of each update
    ResetEvents();

    // State transitions based on debounced state
    if (!prev_pressed_ && now_pressed_)
    {
        // Button just pressed (debounced)
        state_ = State::PRESSED;
        press_start_time_ = current_time;
        medium_triggered_ = false;
        long_triggered_ = false;

        // Fire short press immediately on press
        events_[Event::SHORT_PRESS] = true;
    }
    else if (prev_pressed_ && !now_pressed_)
    {
        // Button just released (debounced)
        uint32_t press_duration = current_time - press_start_time_;
        release_time_ = current_time;

        if (state_ == State::PRESSED)
        {
            if (press_duration < kShortPressMaxMs) // Less than 2 seconds
            {
                press_count_++;

                if (press_count_ == 1)
                {
                    // First press
                    first_press_time_ = release_time_;
                    state_ = State::WAITING_FOR_DOUBLE;
                }
                else if (press_count_ == kDoublePressCount)
                {
                    // Check if second press was within 500ms of first press
                    if ((release_time_ - first_press_time_) <= kDoublePressWindowMs)
                    {
                        events_[Event::DOUBLE_PRESS] = true;
                        state_ = State::WAITING_FOR_DOUBLE; // Continue waiting for multi-press
                    }
                    else
                    {
                        // Too slow for double press, reset for new sequence
                        press_count_ = 1;
                        first_press_time_ = release_time_;
                        state_ = State::WAITING_FOR_DOUBLE;
                    }
                }
                else if (press_count_ == kMultiPressCount)
                {
                    // Check if all presses were within the multipress window (5 * double press window)
                    if ((release_time_ - first_press_time_) <= (kDoublePressWindowMs * kMultiPressCount))
                    {
                        events_[Event::MULTI_PRESS] = true;
                        // Don't fire short press for multipress
                        // Don't reset state - continue accepting more presses but ignore them
                    }
                    else
                    {
                        ResetState();
                    }
                }
                else if (press_count_ > kMultiPressCount)
                {
                    // Ignore subsequent presses after multipress - no events fired
                    if ((release_time_ - first_press_time_) > (kDoublePressWindowMs * kMultiPressCount))
                    {
                        ResetState();
                    }
                }
            }
            else
            {
                // Long press was already handled during holding
                ResetState();
            }
        }
    }

    // Handle holding states (use debounced state)
    if (state_ == State::PRESSED && now_pressed_)
    {
        uint32_t press_duration = current_time - press_start_time_;

        if (press_duration >= kLongPressThresholdMs && !long_triggered_)
        {
            // Long press (5+ seconds)
            events_[Event::LONG_PRESS] = true;
            long_triggered_ = true;
        }
        else if (press_duration >= kMediumPressThresholdMs && !medium_triggered_)
        {
            // Medium press (2+ seconds)
            events_[Event::MEDIUM_PRESS] = true;
            medium_triggered_ = true;
        }
    }

    // Handle timeout for waiting states
    if (state_ == State::WAITING_FOR_DOUBLE)
    {
        if (press_count_ >= kDoublePressCount)
        {
            // Wait for more presses or timeout for multi-press
            if ((current_time - first_press_time_) > (kDoublePressWindowMs * kMultiPressCount))
            {
                ResetState();
            }
        }
        else if (press_count_ == 1)
        {
            // Wait for potential double press (no timeout needed since short press already fired)
            if ((current_time - release_time_) > kDoublePressWindowMs)
            {
                ResetState();
            }
        }
    }

    prev_pressed_ = now_pressed_;
}

bool Button::Happened(Event event)
{
    return events_[event];
}

bool Button::PreventSleep()
{
    return now_pressed_;
}

void Button::ResetEvents()
{
    events_.fill(false);
}

void Button::ResetState()
{
    state_ = State::IDLE;
    press_count_ = 0;
    medium_triggered_ = false;
    long_triggered_ = false;
}