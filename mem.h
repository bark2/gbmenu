#pragma once

#include "bits.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <qglobal.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

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
static constexpr u16 mem_vram = 0x8000;
static constexpr u16 mem_vram_end = 0xa000;
static constexpr u16 mem_eram = 0xa000;
static constexpr u16 mem_eram_end = 0xc000;
static constexpr u16 mem_wram0 = 0xc000;
static constexpr u16 mem_wram0_end = 0xd000;
static constexpr u16 mem_wram1 = 0xd000;
static constexpr u16 mem_wram_end = 0xe000;
static constexpr u16 mem_oam = 0xfe00;
static constexpr u16 mem_oam_end = 0xfea0;
static constexpr u16 mem_hram = 0xff80;
static constexpr u16 mem_hram_end = 0xffff;

static constexpr u16 reg_jp = 0xff00;   // joypad
static constexpr u16 reg_sb = 0xff01;   // sb
static constexpr u16 reg_sc = 0xff02;   // sc
static constexpr u16 reg_div = 0xff04;  // div
static constexpr u16 reg_tima = 0xff05; // tima
static constexpr u16 reg_tma = 0xff06;  // tma
static constexpr u16 reg_tac = 0xff07;  // tac

static constexpr u16 reg_nr10 = 0xff10; // nr10
static constexpr u16 reg_nr11 = 0xff11; // nr11
static constexpr u16 reg_nr12 = 0xff12; // nr12
static constexpr u16 reg_nr14 = 0xff14; // nr14
static constexpr u16 reg_nr21 = 0xff16; // nr21
static constexpr u16 reg_nr22 = 0xff17; // nr22
static constexpr u16 reg_nr24 = 0xff19; // nr24
static constexpr u16 reg_nr30 = 0xff1a; // nr30
static constexpr u16 reg_nr31 = 0xff1b; // nr31
static constexpr u16 reg_nr32 = 0xff1c; // nr32
static constexpr u16 reg_nr33 = 0xff1e; // nr33
static constexpr u16 reg_nr41 = 0xff20; // nr41
static constexpr u16 reg_nr42 = 0xff21; // nr42
static constexpr u16 reg_nr43 = 0xff22; // nr43
static constexpr u16 reg_nr44 = 0xff23; // nr44
static constexpr u16 reg_nr50 = 0xff24; // nr50
static constexpr u16 reg_nr51 = 0xff25; // nr51
static constexpr u16 reg_nr52 = 0xff26; // nr52

static constexpr u16 reg_lcdc = 0xff40; // lcdc
static constexpr u16 reg_stat = 0xff41; // stat
static constexpr u16 reg_scy = 0xff42;  // scy
static constexpr u16 reg_scx = 0xff43;  // scx
static constexpr u16 reg_ly = 0xff44;   // ly
static constexpr u16 reg_lyc = 0xff45;  // lyc
static constexpr u16 reg_dma = 0xff46;  // dma
static constexpr u16 reg_bgp = 0xff47;  // bgp
static constexpr u16 reg_obp0 = 0xff48; // obp0
static constexpr u16 reg_obp1 = 0xff49; // obp1
static constexpr u16 reg_wy = 0xff4a;   // wy
static constexpr u16 reg_wx = 0xff4b;   // wx

static constexpr u16 reg_if = 0xff0f; // if
static constexpr u16 reg_ie = 0xffff; // ie

struct Cartridge_Header {
    array<u8, 1 + 0x103 - 0x100> entry_point;
    array<u8, 1 + 0x133 - 0x104> nintendo_logo;
    array<u8, 1 + 0x143 - 0x134> title;
    // u8 cgb_flag : 0x143;
    array<u8, 1 + 0x145 - 0x144> new_licensee_code;
    u8 sgb_flag;
    u8 cartridge_type;
    u8 rom_size;
    u8 ram_size;
    u8 destination_code;
    u8 old_licensee_code;
    u8 mask_rom_ver_number;
    u8 header_checksum;
    array<u8, 2> global_checksum;
};

struct Memory {
    u8* buf;
    Cartridge_Header cart;
    u8 rom_bank;
    u8 rom_bank_count;
    enum class Select_Mode { UNKNOWN, ROM, RAM } rom_selection_mode;

    // Bit 5 - P15 Select Button Keys      (0=Select)
    // Bit 4 - P14 Select Direction Keys   (0=Select)
    // Bit 3 - P13 Input Down  or Start    (0=Pressed) (Read Only)
    // Bit 2 - P12 Input Up    or Select   (0=Pressed) (Read Only)
    // Bit 1 - P11 Input Left  or Button B (0=Pressed) (Read Only)
    // Bit 0 - P10 Input Right or Button A (0=Pressed) (Read Only)
    enum struct Joypad : u8 { RIGHT = 0, LEFT, UP, DOWN, A, B, SELECT, START, JOYPAD_COUNT, DEBUGGER_VRAM };
    u8 jp;

    int
    init(const char* file_path)
    {
        FILE* in = fopen(file_path, "r");
        if (!in)
            return 1;

        fseek(in, 0L, SEEK_END);
        auto rom_size = ftell(in);
        fseek(in, 0L, SEEK_SET);

	printf("ROM size: %ld\n", rom_size);
        buf = (u8*)malloc(rom_size);
        if (!buf)
            return 1;

        fread(buf, 1, rom_size, in);
        fclose(in);

        cart = *((Cartridge_Header*)(&buf[0x100]));
        rom_bank_count = (cart.rom_size ? 2 << cart.rom_size : 0);
        printf("sgb_flag: %x\ncart type: %x, rom size: %x, ram size: %x\n",
               cart.sgb_flag,
               cart.cartridge_type,
               cart.rom_size,
               cart.ram_size);

        return 0;
    }

    ~Memory() { free(buf); }

    u8
    read_byte(u16 addr) const
    {
        u8 result {};
        switch (addr) {
        case reg_jp: {
            bool select_dir_keys = get_bit(buf[reg_jp], 4);
            bool select_buttom_keys = get_bit(buf[reg_jp], 5);
            if (!(select_dir_keys ^ select_buttom_keys)) {
                printf("Warnning: !(select_dir_keys ^ select_buttom_keys)\n");
            }
            if (select_dir_keys)
                result = (buf[reg_jp] & 0xf0) | ((jp >> 4) & 0x0f);
            else if (select_buttom_keys)
                result = (buf[reg_jp] & 0xf0) | (jp & 0x0f);
        } break;
        default: result = buf[addr];
        }

        const Screen_Mode mode = (Screen_Mode)(buf[reg_stat] & 0b11);
        // disallow read from oam in OAM mode
        // if (get_bit(buf[reg_lcdc], 7) && mode == Screen_Mode::OAM && addr > mem_oam && addr < mem_oam_end)
            // return 0xff;
        // disallow read from vram in * mode
        // if (get_bit(buf[reg_lcdc], 7) && mode == Screen_Mode::TRANSFARING &&
            // ((addr > mem_oam && addr < mem_oam_end) || (addr > mem_vram && addr < mem_vram_end)))
            // return 0xff;

        return result;
    }

    u16
    read_word(u16 addr) const
    {
        return concat(buf[addr + 1], buf[addr]);
    }

    void
    write_byte(u16 addr, u8 byte)
    {
        extern Breakpoint breakpoint;
        if (addr >= 0xFEA0 && addr <= 0xFEFF) { // not usable
            return;
        }
        if (cart.cartridge_type == 0) {
            if (addr <= 0x1FFF) {
                return;
            }
            else if (addr >= 0x2000 && addr <= 0x3FFF) {
                return;
            }
            else if (addr >= 0x4000 && addr <= 0x5FFF) {
                return;
            }
            else if (addr >= 0x6000 && addr <= 0x7FFF) {
                return;
            }
        }
        else if (cart.cartridge_type == 1) {
            if (addr <= 0x1FFF) { // register 0
                assert(false && "unimplemented");
                if (byte == 0xa) // enable RAM
                    return;
                else if (byte == 0) // disable RAM
                    return;
                return;
            }
            else if (addr >= 0x2000 && addr <= 0x3FFF) { // register 1 - lower bits of ROM bank selection
                rom_bank = (rom_bank & (0x3 << 9)) | (byte & 0x1f);
                if (rom_bank == 0x00)
                    rom_bank &= 0x01 & 0x1f;
                else if (rom_bank == 0x20)
                    rom_bank &= 0x21 & 0x1f;
                else if (rom_bank == 0x40)
                    rom_bank &= 0x41 & 0x1f;
                else if (rom_bank == 0x60)
                    rom_bank &= 0x61 & 0x1f;
                assert(rom_bank <= rom_bank_count);
                return;
            }
            else if (addr >= 0x4000 && addr <= 0x5FFF) { // register 2 - higher bits of ROM bank selection
                if (rom_selection_mode == Select_Mode::ROM && byte < 4)
                    rom_bank = ((byte & 0x3) << 9) | (rom_bank & 0x1f);
                else if (rom_selection_mode == Select_Mode::RAM)
                    assert(false && "unimplemented");
                return;
            }
            else if (addr >= 0x6000 && addr <= 0x7FFF) { // register 3 - ROM bank selection mode
                if (byte == 0)
                    rom_selection_mode = Select_Mode::ROM;
                else if (byte == 1)
                    rom_selection_mode = Select_Mode::RAM;
                return;
            }
        }

        switch (addr) {
        case reg_lcdc: {
            if (get_bit(buf[reg_lcdc], 7) && !get_bit(byte, 7)) { // close lcd
                assert((Screen_Mode)(buf[reg_stat] & 0b11) == Screen_Mode::V_BLANK);
                byte = (byte & (~0b11)) | (u8)Screen_Mode::V_BLANK;
            }
        } break;
        case reg_stat: {
            byte = (byte & ~(0b111)) | (buf[reg_stat] & 0b111);
        } break;
        case reg_dma: {
            u16 src = (byte << 8);
            memcpy(&buf[mem_oam], &buf[src], mem_oam_end - mem_oam);
        } break;
        case reg_div: {
            byte = 0;
        } break;
        case reg_sc: {
            if (byte == 0x81) {
                printf("%c", buf[reg_sb]);
                fflush(stdout);
            }
        } break;
            // write allowed
        case reg_if:
        case reg_sb:
        case reg_tima:
        case reg_tma:
        case reg_tac:
        case reg_scy:
        case reg_scx:
        case reg_lyc:
        case reg_bgp:
        case reg_obp0:
        case reg_obp1:
        case reg_wy:
        case reg_wx:
        case reg_jp: {
        } break;
        //
        default: {
            if (addr >= 0xFF10 && addr <= 0xFF3f) // audio
                break;
            if (addr >= 0xFF00 && addr <= 0xFF7F) { // disable other io
                printf("io addr write: 0x%x\n", addr);
                fflush(stdout);
                return;
            }
        } break;
        }

        const Screen_Mode lcd_mode = (Screen_Mode)(buf[reg_stat] & 0b11);
        // disallow writing to oam in OAM mode
        if (get_bit(buf[reg_lcdc], 7) && lcd_mode == Screen_Mode::OAM && addr > mem_oam && addr < mem_oam_end)
            return;
        // disallow writing to vram in * mode
        if (get_bit(buf[reg_lcdc], 7) && lcd_mode == Screen_Mode::TRANSFARING &&
            ((addr > mem_oam && addr < mem_oam_end) || (addr > mem_vram && addr < mem_vram_end)))
            return;
        buf[addr] = byte;
    }

    void
    write_word(u16 addr, u16 word)
    {
        write_byte(addr, lower_byte(word));
        write_byte(addr + 1, word >> 8);
    }
};

extern Memory mem;
