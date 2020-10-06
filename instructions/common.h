#pragma once

#include "../cpu.h"
#include "../mem.h"

enum { FLAG_NZ, FLAG_Z, FLAG_NC, FLAG_C, FLAG_COUNT };

template <u8 flag>
constexpr bool
read_flag()
{
    bool cond {};
    static_assert(flag < FLAG_COUNT);
    if constexpr (flag == FLAG_NZ) // nz
        cond = !cpu.get_flag(Cpu::Flag::Z);
    else if constexpr (flag == FLAG_Z) // z
        cond = cpu.get_flag(Cpu::Flag::Z);
    else if constexpr (flag == FLAG_NC) // nc
        cond = !cpu.get_flag(Cpu::Flag::C);
    else if constexpr (flag == FLAG_C) // c
        cond = cpu.get_flag(Cpu::Flag::C);

    return cond;
}

enum { B, C, D, E, H, L, A, BC, DE, HL, SP, AF, IO_C, IO_N, NN, REG8_NONE };

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
    case HL:
        return mem.get_ref(cpu.hl); // constexpr?
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
