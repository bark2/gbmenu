#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

namespace alu16 {

template <u8 src>
static inline Instruction_Seq
add_hl()
{
    return { read_alu_reg16<src>,
             []() -> u8 {
                 u32 res = cpu.hl + cpu.data_bus;
                 cpu.set_n_flag(false);
                 cpu.set_h_flag((lower(cpu.hl) + lower(cpu.data_bus)) > 0xfff);
                 cpu.set_c_flag(res > 0xffff);
                 cpu.data_bus = static_cast<u16>(res);
                 return 2;
             },
             write_reg16_alu<HL> };
}

static inline Instruction_Seq
add_sp()
{
    return { read_alu_imm8,
             []() -> u8 {
                 s8  imm = lower(cpu.data_bus);
                 u32 res = cpu.sp + imm;
                 cpu.set_z_flag(false);
                 cpu.set_n_flag(false);
                 cpu.set_h_flag(lower(cpu.sp) + imm > 0xfff);
                 cpu.set_c_flag(res > 0xffff);
                 cpu.data_bus = static_cast<u16>(res);
                 return 4;
             },
             write_reg16_alu<SP> };
}

template <u8 src>
static inline Instruction_Seq
inc()
{
    return { []() -> u8 {
        auto& reg = get_reg16(src);
        u32   res = reg + 1;
        cpu.set_z_flag(false);
        cpu.set_n_flag(false);
        cpu.set_h_flag(reg + 1 > 0xfff);
        cpu.set_c_flag(res > 0xffff);
        reg = res;
        return 2;
    } };
}

template <u8 src>
static inline Instruction_Seq
dec()
{
    return {
        []() -> u8 {
            auto& reg = get_reg16(src);
            u32   res = reg - 1;
            cpu.set_z_flag(false);
            cpu.set_n_flag(false);
            // FIXME: flag
            assert(0);
            cpu.set_h_flag(reg - 1 > 0xfff);
            cpu.set_c_flag(res > 0xffff);
            reg = res;
            return 2;
        },
    };
}

} // namespace alu16
