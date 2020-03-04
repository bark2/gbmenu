#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

namespace tr16 {

template <u8 reg>
static inline Instruction_Seq
ld_reg_imm()
{
    return { []() -> u8 {
        u8  cc         = 3;
        u16 imm        = mem.read_word(cpu.pc + 1);
        get_reg16(reg) = imm;

        return cc;
    } };
}

static inline Instruction_Seq
ld_sp_hl()
{
    return { []() -> u8 {
        u8 cc  = 2;
        cpu.sp = cpu.hl;

        return cc;
    } };
}

template <u8 reg>
static inline Instruction_Seq
push()
{
    return { []() -> u8 {
        u8  cc  = 4;
        u16 src = get_reg16(reg);
        cpu.sp -= 2;
        mem.write_word(cpu.sp, src);

        return cc;
    } };
}

template <u8 reg>
static inline Instruction_Seq
pop()
{
    return { []() -> u8 {
        u8 cc = 4;

        u16& dst = get_reg16(reg);
        dst      = mem.read_word(cpu.sp);
        cpu.sp += 2;

        return cc;
    } };
}

static inline Instruction_Seq
ldhl()
{
    return { []() {
        u8 cc  = 3;
        s8 imm = (s8)mem.read_byte(cpu.pc + 1);

        u32 result = cpu.sp + imm;
        cpu.set_z_flag(false);
        cpu.set_n_flag(false);
        cpu.set_h_flag((cpu.sp & 0xfff) + imm > 0xfff);
        cpu.set_c_flag(result > 0xffff);
        cpu.hl = result;

        return cc;
    } };
}

static inline Instruction_Seq
st_sp()
{
    return { []() {
        u8  cc   = 5;
        u16 addr = mem.read_word(cpu.pc + 1);
        mem.write_word(addr, cpu.sp);

        return cc;
    } };
}
}
