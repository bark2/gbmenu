#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

namespace bits {

template <u8 reg_num>
static inline u8
rr()
{
    u8 cc = 1;

    u8& reg   = get_reg8(reg_num);
    u8  carry = reg & 0x01;
    u8  flag  = cpu.c_flag();
    u8  res   = (reg >> 1) | (flag << 7);

    cpu.set_z_flag(res);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(carry);
    reg = res;

    return cc;
}

static inline u8
rra()
{
    u8 cc = 1;

    u8 carry = cpu.a & 0x01;
    u8 flag  = cpu.c_flag();
    cpu.set_z_flag(false);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(carry);
    cpu.a = (cpu.a >> 1) | (flag << 7);

    return cc;
}

template <u8 reg_num>
static inline u8
srl()
{
    u8  cc  = (reg_num == HL ? 2 : 4);
    u8& reg = get_reg8(reg_num);

    cpu.set_z_flag(reg >> 1);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(reg & 1);

    reg = reg >> 1;

    return cc;
}

template <u8 reg>
static inline u8
swap()
{
    u8 cc = (reg == HL ? 2 : 4);
    u8 v  = get_reg8(reg);

    cpu.set_z_flag(v == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(false);
    get_reg8(reg) = (v & 0x0f << 4) | (v & 0xf0 >> 4);

    return cc;
}

};
