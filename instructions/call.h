#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace call {

template <bool check_flag, u8 flag>
static inline u8
call(Cpu& cpu, Memory& mem)
{
    u8  cc   = 3;
    u16 addr = mem.read_word(cpu.pc);
    cpu.pc += 2;

    bool cond = true;
    if constexpr (check_flag) {
        if constexpr (flag == 0b00) // nz
            cond = !cpu.z_flag();
        else if (flag == 0b01) // z
            cond = cpu.z_flag();
        else if (flag == 0b10) // nc
            cond = !cpu.c_flag();
        else // c
            cond = cpu.c_flag();
    }

    if (cond) {
        cc = 6;
        cpu.sp -= 2;
        mem.write_word(cpu.sp, cpu.pc);
        cpu.pc = addr;
    }

    return cc;
}

template <bool check_flag, u8 flag>
static inline u8
ret(Cpu& cpu, Memory& mem)
{
    u8   cc   = 4;
    bool cond = true;

    if constexpr (check_flag) {
        cond = false;
        if constexpr (flag == 0b00) // nz
            cond = !cpu.z_flag();
        else if (flag == 0b01) // z
            cond = cpu.z_flag();
        else if (flag == 0b10) // nc
            cond = !cpu.c_flag();
        else // c
            cond = cpu.c_flag();

        cc = cond ? 5 : 2;
    }

    if (cond) {
        cpu.pc = mem.read_word(cpu.sp);
        cpu.sp += 2;
    }

    return cc;
}

static inline u8
reti(Cpu& cpu, Memory& mem)
{
    u8 cc = 4;

    cpu.pc = mem.read_word(cpu.sp);
    cpu.sp += 2;
    cpu.int_master_enable = true;

    return cc;
}

template <u8 t>
static inline u8
rst(Cpu& cpu, Memory& mem)
{
    u8 cc = 4;

    cpu.sp -= 2;
    mem.write_word(cpu.sp, cpu.pc);
    constexpr u16 addr = t << 3;
    cpu.pc             = addr;

    return cc;
}

}
