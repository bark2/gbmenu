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

enum { B, C, D, E, H, L, A, BC, DE, HL, SP, AF, IO_C, IO_N, NN };
enum { FLAG_NZ, FLAG_Z, FLAG_NC, FLAG_C };

constexpr u8&
get_reg8(u8 src)
{
    switch (src) {
    case B: return cpu.b; break;
    case C: return cpu.c; break;
    case D: return cpu.d; break;
    case E: return cpu.e; break;
    case H: return cpu.h; break;
    case L: return cpu.l; break;
    case A: return cpu.a; break;
    default: assert(0);
    }
}

constexpr u16
get_reg16(u8 src)
{
    switch (src) {
    case BC: return cpu.bc;
    case DE: return cpu.de;
    case HL: return cpu.hl;
    case SP: return cpu.sp;
    case AF: return cpu.af;

    case IO_C: return 0xff00 + cpu.c;
    case IO_N: return 0xff00 + cpu.data_bus;
    default: assert(0);
    }
}

u8
fetch_imm8()
{
    cpu.data_bus = mem.read_byte(cpu.pc++);
    return 1;
}

u8
fetch_imm16()
{
    cpu.addr_bus = mem.read_word(cpu.pc++);
    return 2;
}

template <u8 src>
u8
load_addr()
{
    cpu.addr_bus = get_reg16(src);
    return 0;
}

u8
read_addr()
{
    cpu.data_bus = mem.read_byte(cpu.addr_bus);
    return 1;
}

u8
store_addr()
{
    mem.write_byte(cpu.addr_bus, cpu.data_bus);
    return 1;
}

template <u8 dst>
u8
store_reg()
{
    get_reg8(dst) = cpu.data_bus;
    return 0;
}

template <u8 src>
u8
load_reg()
{
    cpu.data_bus = get_reg8(src);
    return 0;
}
