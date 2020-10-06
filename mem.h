#pragma once

#include "bits.h"
// #include "cpu.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <qglobal.h>
#include <type_traits>
#include <vector>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define LOG(msg) (Log(__FILE__, __LINE__, LogData<None>() << msg))

struct None { };

template<typename List>
struct LogData {
    List list;
};

template<typename List>
void __attribute__ ((noinline)) Log(const char* file, int line, 
         LogData<List>&& data)
{
    std::cout << file << ":" << line << ": ";
    output(std::cout, std::move(data.list));
    std::cout << std::endl;
}

template<typename Begin, typename Value>
constexpr LogData<std::pair<Begin&&, Value&&>> operator<<(LogData<Begin>&& begin, 
                                                          Value&& value) noexcept
{
    return {{ std::forward<Begin>(begin.list), std::forward<Value>(value) }};
}

template<typename Begin, size_t n>
constexpr LogData<std::pair<Begin&&, const char*>> operator<<(LogData<Begin>&& begin, 
                                                              const char (&value)[n]) noexcept
{
    return {{ std::forward<Begin>(begin.list), value }};
}

typedef std::ostream& (*PfnManipulator)(std::ostream&);

template<typename Begin>
constexpr LogData<std::pair<Begin&&, PfnManipulator>> operator<<(LogData<Begin>&& begin, 
                                                                 PfnManipulator value) noexcept
{
    return {{ std::forward<Begin>(begin.list), value }};
}

template <typename Begin, typename Last>
void output(std::ostream& os, std::pair<Begin, Last>&& data)
{
    output(os, std::move(data.first));
    os << data.second;
}

inline void output(std::ostream& os, None)
{ }

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

static constexpr u16 mem_rom_start = 0x0000;
static constexpr u16 mem_rom_end = 0x4000;
static constexpr u16 mem_switchable_rom_start = 0x4000;
static constexpr u16 mem_switchable_rom_end = 0x8000;
static constexpr u16 mem_vram_start = 0x8000;
static constexpr u16 mem_vram_end = 0xa000;
static constexpr u16 mem_eram_start = 0xa000;
static constexpr u16 mem_eram_end = 0xc000;
static constexpr u16 mem_wram0_start = 0xc000;
static constexpr u16 mem_wram0_end = 0xd000;
static constexpr u16 mem_wram1_start = 0xd000;
static constexpr u16 mem_wram1_end = 0xe000;
static constexpr u16 mem_oam_start = 0xfe00;
static constexpr u16 mem_oam_end = 0xfea0;
static constexpr u16 mem_unusable_start = 0xfea0;
static constexpr u16 mem_unusable_end = 0xff00;
static constexpr u16 mem_io_start = 0xff00;
static constexpr u16 mem_io_end = 0xff80;
static constexpr u16 mem_hram_start = 0xff80;
static constexpr u16 mem_hram_end = 0xffff;

enum struct MemorySection { ROM, SWITCHABLE_ROM, VRAM, ERAM, WRAM0, WRAM1, OAM, UNSUABLE, IO, HRAM };

static MemorySection
addr_to_section(u16 addr)
{
    if (addr < mem_switchable_rom_start) {
        return MemorySection::ROM;
    }
    else if (addr >= mem_switchable_rom_start && addr < mem_switchable_rom_end) {
        return MemorySection::SWITCHABLE_ROM;
    }
    else if (addr >= mem_vram_start && addr < mem_vram_end) {
        return MemorySection::VRAM;
    }
    else if (addr >= mem_eram_start && addr < mem_eram_end) {
        return MemorySection::ERAM;
    }
    else if (addr >= mem_wram0_start && addr < mem_wram0_end) {
        return MemorySection::WRAM0;
    }
    else if (addr >= mem_wram1_start && addr < mem_wram1_end) {
        return MemorySection::WRAM1;
    }
    else if (addr >= mem_oam_start && addr < mem_oam_end) {
        return MemorySection::OAM;
    }
    else if (addr >= mem_unusable_start && addr < mem_unusable_end) {
        return MemorySection::UNSUABLE;
    }
    else if (addr >= mem_io_start && addr < mem_io_end) {
        return MemorySection::IO;
    }
    else if (addr >= mem_hram_start && addr < mem_hram_end) {
        return MemorySection::HRAM;
    }
    else { // reg_ie
        return MemorySection::IO;
    }
}

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

struct Cpu;

struct Cartridge_Header {
    array<u8, 1 + 0x103 - 0x100> entry_point;
    array<u8, 1 + 0x133 - 0x104> nintendo_logo;
    array<u8, 1 + 0x143 - 0x134> title;
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

static constexpr u8 select_high_bits_off = 5;

struct Memory {
    array<u8, 0x10000 - (mem_switchable_rom_end - mem_switchable_rom_start) - (mem_eram_end - mem_eram_start)> internal;
    struct Rom {
        Memory* mem;

        int fildes;
        u8* buf;
        size_t buf_size;

        u8* bank;

        int
        init(Memory* mem_, const char* file_path)
        {
            mem = mem_;
            if ((fildes = open(file_path, O_RDONLY)) < 0)
                return 1;
            buf_size = lseek(fildes, 0L, SEEK_END);
            lseek(fildes, 0L, SEEK_SET);
            if (!(buf = (u8*)mmap(nullptr, buf_size, PROT_READ, MAP_PRIVATE, fildes, 0L)))
                return 1;

            update_bank();
            return 0;
        }

        inline u8&
        operator[](int i)
        {
            return bank[i];
        }

        inline u8
        operator[](int i) const
        {
            return bank[i];
        }

        void
        update_bank()
        {
            auto bank_idx = mem->select_high_bits | mem->select_low_bits;
            if ((mem->select_low_bits & ~(1 << select_high_bits_off)) == 0x0)
                bank_idx = 0x0;

            assert(bank_idx < mem->rom_bank_count());
            bank = buf + bank_idx * 0x4000;
            LOG("bank_idx: " << bank_idx);
        }
    } rom;
    u8* eram;
    u8* eram_start;

    Cartridge_Header cart;

    u8 select_high_bits {}, select_low_bits {};
    bool ram_enabled;
    enum class Select_Mode { UNKNOWN, ROM, RAM } select_mode;

    auto
    rom_bank_count() -> u8
    {
        return (cart.rom_size ? 2 << cart.rom_size : 1);
    }

    int
    init(const char* file_path)
    {
        if (rom.init(this, file_path))
            return 1;

        cart = *((Cartridge_Header*)(&rom.buf[0x100]));
        printf("sgb_flag: %x\ncart type: %x, rom size: %x, ram size: %x\n",
               cart.sgb_flag,
               cart.cartridge_type,
               cart.rom_size,
               cart.ram_size);

        if (!(eram_start = (u8*)malloc(mem_eram_end - mem_eram_start)))
            return 1;

        eram = eram_start;

        for (size_t i = 0; i < mem_rom_end; i++) internal[i] = rom.buf[i];

        for (size_t i = mem_vram_start; i < std::min(rom.buf_size, static_cast<size_t>(0x10000)); i++)
            if (!(i >= mem_unusable_start && i < mem_unusable_end))
                get_ref(i) = rom.buf[i];

        return 0;
    }

    ~Memory()
    {
        munmap(rom.buf, rom.buf_size);
        close(rom.fildes);
        free(eram_start);
    }

    u8&
    get_ref(u16 addr)
    {

        // extern string info(Cpu & cpu, Memory & mem, u16 pc);
        // extern u16 inst_pc;
        // extern Cpu cpu;
        // if (addr == 0xde06)
        // std::cout << info(cpu, *this, inst_pc) << "\n>>>";

        switch (addr_to_section(addr)) {
        case MemorySection::ROM: {
            return internal.at(addr);
        } break;
        case MemorySection::SWITCHABLE_ROM: {
            return rom[addr - mem_switchable_rom_start];
        } break;
        case MemorySection::VRAM: {
            constexpr auto hole_size = mem_switchable_rom_end - mem_switchable_rom_start;
            return internal.at(addr - hole_size);
        } break;
        case MemorySection::ERAM: {
            return eram[addr - mem_eram_start];
        } break;
        case MemorySection::WRAM0:
        case MemorySection::WRAM1:
        case MemorySection::OAM:
        case MemorySection::IO:
        case MemorySection::HRAM: {
            constexpr auto hole_size = (mem_switchable_rom_end - mem_switchable_rom_start) + (mem_eram_end - mem_eram_start);
            return internal.at(addr - hole_size);
        } break;
        case MemorySection::UNSUABLE: {
            constexpr auto hole_size = (mem_switchable_rom_end - mem_switchable_rom_start) + (mem_eram_end - mem_eram_start);
            // printf("(Warning) mem.get: unusable memory\n");
            return internal.at(addr - hole_size);
        } break;
        }
    }

    u8
    get(u16 addr) const
    {
        switch (addr_to_section(addr)) {
        case MemorySection::ROM: {
            return internal.at(addr);
        } break;
        case MemorySection::SWITCHABLE_ROM: {
            return rom[addr - mem_switchable_rom_start];
        } break;
        case MemorySection::VRAM: {
            constexpr auto hole_size = mem_switchable_rom_end - mem_switchable_rom_start;
            return internal.at(addr - hole_size);
        } break;
        case MemorySection::ERAM: {
            return eram[addr - mem_eram_start];
        } break;
        case MemorySection::WRAM0:
        case MemorySection::WRAM1:
        case MemorySection::OAM:
        case MemorySection::IO:
        case MemorySection::HRAM: {
            constexpr auto hole_size = (mem_switchable_rom_end - mem_switchable_rom_start) + (mem_eram_end - mem_eram_start);
            return internal.at(addr - hole_size);
        } break;
        case MemorySection::UNSUABLE: {
            return 0;
            constexpr auto hole_size = (mem_switchable_rom_end - mem_switchable_rom_start) + (mem_eram_end - mem_eram_start);
            printf("(Warning) mem.get: unusable memory\n");
            return internal.at(addr - hole_size);
        } break;
        }
    }

public:
    // Bit 5 - P15 Select Button Keys      (0=Select)
    // Bit 4 - P14 Select Direction Keys   (0=Select)
    // Bit 3 - P13 Input Down  or Start    (0=Pressed) (Read Only)
    // Bit 2 - P12 Input Up    or Select   (0=Pressed) (Read Only)
    // Bit 1 - P11 Input Left  or Button B (0=Pressed) (Read Only)
    // Bit 0 - P10 Input Right or Button A (0=Pressed) (Read Only)
    enum struct Joypad : u8 { RIGHT = 0, LEFT, UP, DOWN, A, B, SELECT, START, JOYPAD_COUNT, DEBUGGER_VRAM };
    u8 jp;

    u8
    read_byte(u16 addr) const
    {
        if (addr >= mem_vram_start && addr < mem_vram_end) { // vram
            const Screen_Mode lcd_mode = (Screen_Mode)(get(reg_stat) & 0b11);
            if (get_bit(get(reg_lcdc), 7) && // disallow read from vram in * mode
                lcd_mode == Screen_Mode::TRANSFARING) {
                // printf("Warning: attempt to read from VRAM at mode %u\n", (u8)lcd_mode);
                // return 0xff;
            }
        }
        else if (addr >= mem_oam_start && addr < mem_oam_end) {
            const Screen_Mode lcd_mode = (Screen_Mode)(get(reg_stat) & 0b11);
            if (get_bit(get(reg_lcdc), 7) && // disallow reads from oam in OAM mode
                (lcd_mode == Screen_Mode::OAM || lcd_mode == Screen_Mode::TRANSFARING)) {
                // printf("Warning: attempt to read from OAM at mode %u\n", (u8)lcd_mode);
                return 0xff;
            }
        }
        else if ((addr == 0xffff) || (addr >= mem_io_start && addr < mem_io_end)) { // io
            if (addr == reg_jp) {
                bool select_dir_keys = get_bit(get(reg_jp), 4);
                bool select_buttom_keys = get_bit(get(reg_jp), 5);
                if (!(select_dir_keys ^ select_buttom_keys))
                    printf("Warnning: !(select_dir_keys ^ select_buttom_keys)\n");
                if (select_dir_keys)
                    return (get(reg_jp) & 0xf0) | ((jp >> 4) & 0x0f);
                else if (select_buttom_keys)
                    return (get(reg_jp) & 0xf0) | (jp & 0x0f);
            }
        }

        return get(addr);
    }

    u16
    read_word(u16 addr) const
    {
        return concat(read_byte(addr + 1), read_byte(addr));
    }

    void
    write_byte(u16 addr, u8 byte)
    {
        switch (addr_to_section(addr)) {
        case MemorySection::ROM:
        case MemorySection::SWITCHABLE_ROM: {
            if (cart.cartridge_type == 0)
                return;
            else if (cart.cartridge_type == 1) {
                if (addr < 0x2000) {  // register 0
                    if (byte == 0x0a) // enable RAM
                        ram_enabled = true;
                    else // https://retrocomputing.stackexchange.com/questions/11732/how-does-the-gameboys-memory-bank-switching-work
                        ram_enabled = false;
                    return;
                }
                else if (addr < 0x4000) { // register 1 - lower bits of ROM bank selection
                    select_low_bits = byte & ~(1 << select_high_bits_off);
                    fprintf(stderr, "write_byte: select_low_bits: %x\n", select_high_bits_off);
                    rom.update_bank();
                    return;
                }
                else if (addr < 0x6000) { // register 2 - higher bits of ROM / RAM bank selection
                    select_high_bits = byte & (0b11 << select_high_bits_off);
                    if (select_mode == Select_Mode::ROM)
                        rom.update_bank();
                    return;
                }
                else if (addr < 0x8000) { // register 3 - ROM bank selection mode
                    if (byte == 0) {
                        select_mode = Select_Mode::ROM;
                        rom.update_bank();
                    }
                    else if (byte == 1)
                        select_mode = Select_Mode::RAM;
                    printf("rom selection mode: %u\n", (u8)select_mode);
                    return;
                }
            }
            // dont write anyway
            return;
        } break;
        case MemorySection::VRAM: {
            const Screen_Mode lcd_mode = (Screen_Mode)(internal[reg_stat] & 0b11);
            if (get_bit(get(reg_lcdc), 7) && lcd_mode == Screen_Mode::TRANSFARING) {
                // assert(0);
                return;
            }
        } break;
        case MemorySection::ERAM: {
            if (!ram_enabled && false) // TODO: add ram banks
                printf("Warning: writing to eram which wasn't enabled\n");
        } break;
        case MemorySection::WRAM0: break;
        case MemorySection::WRAM1: break;
        case MemorySection::OAM: {
            const Screen_Mode lcd_mode = (Screen_Mode)(internal[reg_stat] & 0b11);
            if (get_bit(get(reg_lcdc), 7) && lcd_mode == Screen_Mode::OAM)
                return;
        } break;
        case MemorySection::IO: {
            switch (addr) {
            case reg_lcdc: {
                if (get_bit(get(reg_lcdc), 7) && !get_bit(byte, 7)) { // close lcd
                    assert((Screen_Mode)(get(reg_stat) & 0b11) == Screen_Mode::V_BLANK);
                }
            } break;
            case reg_stat: {
                byte = (byte & ~(0b111)) | (internal[reg_stat] & 0b111);
            } break;
            case reg_dma: {
                u16 src = (byte << 8);
                memcpy(&get_ref(mem_oam_start), &get_ref(src), mem_oam_end - mem_oam_start);
            } break;
            case reg_div: {
                byte = 0;
            } break;
            case 0xff4d: {
                // TODO: CPU speed switching
            } break;
            case reg_sc: {
                if (byte == 0x81) {
                    printf("%c", get(reg_sb));
                    fflush(stdout);
                }
            } break;
            // unchanged
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
            default: {
                if (addr >= 0xFF10 && addr <= 0xFF3f) // audio
                    break;
                // if (addr >= 0xFF00 && addr <= 0xFF7F) // disable other io?
                // printf("io addr write: 0x%x\n", addr);
            } break;
            }
        } break;
        case MemorySection::HRAM: break;
        case MemorySection::UNSUABLE: {
            break;
            printf("Warning1: writing to unusable memory\n");
        } break;
        }

        get_ref(addr) = byte;
        return;
    }

    void
    write_word(u16 addr, u16 word)
    {
        write_byte(addr, lower_byte(word));
        write_byte(addr + 1, word >> 8);
    }
};

extern Memory mem;
