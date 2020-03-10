#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace call {

template <bool check_flag, u8 flag>
static inline u8
call()
{
    u8  cc   = 3;
    u16 addr = mem.read_word(cpu.pc);
    cpu.pc += 2;

    if (check_flag ? read_flag<flag>() : true) {
        cc = 6;
        cpu.sp -= 2;
        mem.write_word(cpu.sp, cpu.pc);
        cpu.pc = addr;
    }

    return cc;
}

template <bool check_flag, u8 flag>
static inline u8
ret()
{
    u8   cc   = 4;
    bool cond = true;
    u16  addr = mem.read_word(cpu.sp);

    if constexpr (check_flag) {
        cond = read_flag<flag>();
        cc   = cond ? 5 : 2;
    }

    if (cond) {
        cpu.pc = addr;
        cpu.sp += 2;
    }

    return cc;
}

static inline u8
reti()
{
    u8 cc = 4;

    cpu.pc = mem.read_word(cpu.sp);
    cpu.sp += 2;
    cpu.int_master_enable = true;

    return cc;
}

template <u8 t>
static inline u8
rst()
{
    u8 cc = 4;

    cpu.sp -= 2;
    mem.write_word(cpu.sp, cpu.pc);
    u16 addr = t << 3;
    cpu.pc   = addr;

    return cc;
}

}
