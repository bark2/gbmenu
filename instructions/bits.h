#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace bits {

static inline u8&
get_reg8(Cpu& cpu, Memory& mem, u8 reg_num)
{
    switch (reg_num) {
    case 0b000: return cpu.b; break;
    case 0b001: return cpu.c; break;
    case 0b010: return cpu.d; break;
    case 0b011: return cpu.e; break;
    case 0b100: return cpu.h; break;
    case 0b101: return cpu.l; break;
    case 0b110: return mem.buf[cpu.hl]; break;
    case 0b111: return cpu.a; break;
    default: assert(0);
    }
}

template <u8 reg_num>
static inline u8
rr(Cpu& cpu, Memory& mem)
{
    u8 cc = 1;

    u8& reg   = get_reg8(cpu, mem, reg_num);
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
rra(Cpu& cpu, Memory& mem)
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
srl(Cpu& cpu, Memory& mem)
{
    u8 cc = (reg_num == 0b110 ? 2 : 4);
    u8& reg = get_reg8(cpu, mem, reg_num);

    cpu.set_z_flag(reg >> 1);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(reg & 1);

    reg = reg >> 1;

    return cc;
}

};
