#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace tr8 {

static inline u8&
get_reg8(Cpu& cpu, Memory& mem, u8 src)
{

    switch (src) {
    case 0b000: return cpu.b; break;
    case 0b001: return cpu.c; break;
    case 0b010: return cpu.d; break;
    case 0b011: return cpu.e; break;
    case 0b100: return cpu.h; break;
    case 0b101: return cpu.l; break;
    case 0b110: return mem.buf[cpu.hl]; break;
    case 0b111: return cpu.a; break;
    default: assert(0);
    };
}

template <u8 dst_reg, u8 src_reg>
static inline u8
ld_reg_reg(Cpu& cpu, Memory& mem)
{
    u8& dst = get_reg8(cpu, mem, dst_reg);
    u8& src = get_reg8(cpu, mem, src_reg);
    dst     = src;

    return 1;
}

template <u8 dst_reg>
static inline u8
ld_reg_imm(Cpu& cpu, Memory& mem)
{
    u8& dst = get_reg8(cpu, mem, dst_reg);
    u8  imm = mem.buf[cpu.pc++];
    u8  cc  = (dst == mem.buf[cpu.hl] ? 3 : 2);

    dst = imm;

    return cc;
}

static inline u8
ld_a_bc(Cpu& cpu, Memory& mem)
{
    u8 cc  = 2;
    u8 src = mem.read_byte(cpu.bc);

    cpu.a = src;

    return cc;
}

static inline u8
ld_a_de(Cpu& cpu, Memory& mem)
{
    u8 cc  = 2;
    u8 src = mem.read_byte(cpu.de);

    cpu.a = src;

    return cc;
}

static inline u8
ld_a_io_c(Cpu& cpu, Memory& mem)
{
    u8 cc  = 3;
    u8 src = mem.read_io(cpu.c);

    cpu.a = src;

    return cc;
}

static inline u8
st_io_c_a(Cpu& cpu, Memory& mem)
{
    u8 cc = 2;
    mem.write_io(cpu.c, cpu.a);

    return cc;
}

static inline u8
ld_a_io_imm(Cpu& cpu, Memory& mem)
{
    u8 cc  = 3;
    u8 src = mem.read_io(mem.read_byte(cpu.pc++));

    cpu.a = src;

    return cc;
}

static inline u8
st_io_imm_a(Cpu& cpu, Memory& mem)
{
    u8 cc = 2;
    mem.write_io(mem.read_byte(cpu.pc++), cpu.a);

    return cc;
}

static inline u8
ld_a(Cpu& cpu, Memory& mem)
{
    u8 cc = 4;
    cpu.a = mem.read_byte(mem.read_word(cpu.pc));
    cpu.pc += 2;

    return cc;
}

static inline u8
ld_a_hli(Cpu& cpu, Memory& mem)
{
    u8 cc = 2;
    cpu.a = mem.read_byte(cpu.hl++);

    return cc;
}

static inline u8
ld_a_hld(Cpu& cpu, Memory& mem)
{
    u8 cc = 2;
    cpu.a = mem.read_byte(cpu.hl++);

    return cc;
}

template <bool imm, u8 reg>
static inline u8
st_a(Cpu& cpu, Memory& mem)
{
    u8  cc = imm ? 4 : 2;
    u16 dst;

    if constexpr (imm) {
        dst = mem.read_word(cpu.pc);
        cpu.pc += 2;
    }
    else {
        switch (reg) {
        case 0b000: dst = cpu.bc; break;
        case 0b010: dst = cpu.de; break;
        case 0b100: dst = cpu.hl++; break;
        case 0b110: dst = cpu.hl--; break;
        }
    }
    mem.write_byte(dst, cpu.a);

    return cc;
}

}