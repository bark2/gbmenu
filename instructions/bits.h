#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

namespace bits {

template <u8 Reg, bool PrefixCb = true>
static inline u8
rl()
{
    static_assert(PrefixCb | (!PrefixCb && (Reg == A)), "");
    u8 cc = Reg == HL ? 4 : 2;

    u8& reg = get_reg8(Reg);
    u8  c   = (cpu.get_flag(Cpu::Flag::C) >> Cpu::Flag::C);
    u8  res = (reg << 1) | c;

    if constexpr (!PrefixCb)
        cpu.set_z_flag(false);
    else
        cpu.set_z_flag(!res);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(0x80 & reg);
    reg = res;

    return cc;
}

template <u8 Reg, bool PrefixCb = true>
static inline u8
rr()
{
    static_assert(PrefixCb | (!PrefixCb && (Reg == A)), "");
    u8 cc = Reg == HL ? 4 : 2;

    u8& reg = get_reg8(Reg);
    u8  c   = (cpu.get_flag(Cpu::Flag::C) >> Cpu::Flag::C);
    u8  res = (reg >> 1) | (c << 7);

    if constexpr (!PrefixCb)
        cpu.set_z_flag(false);
    else
        cpu.set_z_flag(!res);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(1 & reg);
    reg = res;

    return cc;
}

template <u8 Reg, bool PrefixCb = true>
static inline u8
rrc()
{
    static_assert(PrefixCb | (!PrefixCb && (Reg == A)), "");
    u8 cc = Reg == HL ? 4 : 2;

    u8& reg = get_reg8(Reg);
    u8  res = (reg >> 1) | (reg << 7);

    if constexpr (!PrefixCb)
        cpu.set_z_flag(false);
    else
        cpu.set_z_flag(!res);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(reg & 1);
    reg = res;

    return cc;
}

template <u8 Reg>
static inline u8
srl()
{
    u8  cc  = (Reg == HL ? 4 : 2);
    u8& reg = get_reg8(Reg);
    u8  res = reg >> 1;

    cpu.set_z_flag(!res);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(reg & 1);

    reg = res;

    return cc;
}

template <u8 Reg>
static inline u8
sra()
{
    u8  cc  = (Reg == HL ? 4 : 2);
    u8& reg = get_reg8(Reg);
    u8  res = (reg & 0x80) | (reg >> 1);

    cpu.set_z_flag(!res);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(reg & 1);

    reg = res;

    return cc;
}

template <u8 Reg>
static inline u8
swap()
{
    u8  cc  = (Reg == HL ? 4 : 2);
    u8& reg = get_reg8(Reg);

    cpu.set_z_flag(!reg);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(false);
    reg = (reg << 4) | (reg >> 4);

    return cc;
}

template <u8 Reg, bool PrefixCb = true>
static inline u8
rlc()
{
    static_assert(PrefixCb | (!PrefixCb && (Reg == A)), "");
    u8    cc  = (Reg == HL ? 4 : 2);
    auto& reg = get_reg8(Reg);
    u8    c   = reg >> 7;

    if constexpr (!PrefixCb)
        cpu.set_z_flag(false);
    else
        cpu.set_z_flag(!reg);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(c);
    reg = (reg << 1) | c;

    return cc;
}

template <u8 Reg>
static inline u8
sla()
{
    u8    cc  = (Reg == HL ? 4 : 2);
    auto& reg = get_reg8(Reg);

    auto res = (reg << 1) & 0xff;
    cpu.set_z_flag(!res);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(reg & 0x80);
    reg = res;

    return cc;
}

template <u8 Bit, u8 Reg>
static inline u8
bit()
{
    u8    cc  = (Reg == HL ? 3 : 2);
    auto& reg = get_reg8(Reg);

    cpu.set_z_flag((~(reg & (1 << Bit))) & (1 << Bit));
    cpu.set_n_flag(false);
    cpu.set_h_flag(true);

    return cc;
}

template <u8 Bit, u8 Reg>
static inline u8
res()
{
    u8    cc  = (Reg == HL ? 3 : 2);
    auto& reg = get_reg8(Reg);

    set_bit(reg, Bit, 0);

    return cc;
}

template <u8 Bit, u8 Reg>
static inline u8
set()
{
    u8    cc  = (Reg == HL ? 3 : 2);
    auto& reg = get_reg8(Reg);

    set_bit(reg, Bit, 1);

    return cc;
}

};
