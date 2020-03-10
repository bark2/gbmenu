#pragma once

#include "../cpu.h"
#include "../mem.h"
#include "../types.h"
#include "common.h"

namespace tr16 {

template <u8 dst>
static inline u8
ld_reg_imm()
{
    u8  cc  = 3;
    u16 imm = mem.read_word(cpu.pc);
    cpu.pc += 2;
    get_reg16(dst) = imm;

    return cc;
}

static inline u8
ld_sp_hl()
{
    u8 cc  = 2;
    cpu.sp = cpu.hl;

    return cc;
}

template <u8 src>
static inline u8
push()
{
    u8 cc = 4;

    cpu.sp -= 2;
    mem.write_word(cpu.sp, get_reg16(src));

    return cc;
}

template <u8 dst>
static inline u8
pop()
{
    u8 cc = 4;

    get_reg16(dst) = mem.read_word(cpu.sp);
    cpu.sp += 2;

    return cc;
}

static inline u8
ldhl()
{
    s8   imm    = (s8)mem.read_byte(cpu.pc++);
    u32  res    = static_cast<u32>((int)cpu.sp + (int)imm);
    auto tmpVal = cpu.sp ^ ((s8)imm) ^ res;
    cpu.set_z_flag(false);
    cpu.set_n_flag(false);
    cpu.set_h_flag((tmpVal & 0x10) == 0x10);
    cpu.set_c_flag((tmpVal & 0x100) == 0x100);
    cpu.hl = 0xffff & res;
    return 3;

    // u8  cc  = 3;
    // s8  imm = (s8)mem.read_byte(cpu.pc++);
    // u32 res = static_cast<u32>(cpu.sp + imm);

    // cpu.set_z_flag(false);
    // cpu.set_n_flag(false);
    // cpu.set_h_flag((cpu.sp & 0xfff) + imm > 0xfff);
    // cpu.set_c_flag(res > 0xffff);
    // cpu.hl = res;

    // return cc;
}

static inline u8
st_sp()
{
    u8  cc   = 5;
    u16 addr = mem.read_word(cpu.pc);
    cpu.pc += 2;
    mem.write_word(addr, cpu.sp);

    return cc;
}

}
