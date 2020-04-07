#include "cpu.h"
#include "instructions/misc.h"
#include "memory"

#include "instruction.h"
#include "types.h"

#include <cassert>
#include <cstdio>
#include <vector>

void
Cpu::boot_sequence(Memory& mem)
{
    cycles = 0;
    last_inst_cycle = 0;

    af = 0x01b0;
    bc = 0x0013;
    de = 0x00d8;
    hl = 0x014d;
    sp = 0xfffe;
    pc = 0x0100;

    mem.jp = 0xff;

    mem.buf[0xff05] = (0x00); // tima
    mem.buf[0xff05] = (0x00); // tima
    mem.buf[0xff06] = (0x00); // tma
    mem.buf[0xff07] = (0x00); // tac
    mem.buf[0xff10] = (0x80); // nr10
    mem.buf[0xff11] = (0xbf); // nr11
    mem.buf[0xff12] = (0xf3); // nr12
    mem.buf[0xff14] = (0xbf); // nr14
    mem.buf[0xff16] = (0x3f); // nr21
    mem.buf[0xff17] = (0x00); // nr22
    mem.buf[0xff19] = (0xbf); // nr24
    mem.buf[0xff1a] = (0x7f); // nr30
    mem.buf[0xff1b] = (0xff); // nr31
    mem.buf[0xff1c] = (0x9f); // nr32
    mem.buf[0xff1e] = (0xbf); // nr33
    mem.buf[0xff20] = (0xff); // nr41
    mem.buf[0xff21] = (0x00); // nr42
    mem.buf[0xff22] = (0x00); // nr43
    mem.buf[0xff23] = (0xbf); // nr44
    mem.buf[0xff24] = (0x77); // nr50
    mem.buf[0xff25] = (0xf3); // nr51
    mem.buf[0xff26] = (0xf1); // nr52
    mem.buf[0xff40] = (0x91); // lcdc
    mem.buf[0xff42] = (0x00); // scy
    mem.buf[0xff43] = (0x00); // scx
    // mem.buf[reg_stat]= (0x00); // stat
    mem.buf[0xff44] = (0x00); // ly
    mem.buf[0xff45] = (0x00); // lyc
    mem.buf[0xff47] = (0xfc); // bgp
    mem.buf[0xff48] = (0xff); // obp0
    mem.buf[0xff49] = (0xff); // obp1
    mem.buf[0xff4a] = (0x00); // wy
    mem.buf[0xff4b] = (0x00); // wx
    mem.buf[0xffff] = (0x00); // ie
}

u8
Cpu::get_flag(Cpu::Flag flag)
{
    return flags & (1 << flag);
}

void
Cpu::set_flag(u8 flag, bool value)
{
    if (value)
        flags |= 1 << flag;
    else
        flags &= ~(1 << flag);
}

void
Cpu::set_c_flag(bool value)
{
    set_flag(4, value);
}

void
Cpu::set_h_flag(bool value)
{
    set_flag(5, value);
}

void
Cpu::set_n_flag(bool value)
{
    set_flag(6, value);
}

void
Cpu::set_z_flag(bool value)
{
    set_flag(7, value);
}

void
Cpu::set_interupt(Interupt i)
{
    set_bit(mem.buf[reg_if], (u8)i, true);
    if ((mem.buf[reg_if] & 0x1f) & (mem.buf[reg_ie] & 0x1f))
        halt = false;
}

string
to_string(const Cpu& cpu)
{
    string result;
    result += "af:\t\t" + conv_word_hex(cpu.af) + "\n";
    result += "bc:\t\t" + conv_word_hex(cpu.bc) + "\n";
    result += "de:\t\t" + conv_word_hex(cpu.de) + "\n";
    result += "hl:\t\t" + conv_word_hex(cpu.hl) + "\n";
    result += "sp:\t\t" + conv_word_hex(cpu.sp) + "\n";
    return result;
}

std::ostream&
operator<<(std::ostream& o, const Cpu& cpu)
{
    return o << to_string(cpu);
}

// TODO: we can fetch until no jump ?
// static std::vector<Instruction_Seq> queue;

bool
Cpu::handle_interupts()
{
    u8 if_val = mem.read_byte(reg_if);
    u8 ie_val = mem.read_byte(reg_ie);

    if (int_master_enable && ((if_val & 0x1f) & (ie_val & 0x1f))) {
        assert((if_val & 0xe0) == 0x00);
        assert((ie_val & 0xe0) == 0x00);
        // disable halt
        // halt = false;
        // all interrupts are now prohibited.
        int_master_enable = false;
        // the contents of the $pc are pushed onto the stack RAM.
        sp -= 2;
        mem.write_word(sp, pc);

        // find the highest priority requested interupt
        int i;
        for (i = 0; i < 5; i++)
            if (get_bit(ie_val, i) && get_bit(if_val, i))
                break;

        // unset the interupt request register
        mem.write_byte(reg_if, if_val & ~(1 << i));

        pc = 0x40 + (i << 3);
        return true;
    }

    return false;
}

bool
Cpu::exec()
{
    Instruction inst = instructions[mem.buf[cpu.pc]];
    if (mem.buf[cpu.pc++] == 0xcb)
        inst = prefix_cb_instructions[mem.buf[cpu.pc++]];

    if (inst.fun == illegal)
        return true;

    auto inst_cycles = inst.fun();
    cycles += 4 * inst_cycles;
    last_inst_cycle = 4 * inst_cycles;

    return false;
}
