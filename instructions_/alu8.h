#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

/*
  Z:  Set to 1 when the result of an operation is 0; otherwise reset.
  N:  Set to 1 following execution of the substruction instruction, regardless of the
      result.
  H : Set to 1 when an operation results in carrying from or borrowing to bit 3.
  CY : Set to 1 when an operation results in carrying from or borrowing to bit 7.
*/

namespace alu8 {

template <bool imm, u8 src>
static inline Instruction_Seq
add()
{
    Instruction_Seq result {};

    if constexpr (imm)
        result[0] = read_alu_imm8;
    else if constexpr (!imm)
        result[0] = read_alu_reg8<src>;

    result[1] = []() -> u8 {
        u16 res = cpu.a + cpu.data_bus;
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(false);
        cpu.set_h_flag(((cpu.a & 0x0f) + (cpu.data_bus & 0x0f)) > 0x10);
        cpu.set_c_flag(res > 0xff);
        cpu.a = res;
        return 1;
    };

    return result;
}

template <bool imm, u8 src>
static inline Instruction_Seq
adc()
{
    Instruction_Seq result;

    if constexpr (imm)
        result[0] = read_alu_imm8;
    else if constexpr (!imm)
        result[0] = read_alu_reg8<src>;

    result[1] = []() -> u8 {
        u16 res = cpu.a + cpu.data_bus + cpu.c_flag();
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(false);
        cpu.set_h_flag(((cpu.a & 0x0f) + (cpu.data_bus & 0x0f) + cpu.c_flag()) > 0x10);
        cpu.set_c_flag(res > 0xff);
        cpu.a = res;
        return 1;
    };

    return result;
}

template <bool imm, u8 src>
static inline Instruction_Seq
sub()
{
    Instruction_Seq result;

    if constexpr (imm)
        result[0] = read_alu_imm8;
    else if constexpr (!imm)
        result[0] = read_alu_reg8<src>;

    result[1] = []() -> u8 {
        u16 res = cpu.a - cpu.data_bus;
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(true);
        cpu.set_h_flag((cpu.a & 0x0f) < (cpu.data_bus & 0x0f)); // half borrow
        cpu.set_c_flag(cpu.a < cpu.data_bus);                   // borrow
        cpu.a = res;
        return 1;
    };

    return result;
}

template <bool imm, u8 src>
static inline Instruction_Seq
sbc()
{
    Instruction_Seq result;

    if constexpr (imm)
        result[0] = read_alu_imm8;
    else if constexpr (!imm)
        result[0] = read_alu_reg8<src>;

    result[1] = []() -> u8 {
        u16 rhs = cpu.data_bus + cpu.c_flag();
        u16 res = cpu.a - rhs;
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(true);
        cpu.set_h_flag((cpu.a & 0x0f) < (rhs & 0x0f)); // half borrow
        cpu.set_c_flag(cpu.a < rhs);                   // borrow
        cpu.a = res;
        return 1;
    };

    return result;
}

template <bool imm, u8 src>
static inline Instruction_Seq
and_inst()
{
    Instruction_Seq result;

    if constexpr (imm)
        result[0] = read_alu_imm8;
    else if constexpr (!imm)
        result[0] = read_alu_reg8<src>;

    result[1] = []() -> u8 {
        u16 res = cpu.a & cpu.data_bus;
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(false);
        cpu.set_h_flag(true);
        cpu.set_c_flag(false);
        cpu.a = res;
        return 1;
    };

    return result;
}

template <bool imm, u8 src>
static inline Instruction_Seq
or_inst()
{
    Instruction_Seq result;

    if constexpr (imm)
        result[0] = read_alu_imm8;
    else if constexpr (!imm)
        result[0] = read_alu_reg8<src>;

    result[1] = []() -> u8 {
        u16 res = cpu.a | cpu.data_bus;
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(false);
        cpu.set_h_flag(false);
        cpu.set_c_flag(false);
        cpu.a = res;
        return 1;
    };

    return result;
}

template <bool imm, u8 src>
static inline Instruction_Seq
xor_inst()
{
    Instruction_Seq result;

    if constexpr (imm)
        result[0] = read_alu_imm8;
    else if constexpr (!imm)
        result[0] = read_alu_reg8<src>;

    result[1] = []() -> u8 {
        u16 res = cpu.a ^ cpu.data_bus;
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(false);
        cpu.set_h_flag(false);
        cpu.set_c_flag(false);
        cpu.a = res;
        return 1;
    };

    return result;
}

template <bool imm, u8 src>
static inline Instruction_Seq
cp()
{
    Instruction_Seq result {};

    if constexpr (imm)
        result[0] = read_alu_imm8;
    else if constexpr (!imm)
        result[0] = read_alu_reg8<src>;

    result[1] = []() -> u8 {
    };

    return result;
}

template <u8 src>
static inline Instruction_Seq
inc()
{
    return { []() -> u8 {
        auto& reg = get_reg8(src);
        u16   res = reg + 1;
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(false);
        cpu.set_h_flag((reg & 0x0f) == 0x0f);
        reg = lower(res);
        return 1;
    } };
}

template <u8 src>
static inline Instruction_Seq
dec()
{
    return { []() -> u8 {
        auto& reg = get_reg8(src);
        u16   res = reg - 1;
        cpu.set_z_flag(lower(res) == 0x00);
        cpu.set_n_flag(false);
        cpu.set_h_flag((reg & 0x0f) == 0x00);
        reg = lower(res);
        return 1;
    } };
}

} // namespace alu8
