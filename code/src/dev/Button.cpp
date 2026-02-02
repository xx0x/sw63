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
        short_press_event_ = true;
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
                        double_press_event_ = true;
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
                        multi_press_event_ = true;
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
            long_press_event_ = true;
            long_triggered_ = true;
        }
        else if (press_duration >= kMediumPressThresholdMs && !medium_triggered_)
        {
            // Medium press (2+ seconds)
            medium_press_event_ = true;
            medium_triggered_ = true;
        }
    }

    // Handle timeout for waiting states
    if (state_ == State::WAITING_FOR_DOUBLE)
    {
        if (press_count_ >= kDoublePressCount && press_count_ < kMultiPressCount)
        {
            // Wait for more presses or timeout for multi-press
            if ((current_time - first_press_time_) > (kDoublePressWindowMs * kMultiPressCount))
            {
                ResetState();
            }
        }
        else if (press_count_ >= kMultiPressCount)
        {
            // For multipress counts, keep accepting presses within the window
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

bool Button::ShortPressed()
{
    return short_press_event_;
}

bool Button::MediumPressed()
{
    return medium_press_event_;
}

bool Button::LongPressed()
{
    return long_press_event_;
}

bool Button::DoublePressed()
{
    return double_press_event_;
}

bool Button::MultiPressed()
{
    return multi_press_event_;
}

bool Button::PreventSleep()
{
    return now_pressed_;
}

void Button::ResetEvents()
{
    short_press_event_ = false;
    medium_press_event_ = false;
    long_press_event_ = false;
    double_press_event_ = false;
    multi_press_event_ = false;
}

void Button::ResetState()
{
    state_ = State::IDLE;
    press_count_ = 0;
    medium_triggered_ = false;
    long_triggered_ = false;
}