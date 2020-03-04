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

    cpu.a = ~cpu.a;
    cpu.set_n_flag(true);
    cpu.set_h_flag(true);

    return cc;
}

static inline u8
ccf()
{
    u8 cc = 1;

    cpu.set_n_flag(false);
    cpu.set_h_flag(false);
    cpu.set_c_flag(!cpu.c_flag());

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

// Even if a DI instruction is executed in an interrupt routine, the IME flag isset if a return is
// performed with a RETI instruction.
static inline u8
di()
{
    u8 cc = 1;

    cpu.int_master_enable = false;

    return cc;
}

// The IME flag is reset immediately after an interrupt occurs.  The IME flag reset remains in effect
// if control is returned from the interrupt routine bya RET instruction.  However, if an EI instruction
// is executed in the interrupt routine, control is returned with IME = 1.
static inline u8
ei()
{
    u8 cc = 1;

    cpu.int_master_enable = true;

    return cc;
}

}
