#pragma once

#include "app/LocaleConfig.hpp"
#include <cstdint>

class Animation
{
public:
    // Returns delay in milliseconds until next frame
    virtual uint32_t ProcessNextFrame() = 0;

    virtual void Reset()
    {
        current_step_ = 0;
        finished_ = false;
    }

    virtual void SetTime(const LocaleConfig::TimeParameters &time_params)
    {
        time_params_ = time_params;
    }

    virtual bool IsFinished() const
    {
        return finished_;
    }

    virtual bool ShouldPauseBetweenFrames() const
    {
        return false;
    }

protected:
    LocaleConfig::TimeParameters time_params_;
    uint32_t current_step_ = 0;
    bool finished_ = false;
};