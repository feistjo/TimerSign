#pragma once

#include <stdint.h>

#include <functional>

class Timer
{
public:
    Timer(uint32_t set_time, std::function<uint32_t(void)> get_millis)
        : set_time_{set_time}, get_millis_{get_millis}, end_time_{get_millis() + set_time} {};

    void SetTime(uint32_t set_time) { set_time_ = set_time; }

    uint32_t GetTime() { return set_time_; }

    void Reset() { end_time_ = get_millis_() + set_time_; }

    uint32_t GetTimeRemaining() { return end_time_ - get_millis_(); }

private:
    uint32_t set_time_;
    std::function<uint32_t(void)> get_millis_;
    int32_t end_time_;
};