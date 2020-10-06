#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

namespace tr8 {

template <u8 dst, u8 src>
static inline u8
ld_reg_reg()
{
    if constexpr (src == HL)
        get_reg8(dst) = mem.get(cpu.hl);
    else
        get_reg8(dst) = get_reg8(src);
    return 1;
}

template <u8 dst>
static inline u8
ld_reg_imm()
{
    get_reg8(dst) = mem.read_byte(cpu.pc++);
    return (dst == HL ? 3 : 2);
}

static inline u8
ld_a_bc()
{
    u8 cc = 2;
    u8 src = mem.read_byte(cpu.bc);

    cpu.a = src;

    return cc;
}

static inline u8
ld_a_de()
{
    u8 cc = 2;
    u8 src = mem.read_byte(cpu.de);

    cpu.a = src;

    return cc;
}

static inline u8
ld_bc_a()
{
    u8 cc = 2;
    mem.write_byte(cpu.bc, cpu.a);

    return cc;
}

static inline u8
ld_de_a()
{
    u8 cc = 2;
    mem.write_byte(cpu.de, cpu.a);

    return cc;
}

static inline u8
ld_a_io_c()
{
    u8 cc = 2;
    u8 src = mem.read_byte(0xff00 | cpu.c);

    cpu.a = src;

    return cc;
}

static inline u8
st_io_c_a()
{
    u8 cc = 2;
    mem.write_byte(0xff00 | cpu.c, cpu.a);

    return cc;
}

static inline u8
ld_a_io_imm()
{
    u8 cc = 3;
    u8 src = mem.read_byte(0xff00 | mem.read_byte(cpu.pc++));

    cpu.a = src;

    return cc;
}

static inline u8
st_io_imm_a()
{
    u8 cc = 3;
    mem.write_byte(0xff00 | mem.read_byte(cpu.pc++), cpu.a);

    return cc;
}

static inline u8
ld_a_addr()
{
    u8 cc = 4;
    cpu.a = mem.read_byte(mem.read_word(cpu.pc));
    cpu.pc += 2;

    return cc;
}

static inline u8
ld_addr_a()
{
    u8 cc = 4;
    mem.write_byte(mem.read_word(cpu.pc), cpu.a);
    cpu.pc += 2;

    return cc;
}

static inline u8
ld_a_hli()
{
    u8 cc = 2;
    cpu.a = mem.read_byte(cpu.hl++);

    return cc;
}

static inline u8
ld_a_hld()
{
    u8 cc = 2;
    cpu.a = mem.read_byte(cpu.hl--);

    return cc;
}

static inline u8
ld_hli_a()
{
    u8 cc = 2;
    mem.write_byte(cpu.hl++, cpu.a);

    return cc;
}

static inline u8
ld_hld_a()
{
    u8 cc = 2;
    mem.write_byte(cpu.hl--, cpu.a);

    return cc;
}

}
