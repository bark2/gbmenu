#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace jp {

template <bool is_rel>
static inline Instruction_Seq
jump()
{
    return { []() {
        u8 cc = (is_rel ? 3 : 4);
        if constexpr (!is_rel)
            cpu.pc = mem.read_word(cpu.pc + 1);
        else if constexpr (is_rel) {
            s8 imm = static_cast<s8>(mem.read_byte(cpu.pc + 1));
            cpu.pc += imm + 2;
        }

        return cc;
    } };
}

static inline Instruction_Seq
jump_hl()
{
    return { []() {
        u8 cc = 1;

        cpu.pc = cpu.hl;

        return cc;
    } };
}

template <bool is_rel, u8 flag>
static inline Instruction_Seq
jump_if()
{
    return { []() {
        bool cond = read_flag<flag>();

        u8 cc;
        if constexpr (is_rel) {
            cc     = 2;
            s8 imm = static_cast<s8>(mem.read_byte(cpu.pc + 1));
            if (cond) {
                cpu.pc += imm + 2;
                cc = 3;
            }
        }
        else if constexpr (!is_rel) {
            cc = 3;
            if (cond) {
                cpu.pc = mem.read_word(cpu.pc);
                cc     = 4;
            }
        }

        return cc;
    } };
}

} // namespace jp
