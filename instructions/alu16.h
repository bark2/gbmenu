#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"
namespace alu16 {

template <u8 src>
static inline u8
add_hl()
{
    auto lhs = get_reg16(src);
    u32  res = cpu.hl + lhs;
    cpu.set_n_flag(false);
    cpu.set_h_flag(((cpu.hl & 0xfff) + (lhs & 0xfff)) > 0xfff);
    cpu.set_c_flag(res > 0xffff);
    cpu.hl = 0xffff & res;
    return 2;
}

static inline u8
add_sp()
{
    s8  imm = (s8)mem.read_byte(cpu.pc++);
    u32 res = cpu.sp + imm;
    cpu.set_z_flag(false);
    cpu.set_n_flag(false);
    cpu.set_h_flag((cpu.sp & 0xfff) + imm > 0xfff);
    cpu.set_c_flag(res > 0xffff);
    cpu.sp = 0xffff & res;
    return 4;
}

template <u8 reg>
static inline u8
inc()
{
    u32 res        = get_reg16(reg) + 1;
    get_reg16(reg) = 0xffff & res;
    return 2;
}

template <u8 reg>
static inline u8
dec()
{
    u32 res        = get_reg16(reg) - 1;
    get_reg16(reg) = 0xffff & res;
    return 2;
}

} // namespace alu16
