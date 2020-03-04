#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"
/*
        Z:  Set to 1 when the result of an operation is 0; otherwise reset.
        N:  Set to 1 following execution of the substruction instruction, regardless of
   the result. H : Set to 1 when an operation results in carrying from or borrowing to
   bit 3. CY : Set to 1 when an operation results in carrying from or borrowing to bit 7.
*/

namespace alu8 {

template <bool imm, u8 src>
static inline u8
add()
{
    u8 rhs;
    if constexpr (imm)
        rhs = mem.read_byte(cpu.pc++);
    else
        rhs = get_reg8(src);

    u16 res = cpu.a + rhs;
    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(((cpu.a & 0x0f) + (rhs & 0x0f)) > 0x10);
    cpu.set_c_flag(res > 0xff);
    cpu.a = res;
    return 1;
}

template <bool imm, u8 src>
static inline u8
adc()
{
    u8 rhs;
    if constexpr (imm)
        rhs = mem.read_byte(cpu.pc++);
    else
        rhs = get_reg8(src);

    u16 res = cpu.a + rhs + cpu.c_flag();
    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(((cpu.a & 0x0f) + (rhs & 0x0f) + cpu.c_flag()) > 0x10);
    cpu.set_c_flag(res > 0xff);
    cpu.a = res;
    return 1;
}

template <bool imm, u8 src>
static inline u8
sub()
{
    u8 rhs;
    if constexpr (imm)
        rhs = mem.read_byte(cpu.pc++);
    else
        rhs = get_reg8(src);

    u16 res = cpu.a - rhs;
    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(true);
    cpu.set_h_flag((cpu.a & 0x0f) < (rhs & 0x0f)); // half borrow
    cpu.set_c_flag(cpu.a < rhs);                   // borrow
    cpu.a = res;
    return 1;
}

template <bool imm, u8 src>
static inline u8
sbc()
{
    u8 rhs;
    if constexpr (imm)
        rhs = mem.read_byte(cpu.pc++);
    else
        rhs = get_reg8(src);

    rhs += cpu.c_flag();
    u16 res = cpu.a - rhs;
    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(true);
    cpu.set_h_flag((cpu.a & 0x0f) < (rhs & 0x0f)); // half borrow
    cpu.set_c_flag(cpu.a < rhs);                   // borrow
    cpu.a = res;
    return 1;
}

template <bool imm, u8 src>
static inline u8
and_inst()
{
    u8 rhs;
    if constexpr (imm)
        rhs = mem.read_byte(cpu.pc++);
    else
        rhs = get_reg8(src);

    u8 res = cpu.a & rhs;
    cpu.set_z_flag(res == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(true);
    cpu.set_c_flag(false);
    cpu.a = res;
    return 1;
}

template <bool imm, u8 src>
static inline u8
or_inst()
{
    u8 rhs;
    if constexpr (imm)
        rhs = mem.read_byte(cpu.pc++);
    else
        rhs = get_reg8(src);

    u8 res = cpu.a | rhs;
    cpu.set_z_flag(res == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(false);
    cpu.a = res;
    return 1;
}

template <bool imm, u8 src>
static inline u8
xor_inst()
{
    u8 rhs;
    if constexpr (imm)
        rhs = mem.read_byte(cpu.pc++);
    else
        rhs = get_reg8(src);

    u8 res = cpu.a ^ rhs;
    cpu.set_z_flag(res == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(false);
    cpu.a = res;
    return 1;
}

template <bool imm, u8 src>
static inline u8
cp()
{
    u8 rhs;
    if constexpr (imm)
        rhs = mem.read_byte(cpu.pc++);
    else
        rhs = get_reg8(src);

    u16 res = cpu.a - rhs;
    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(true);
    cpu.set_h_flag((cpu.a & 0x0f) < (rhs & 0x0f)); // half borrow
    cpu.set_c_flag((cpu.a) < (rhs));               // borrow
    return 1;
}

template <u8 src>
static inline u8
inc()
{
    auto& reg = get_reg8(src);
    u16   res = reg + 1;
    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag((reg & 0x0f) == 0x0f);
    reg = res;
    return 1;
}

template <u8 src>
static inline u8
dec()
{
    auto& reg = get_reg8(src);
    u16   res = reg - 1;
    cpu.set_z_flag(lower(res) == 0x00);
    cpu.set_n_flag(false);
    cpu.set_h_flag((reg & 0x0f) == 0x00);
    reg = res;
    return 1;
}

} // namespace alu8
