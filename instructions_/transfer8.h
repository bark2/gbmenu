#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

namespace tr8 {

template <u8 dst, u8 src>
static inline Instruction_Seq
ld_reg_reg()
{
    return { load_reg<src>, store_reg<dst> };
}

template <u8 dst>
static inline Instruction_Seq
ld_reg_imm()
{
    return { fetch_imm8(), store_reg<dst> };
}

static inline Instruction_Seq
ld_a_io_c()
{
    return { load_addr<IO_C>, read_addr, store_reg<A> };
}

template <u8 dst>
static inline Instruction_Seq
ld_addr_a()
{
    return { load_reg<A>, load_addr<dst>, store_addr };
}

    template <u8 dst>
static inline Instruction_Seq
ld_addr_imm()
{
    return { fetch_imm8, load_addr<dst>, load_reg<A>, store_addr };
}

static inline Instruction_Seq
st_io_imm_a()
{
    return { read_alu_imm8, []() -> u8 {
                mem.write_io(cpu.data_bus, cpu.a);
                return 2;
            } };
}

static inline Instruction_Seq
ld_a_hli()
{
    return { read_alu_reg8<HL>, write_reg8_alu<A>, []() -> u8 {
                cpu.hl++;
                return 1;
            } };
}

static inline Instruction_Seq
ld_hli_a()
{
    return { read_alu_reg8<A>, write_reg8_alu<HL>, []() -> u8 {
                cpu.hl++;
                return 1;
            } };
}

static inline Instruction_Seq
ld_a_hld()
{
    return { read_alu_reg8<HL>, write_alu_reg8<A>, []() -> u8 {
                cpu.hl--;
                return 1;
            } };
}

static inline Instruction_Seq
ld_hld_a()
{
    return { read_alu_reg8<A>, write_reg8_alu<HL>, []() -> u8 {
                cpu.hl++;
                return 1;
            } };
}

template <u8 src>
static inline Instruction_Seq
ld_a_mem()
{
    return { []() -> u8 {
                cpu.data_bus = (src == NN ? mem.read_word(mem.read_word(cpu.pc + 1))
                                          : mem.read_word(get_reg16(src)));
                return (src == NN ? 2 : 1);
            },
             write_reg8_alu<A> };
}

template <u8 dst>
static inline Instruction_Seq
ld_mem_a()
{
    return { []() -> u8 {
        mem.write_byte(get_reg16(dst), cpu.a);
        return 2;
    } };
}

}
