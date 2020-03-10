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
    auto rhs = get_reg16(src);
    u32  res = cpu.hl + rhs;
    cpu.set_n_flag(false);
    cpu.set_h_flag(((cpu.hl & 0xfff) + (rhs & 0xfff)) > 0xfff);
    cpu.set_c_flag(res > 0xffff);
    cpu.hl = 0xffff & res;
    return 2;
}

static inline u8
add_sp()
{
    // total := uint16(int32(val1) + int32(val2))
    // set(total)
    // tmpVal := val1 ^ uint16(val2) ^ total
    // gb.CPU.SetZ(false)
    // gb.CPU.SetN(false)
    // gb.CPU.SetH((tmpVal & 0x10) == 0x10)
    // gb.CPU.SetC((tmpVal & 0x100) == 0x100)

    s8   imm    = (s8)mem.read_byte(cpu.pc++);
    u32  res    = static_cast<u32>((int)cpu.sp + (int)imm);
    auto tmpVal = cpu.sp ^ ((s8)imm) ^ res;
    cpu.set_z_flag(false);
    cpu.set_n_flag(false);
    cpu.set_h_flag((tmpVal & 0x10) == 0x10);
    cpu.set_c_flag((tmpVal & 0x100) == 0x100);
    // cpu.set_h_flag(static_cast<u32>((cpu.sp & 0xfff) + imm) > 0xfff);
    // cpu.set_c_flag(res > 0xffff);
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
