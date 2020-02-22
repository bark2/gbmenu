#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

/*
        Z:  Set to 1 when the result of an operation is 0; otherwise reset.
        N:  Set to 1 following execution of the substruction instruction, regardless of the result.
        H : Set to 1 when an operation results in carrying from or borrowing to bit 3.
        CY : Set to 1 when an operation results in carrying from or borrowing to bit 7.
*/

namespace alu8 {

static inline u8&
get_reg8(Cpu& cpu, Memory& mem, u8 src)
{
    switch (src) {
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

template <bool imm, u8 src_reg>
static inline u8
add(Cpu& cpu, Memory& mem)
{
    u8 src;
    if constexpr (imm)
        src = mem.read_byte(cpu.pc++);
    else
        src = get_reg8(cpu, mem, src_reg);

    u16 res = cpu.a + src;
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 2 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(((cpu.a & 0x0f) + (src & 0x0f)) > 0x10);
    cpu.set_c_flag(res > 0xff);
    cpu.a = lower(res);

    return cc;
}

template <bool imm, u8 src_reg>
static inline u8
adc(Cpu& cpu, Memory& mem)
{
    u8 src;
    if constexpr (imm)
        src = mem.read_byte(cpu.pc++);
    else
        src = get_reg8(cpu, mem, src_reg);

    u16 res = cpu.a + src + cpu.c;
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 2 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(((cpu.a & 0x0f) + (src & 0x0f) + cpu.c_flag()) > 0x10);
    cpu.set_c_flag(res > 0xff);
    cpu.a = lower(res);

    return cc;
}

template <bool imm, u8 src_reg>
static inline u8
sub(Cpu& cpu, Memory& mem)
{
    u8 src;
    if constexpr (imm)
        src = mem.read_byte(cpu.pc++);
    else
        src = get_reg8(cpu, mem, src_reg);

    u16 res = cpu.a + src + cpu.c;
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 2 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(true);
    cpu.set_h_flag((cpu.a & 0x0f) < (src & 0x0f)); // half borrow
    cpu.set_c_flag(cpu.a < src);                   // borrow
    cpu.a = lower(res);

    return cc;
}

template <bool imm, u8 src_reg>
static inline u8
sbc(Cpu& cpu, Memory& mem)
{
    u8 src;
    if constexpr (imm)
        src = mem.read_byte(cpu.pc++);
    else
        src = get_reg8(cpu, mem, src_reg);

    u16 res = cpu.a - src - cpu.c_flag();
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 2 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(true);
    cpu.set_h_flag((cpu.a & 0x0f) < (src & 0x0f)); // half borrow
    cpu.set_c_flag(cpu.a < src);                   // borrow
    cpu.a = lower(res);

    return cc;
}

template <bool imm, u8 src_reg>
static inline u8
andOp(Cpu& cpu, Memory& mem)
{
    u8 src;
    if constexpr (imm)
        src = mem.read_byte(cpu.pc++);
    else
        src = get_reg8(cpu, mem, src_reg);

    u16 res = cpu.a & src;
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 2 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(true);
    cpu.set_c_flag(false);
    cpu.a = lower(res);

    return cc;
}

template <bool imm, u8 src_reg>
static inline u8
orOp(Cpu& cpu, Memory& mem)
{
    u8 src;
    if constexpr (imm)
        src = mem.read_byte(cpu.pc++);
    else
        src = get_reg8(cpu, mem, src_reg);

    u16 res = cpu.a | src;
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 2 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(false);
    cpu.a = lower(res);

    return cc;
}

template <bool imm, u8 src_reg>
static inline u8
xorOp(Cpu& cpu, Memory& mem)
{
    u8 src;
    if constexpr (imm)
        src = mem.read_byte(cpu.pc++);
    else
        src = get_reg8(cpu, mem, src_reg);

    u16 res = cpu.a ^ src;
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 2 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(false);
    cpu.a = lower(res);

    return cc;
}

template <bool imm, u8 src_reg>
static inline u8
cp(Cpu& cpu, Memory& mem)
{
    u8 src;
    if constexpr (imm)
        src = mem.read_byte(cpu.pc++);
    else
        src = get_reg8(cpu, mem, src_reg);

    bool res = cpu.a == src;
    u8   cc  = (&src == &mem.buf[cpu.hl]) ? 2 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(true);
    cpu.set_h_flag((cpu.a & 0x08) == (src & 0x08));
    cpu.set_c_flag((cpu.a & 0x80) == (src & 0x80));
    cpu.a = lower(res);

    return cc;
}

template <bool imm, u8 src_reg>
static inline u8
inc(Cpu& cpu, Memory& mem)
{
    u8& src = (imm ? mem.buf[cpu.pc++] : get_reg8(cpu, mem, src_reg));
    u16 res = src + 1; 
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 3 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag((src & 0x0f) == 0x0f); // DONT CHANGE, THIS IS THE ONLY OPTION
    src = lower(res);

    return cc;
}

template <bool imm, u8 reg>
static inline u8
dec(Cpu& cpu, Memory& mem)
{
    u8& src = (imm ? mem.buf[cpu.pc++] : get_reg8(cpu, mem, reg));
    u16 res = src - 1;
    u8  cc  = (&src == &mem.buf[cpu.hl]) ? 3 : 1;

    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag((src & 0x0f) == 0x00); // DONT CHANGE, THIS IS THE ONLY OPTION
    src = (src == 0x00 ? 0xff : lower(res));

    return cc;
}

} // namespace alu8
