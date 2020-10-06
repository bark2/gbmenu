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

    mem.get_ref(0xff05) = (0x00); // tima
    mem.get_ref(0xff05) = (0x00); // tima
    mem.get_ref(0xff06) = (0x00); // tma
    mem.get_ref(0xff07) = (0x00); // tac
    mem.get_ref(0xff10) = (0x80); // nr10
    mem.get_ref(0xff11) = (0xbf); // nr11
    mem.get_ref(0xff12) = (0xf3); // nr12
    mem.get_ref(0xff14) = (0xbf); // nr14
    mem.get_ref(0xff16) = (0x3f); // nr21
    mem.get_ref(0xff17) = (0x00); // nr22
    mem.get_ref(0xff19) = (0xbf); // nr24
    mem.get_ref(0xff1a) = (0x7f); // nr30
    mem.get_ref(0xff1b) = (0xff); // nr31
    mem.get_ref(0xff1c) = (0x9f); // nr32
    mem.get_ref(0xff1e) = (0xbf); // nr33
    mem.get_ref(0xff20) = (0xff); // nr41
    mem.get_ref(0xff21) = (0x00); // nr42
    mem.get_ref(0xff22) = (0x00); // nr43
    mem.get_ref(0xff23) = (0xbf); // nr44
    mem.get_ref(0xff24) = (0x77); // nr50
    mem.get_ref(0xff25) = (0xf3); // nr51
    mem.get_ref(0xff26) = (0xf1); // nr52
    mem.get_ref(0xff40) = (0x91); // lcdc
    mem.get_ref(0xff42) = (0x00); // scy
    mem.get_ref(0xff43) = (0x00); // scx
    // mem.get_ref(reg_stat)= (0x00); // stat
    mem.get_ref(0xff44) = (0x00); // ly
    mem.get_ref(0xff45) = (0x00); // lyc
    mem.get_ref(0xff47) = (0xfc); // bgp
    mem.get_ref(0xff48) = (0xff); // obp0
    mem.get_ref(0xff49) = (0xff); // obp1
    mem.get_ref(0xff4a) = (0x00); // wy
    mem.get_ref(0xff4b) = (0x00); // wx
    mem.get_ref(0xffff) = (0x00); // ie
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
    set_bit(mem.get_ref(reg_if), (u8)i, true);
    if ((mem.get(reg_if) & 0x1f) & (mem.get(reg_ie) & 0x1f))
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

u16 inst_pc;

bool
Cpu::exec()
{
    inst_pc = cpu.pc;
    Instruction inst = instructions[mem.read_byte(cpu.pc)];
    if (mem.read_byte(cpu.pc++) == 0xcb)
        inst = prefix_cb_instructions[mem.read_byte(cpu.pc++)];

    if (inst.fun == illegal)
        return true;

    auto inst_cycles = inst.fun();
    cycles += 4 * inst_cycles;
    last_inst_cycle = 4 * inst_cycles;

    return false;
}
