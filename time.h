#pragma once

#include "types.h"

#include "cpu.h"

#include <chrono>

constexpr u32 cpu_freq = 4 * 1048576;
using Cc_Duration      = std::chrono::duration<int, std::ratio<1, cpu_freq>>;
using Time_Point       = std::chrono::time_point<std::chrono::steady_clock, Cc_Duration>;

static inline auto
now()
{
    return std::chrono::time_point_cast<Cc_Duration>(std::chrono::steady_clock::now());
}

static inline u32
hz_to_cc(u32 freq)
{
    assert(cpu_freq > freq);
    return cpu_freq / freq;
}

auto
div(const Time_Point& now, const Time_Point& last, u32 freq)
{
    return std::pair { static_cast<u32>((now - last).count()) / hz_to_cc(freq),
                       static_cast<u32>((now - last).count()) % hz_to_cc(freq) };
}

template <typename Timer, typename F>
static inline __attribute__((always_inline)) void
run_with_freq(const Time_Point& now, Timer& timer, const u32 freq, F fun)
{
    if (!cpu.halt && (cpu.cc - timer.last_cc > hz_to_cc(freq))) {
        timer.last_cc   = cpu.cc;
        timer.last_time = now;
        fun();
    }
    else if (cpu.halt) {
        auto [times, reminder] = div(now, timer.last_time, freq);
        while (times-- > 0) fun();
        timer.last_time = now - cc_duration(reminder);
    }
}

template <typename Timer, typename F>
static inline __attribute__((always_inline)) void
run_with_freq(Timer& timer, const u32 freq, F fun)
{
    auto now =
        std::chrono::time_point_cast<Cc_Duration>(std::chrono::steady_clock::now());

    if (!cpu.halt && (cpu.cc - timer.last_cc > hz_to_cc(freq))) {
        timer.last_cc   = cpu.cc;
        timer.last_time = now;
        fun();
    }
    else if (cpu.halt) {
        auto [times, reminder] = div(now, timer.last_time, freq);
        while (times-- > 0) fun();
        timer.last_time = now - Cc_Duration(reminder);
    }
}
