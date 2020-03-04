#pragma once

#include "../cpu.h"
#include "../mem.h"

template <u8 flag>
constexpr bool
read_flag()
{
    bool cond {};
    static_assert(flag >= 0b00 && flag <= 0b11);
    if constexpr (flag == 0b00) // nz
        cond = !cpu.z_flag();
    else if constexpr (flag == 0b01) // z
        cond = cpu.z_flag();
    else if constexpr (flag == 0b10) // nc
        cond = !cpu.c_flag();
    else if constexpr (flag == 0b11) // c
        cond = cpu.c_flag();

    return cond;
}

enum { B, C, D, E, H, L, A, BC, DE, HL, SP, AF, IO_C, IO_N, NN, REG8_NONE };
enum { FLAG_NZ, FLAG_Z, FLAG_NC, FLAG_C };

constexpr u8&
get_reg8(u8 src)
{
    switch (src) {
    case B: return cpu.b;
    case C: return cpu.c;
    case D: return cpu.d;
    case E: return cpu.e;
    case H: return cpu.h;
    case L: return cpu.l;
    case HL: return mem.buf[cpu.hl];
    case A: return cpu.a;
    default: assert(0);
    }
}

constexpr u16&
get_reg16(u8 src)
{
    switch (src) {
    case BC: return cpu.bc;
    case DE: return cpu.de;
    case HL: return cpu.hl;
    case SP: return cpu.sp;
    case AF: return cpu.af;
    default: assert(0);
    }
}
