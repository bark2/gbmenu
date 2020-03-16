#pragma once

#include "types.h"

#include "cpu.h"

#include <chrono>

auto
div(const std::chrono::time_point<std::chrono::steady_clock, cc_duration>& now,
    const std::chrono::time_point<std::chrono::steady_clock, cc_duration>& last,
    u32                                                                    freq)
{
    return std::pair { static_cast<u32>((now - last).count()) / hz_to_cc(freq),
                       static_cast<u32>((now - last).count()) % hz_to_cc(freq) };
}

template <typename Timer, typename F>
static inline void
run_with_freq(Timer& timer, u32 freq, F fun)
{
    auto now =
        std::chrono::time_point_cast<cc_duration>(std::chrono::steady_clock::now());
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
