#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace misc {

static inline u8
nop()
{
    return 1;
}

static inline u8
cpl()
{
    u8 cc = 1;

    cpu.set_n_flag(true);
    cpu.set_h_flag(true);
    cpu.a = ~cpu.a;

    return cc;
}

static inline u8
ccf()
{
    u8 cc = 1;

    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(!(cpu.get_flag(Cpu::Flag::C) >> Cpu::Flag::C));

    return cc;
}

static inline u8
scf()
{
    u8 cc = 1;

    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(true);

    return cc;
}

static inline u8
di()
{
    u8 cc = 1;

    cpu.int_master_enable = false;

    return cc;
}

static inline u8
ei()
{
    u8 cc = 1;

    cpu.int_master_enable = true;

    return cc;
}

}
