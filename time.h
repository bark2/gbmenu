#pragma once

#include "types.h"

#include "cpu.h"

#include <cassert>
#include <chrono>

static inline auto
now()
{
    return std::chrono::time_point_cast<Cycle_Duration>(std::chrono::high_resolution_clock::now());
}

static inline u32
hz_to_cc(u32 freq)
{
    assert(cpu_freq > freq);
    return cpu_freq / freq;
}

struct Div_Result {
    u32 times;
    u32 reminder;
};

auto
div(u32 delta, u32 freq) -> Div_Result
{
    return { delta / hz_to_cc(freq), delta % hz_to_cc(freq) };
}

template <typename Timer, typename F>
static inline auto
run_with_freq(Timer& timer, const u32 freq, F fun) -> void
{
    Div_Result r = div(cpu.cycles - timer.last_cc, freq);
    while (r.times-- > 0) fun();
    timer.last_cc = cpu.cycles - r.reminder;
}
