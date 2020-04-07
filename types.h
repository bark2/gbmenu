#pragma once

#include <array>
#include <cstdint>
#include <string>

using u8 = uint8_t;
using s8 = int8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using std::array;
using std::string;

struct Breakpoint {
    char command;
    char reg[3];
    u32 val;
    u16 line;
    bool reached = false;
};
extern bool print_;

#include <chrono>
constexpr u32 cpu_freq = 4194304;
using Cycle_Duration = std::chrono::duration<int, std::ratio<1, cpu_freq>>;
using Time_Point = std::chrono::time_point<std::chrono::high_resolution_clock, Cycle_Duration>;

enum class Screen_Mode {
    H_BLANK = 0,
    V_BLANK = 1,
    OAM = 2,
    TRANSFARING = 3,
};

