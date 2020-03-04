#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

namespace call {

template <bool check_flag, u8 flag>
static inline Instruction_Seq
call()
{
    return { []() {
        u8  cc   = 3;
        u16 addr = mem.read_word(cpu.pc + 1);

        bool cond = true;
        if constexpr (check_flag) cond = read_flag<flag>();

        if (cond) {
            cc = 6;
            cpu.sp -= 2;
            mem.write_word(cpu.sp, cpu.pc + 3);
            cpu.pc = addr;
        }

        return cc;
    } };
}

template <bool check_flag, u8 flag>
static inline Instruction_Seq
ret()
{
    return { []() {
        u8   cc   = 4;
        bool cond = true;

        if constexpr (check_flag) {
            cond = read_flag<flag>();
            cc   = cond ? 5 : 2;
        }

        if (cond) {
            cpu.pc = mem.read_word(cpu.sp);
            cpu.sp += 2;
        }

        return cc;
    } };
}

static inline Instruction_Seq
reti()
{
    return { []() {
        u8 cc = 4;

        cpu.pc = mem.read_word(cpu.sp);
        cpu.sp += 2;
        cpu.int_master_enable = true;

        return cc;
    } };
}

template <u8 t>
static inline Instruction_Seq
rst()
{
    return { []() {
        u8 cc = 4;

        cpu.sp -= 2;
        mem.write_word(cpu.sp, cpu.pc);
	u16 intreput_addr = t << 3;
        cpu.pc   = intreput_addr;

        return cc;
    } };
}

}
