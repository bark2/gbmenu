#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"

namespace tr16 {

template <u8 reg>
static inline u8
ld_reg_imm(Cpu& cpu, Memory& mem)
{
    u8  cc  = 3;
    u16 imm = mem.read_word(cpu.pc);
    cpu.pc += 2;
    switch (reg) {
    case 0b00: cpu.bc = imm; break;
    case 0b01: cpu.de = imm; break;
    case 0b10: cpu.hl = imm; break;
    case 0b11: cpu.sp = imm; break;
    }

    return cc;
}

static inline u8
ld_sp_hl(Cpu& cpu, Memory& mem)
{
    u8 cc  = 2;
    cpu.sp = cpu.hl;

    return cc;
}

template <u8 reg>
static inline u8
push(Cpu& cpu, Memory& mem)
{
    u8  cc = 4;
    u16 src;
    switch (reg) {
    case 0b00: src = cpu.bc; break;
    case 0b01: src = cpu.de; break;
    case 0b10: src = cpu.hl; break;
    case 0b11: src = cpu.af; break;
    }

    mem.write_byte(cpu.sp - 1, higher(src));
    mem.write_byte(cpu.sp - 2, lower(src));
    cpu.sp -= 2;

    return cc;
}

template <u8 reg>
static inline u8
pop(Cpu& cpu, Memory& mem)
{
    u8 cc = 4;

    u16* dst;
    switch (reg) {
    case 0b00: dst = &cpu.bc; break;
    case 0b01: dst = &cpu.de; break;
    case 0b10: dst = &cpu.hl; break;
    case 0b11: dst = &cpu.af; break;
    }

    *dst = mem.read_word(cpu.sp);
    cpu.sp += 2;

    return cc;
}

static inline u8
ldhl(Cpu& cpu, Memory& mem)
{
    u8  cc     = 3;
    s8  imm    = mem.read_byte(cpu.pc++);
    u32 result = cpu.sp + imm;

    cpu.set_z_flag(false);
    cpu.set_n_flag(false);
    cpu.set_h_flag((cpu.sp & 0xfff) + imm > 0x1000);
    cpu.set_c_flag(result > 0xffff);
    cpu.hl = result;

    return cc;
}

static inline u8
st_sp(Cpu& cpu, Memory& mem)
{
    u8  cc   = 5;
    u16 addr = mem.read_word(cpu.pc);
    cpu.pc += 2;
    mem.write_word(addr, cpu.sp);

    return cc;
}
}