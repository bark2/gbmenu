#include "cpu.h"
#include "memory"

#include "instruction.h"

#include <cassert>
#include <cstdio>
#include <vector>

using Int_t = Cpu::Int_t;

void
Cpu::boot_sequence(Memory& mem)
{
    cc = 0;

    af = 0x01B0;
    bc = 0x0013;
    de = 0x00D8;
    hl = 0x014D;
    sp = 0xFFFE;
    pc = 0x0100;

    mem.write_byte(0xff05, 0x00); // tima
    mem.write_byte(0xff05, 0x00); // tima
    mem.write_byte(0xff06, 0x00); // tma
    mem.write_byte(0xff07, 0x00); // tac
    mem.write_byte(0xff10, 0x80); // nr10
    mem.write_byte(0xff11, 0xbf); // nr11
    mem.write_byte(0xff12, 0xf3); // nr12
    mem.write_byte(0xff14, 0xbf); // nr14
    mem.write_byte(0xff16, 0x3f); // nr21
    mem.write_byte(0xff17, 0x00); // nr22
    mem.write_byte(0xff19, 0xbf); // nr24
    mem.write_byte(0xff1a, 0x7f); // nr30
    mem.write_byte(0xff1b, 0xff); // nr31
    mem.write_byte(0xff1c, 0x9f); // nr32
    mem.write_byte(0xff1e, 0xbf); // nr33
    mem.write_byte(0xff20, 0xff); // nr41
    mem.write_byte(0xff21, 0x00); // nr42
    mem.write_byte(0xff22, 0x00); // nr43
    mem.write_byte(0xff23, 0xbf); // nr44
    mem.write_byte(0xff24, 0x77); // nr50
    mem.write_byte(0xff25, 0xf3); // nr51
    mem.write_byte(0xff26, 0xf1); // nr52
    mem.write_byte(0xff40, 0x91); // lcdc
    mem.write_byte(0xff42, 0x00); // scy
    mem.write_byte(0xff43, 0x00); // scx
    mem.write_byte(0xff45, 0x00); // lyc
    mem.write_byte(0xff47, 0xfc); // bgp
    mem.write_byte(0xff48, 0xff); // obp0
    mem.write_byte(0xff49, 0xff); // obp1
    mem.write_byte(0xff4a, 0x00); // wy
    mem.write_byte(0xff4b, 0x00); // wx
    mem.write_byte(0xffff, 0x00); // ie
}

u8
Cpu::get_flag(u8 flag)
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

u8
Cpu::c_flag()
{
    return get_flag(4);
}

void
Cpu::set_c_flag(bool value)
{
    set_flag(4, value);
}

u8
Cpu::h_flag()
{
    return get_flag(5);
}

void
Cpu::set_h_flag(bool value)
{
    set_flag(5, value);
}

u8
Cpu::n_flag()
{
    return get_flag(6);
}

void
Cpu::set_n_flag(bool value)
{
    set_flag(6, value);
}

u8
Cpu::z_flag()
{
    return get_flag(7);
}

void
Cpu::set_z_flag(bool value)
{
    set_flag(7, value);
}

// enum Int_t { JOYPAD, SERIAL, TIMER, LCD_STAT, VERTICAL_BLANK, INT_T_COUNT };

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

bool Cpu::exec(u8 (*inst)())
{
    u8 reg_if = mem.read_byte(mem_if);
    u8 reg_ie = mem.read_byte(mem_ie);

    if (int_master_enable && ((reg_if & 0x1f) & (reg_ie & 0x1f))) {
        printf("interupt\n");
        fflush(stdout);
        // enabled interupt is requested
        assert((reg_if & 0xe0) == 0x00);
        assert((reg_ie & 0xe0) == 0x00);
        // the IME flag is low, and all interrupts are now prohibited.
        int_master_enable = false;
        // the contents of the $pc are pushed onto the stack RAM.
        sp -= 2;
        mem.write_word(sp, pc);

        // find the highest priority requested interupt
        int i;
        for (i = 0; i < 5; i++)
            if ((reg_ie & (1 << i)) && (reg_if & (1 << i))) break;

        // unset the interupt request register
        mem.write_byte(reg_if, reg_if & ~(1 << i));

        pc = 0x40 + 0x8 * i;
    }

    auto inst_cc = inst();
    if (inst_cc == 100) return true; // unimplemented
    cc += inst_cc;
    return false;
}
