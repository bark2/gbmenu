#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace alu16 {

constexpr u16&
get_reg16(Cpu& cpu, Memory& mem, u8 src)
{
    switch (src) {
    case 0b000: return cpu.bc; break;
    case 0b001: return cpu.de; break;
    case 0b010: return cpu.hl; break;
    case 0b011: return cpu.sp; break;
    default: assert(0);
    };
}

template <u8 src_reg>
static inline u8
add_hl(Cpu& cpu, Memory& mem)
{
    u16 src = get_reg16(cpu, mem, src_reg);
    u32 res = cpu.hl + src;
    u8  cc  = 2;

    cpu.set_n_flag(false);
    cpu.set_h_flag((lower(cpu.hl) + lower(src)) > 0x100);
    cpu.set_c_flag(res > 0xffff);
    cpu.hl = static_cast<u16>(res);

    return cc;
}

static inline u8
add_sp(Cpu& cpu, Memory& mem)
{
    s8  imm = mem.buf[cpu.pc++];
    u32 res = cpu.sp + imm;
    u8  cc  = 4;

    cpu.set_z_flag(false);
    cpu.set_n_flag(false);
    cpu.set_h_flag(lower(cpu.sp) + imm > 0x100);
    cpu.set_c_flag(res > 0xffff);
    cpu.sp = static_cast<u16>(res);

    return cc;
}

template <u8 reg>
static inline u8
inc(Cpu& cpu, Memory& mem)
{
    u16& dst = get_reg16(cpu, mem, reg);
    u32  res = dst + 1;
    u8   cc  = 2;

    dst = static_cast<u16>(res);

    return cc;
}

template <u8 reg>
static inline u8
dec(Cpu& cpu, Memory& mem)
{
    u16& dst = get_reg16(cpu, mem, reg);
    u32  res = dst - 1;
    u8   cc  = 2;

    dst = static_cast<u16>(res);

    return cc;
}
} // namespace alu16
