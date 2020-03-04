#pragma once

#include "bits.h"
#include <cassert>
#include <cstdio>
#include <iostream>

// 0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
// 4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
// 8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
// A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
// C000-CFFF   4KB Work RAM Bank 0 (WRAM)
// D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
// E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
// FE00-FE9F   Sprite Attribute Table (OAM)
// FEA0-FEFF   Not Usable
// FF00-FF7F   I/O Ports
// FF80-FFFE   High RAM (HRAM)
// FFFF        Interrupt Enable Register

// Port/Mode registers
static constexpr u16 mem_p1   = 0xff00; // p1
static constexpr u16 mem_sb   = 0xff01; // sb
static constexpr u16 mem_sc   = 0xff02; // sc
static constexpr u16 mem_div  = 0xff04; // div
static constexpr u16 mem_tima = 0xff05; // tima
static constexpr u16 mem_tma  = 0xff06; // tma
static constexpr u16 mem_tac  = 0xff07; // tac
// LCD Registers
static constexpr u16 mem_nr10 = 0xff10; // nr10
static constexpr u16 mem_nr11 = 0xff11; // nr11
static constexpr u16 mem_nr12 = 0xff12; // nr12
static constexpr u16 mem_nr14 = 0xff14; // nr14
static constexpr u16 mem_nr21 = 0xff16; // nr21
static constexpr u16 mem_nr22 = 0xff17; // nr22
static constexpr u16 mem_nr24 = 0xff19; // nr24
static constexpr u16 mem_nr30 = 0xff1a; // nr30
static constexpr u16 mem_nr31 = 0xff1b; // nr31
static constexpr u16 mem_nr32 = 0xff1c; // nr32
static constexpr u16 mem_nr33 = 0xff1e; // nr33
static constexpr u16 mem_nr41 = 0xff20; // nr41
static constexpr u16 mem_nr42 = 0xff21; // nr42
static constexpr u16 mem_nr43 = 0xff22; // nr43
static constexpr u16 mem_nr44 = 0xff23; // nr44
static constexpr u16 mem_nr50 = 0xff24; // nr50
static constexpr u16 mem_nr51 = 0xff25; // nr51
static constexpr u16 mem_nr52 = 0xff26; // nr52
static constexpr u16 mem_lcdc = 0xff40; // lcdc
static constexpr u16 mem_stat = 0xff41; // stat
static constexpr u16 mem_scy  = 0xff42; // scy
static constexpr u16 mem_scx  = 0xff43; // scx
static constexpr u16 mem_ly   = 0xff44; // ly
static constexpr u16 mem_lyc  = 0xff45; // lyc
static constexpr u16 mem_bgp  = 0xff47; // bgp
static constexpr u16 mem_obp0 = 0xff48; // obp0
static constexpr u16 mem_obp1 = 0xff49; // obp1
static constexpr u16 mem_wy   = 0xff4a; // wy
static constexpr u16 mem_wx   = 0xff4b; // wx
static constexpr u16 mem_if   = 0xff0f; // if
static constexpr u16 mem_ie   = 0xffff; // ie

struct Memory {
    u8* buf;

    int
    init(const char* file_path)
    {
        auto mem_size = 0xffff;
        buf           = (u8*)calloc(mem_size, 1);
        if (!buf) return 1;

        FILE* in = fopen(file_path, "r");
        if (!in) return 1;

        auto size = fread(buf, 1, 0x8000, in);

        fclose(in);
        return 0;
    }

    ~Memory() { free(buf); }

    constexpr const u8&
    read_byte(u16 addr) const
    {
        return buf[addr];
    }

    u16
    read_word(u16 addr) const
    {
        return concat(buf[addr + 1], buf[addr]);
    }

    void
    write_byte(u16 addr, u8 byte)
    {
        switch (addr) {
        case mem_scy: {
            printf("scy: %d\n", byte);
        } break;
        case mem_sc: {
            if (byte == 0x81) {
                // printf("serial: %c\n", read_byte(mem_sb));
                printf("%c", read_byte(mem_sb));
            }
        } break;
        default: break;
        }
        buf[addr] = byte;
    }

    void
    write_word(u16 addr, u16 word)
    {
        write_byte(addr, lower(word));
        write_byte(addr + 1, higher(word));
    }

    const u8&
    read_io(u8 offset) const
    {
        return read_byte(0xff00 + offset);
    }

    void
    write_io(u8 offset, u8 byte)
    {
        write_byte(0xff00 + offset, byte);
    }
};

extern Memory mem;
