#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace jp {

template <bool is_rel>
static inline u8
jump(Cpu& cpu, Memory& mem)
{
    u8 cc = (is_rel ? 3 : 4);
    if constexpr (!is_rel)
        cpu.pc = mem.read_word(cpu.pc);
    else {
        s8 imm = static_cast<s8>(mem.read_byte(cpu.pc++));
        cpu.pc += imm;
    }

    return cc;
}

static inline u8
jump_hl(Cpu& cpu, Memory& mem)
{
    u8 cc = 1;

    cpu.pc = cpu.hl;

    return cc;
}

template <bool is_rel, u8 flag>
static inline u8
jump_if(Cpu& cpu, Memory& mem)
{
    bool cond;
    if constexpr (flag == 0b00) // nz
        cond = !cpu.z_flag();
    else if (flag == 0b01) // z
        cond = cpu.z_flag();
    else if (flag == 0b10) // nc
        cond = !cpu.c_flag();
    else // c
        cond = cpu.c_flag();

    u8 cc;
    if constexpr (is_rel) {
        cc     = 2;
        s8 imm = static_cast<s8>(mem.read_byte(cpu.pc++));
        if (cond) {
            cpu.pc += imm;
            cc = 3;
        }
    }
    else {
        cc = 3;
        if (cond) {
            cpu.pc = mem.read_word(cpu.pc);
            cc     = 4;
        }
    }

    return cc;
}

} // namespace jp
