#pragma once

#include "types.h"
#include <chrono>
#include <iostream>
#include <istream>

#include "mem.h"

#include "instruction.h"

using namespace std::chrono;
constexpr u32 cpu_freq = 1048576;

struct Cpu {
    bool int_master_enable; // enable the jump to the interrupt vectors.
    u32  cc;
    u16  sp, pc;
    union {
        u8 registers[16];
        struct {
            union {
                u16 af;
                struct {
                    u8 flags; // 4 lsb always zero
                    u8 a;
                };
            };
            union {
                u16 bc;
                struct {
                    u8 c;
                    u8 b;
                };
            };
            union {
                u16 de;
                struct {
                    u8 e;
                    u8 d;
                };
            };
            union {
                u16 hl;
                struct {
                    u8 l;
                    u8 h;
                };
            };
        };
    };

    void boot_sequence(Memory& mem);

    // 0xff0f
    enum Int_t { JOYPAD = 0, SERIAL = 1, TIMER = 2, LCD_STAT = 4, VERTICAL_BLANK = 8 };
    u8   get_Ie(Memory& mem, Int_t i);
    void set_Ie(Memory& mem, Int_t i, bool val);

    // 0xffff
    void int_flag();

    u8   get_flag(u8 flag);
    void set_flag(u8 flag, bool value);
    u8   c_flag();
    void set_c_flag(bool value);
    u8   h_flag();
    void set_h_flag(bool value);
    u8   n_flag();
    void set_n_flag(bool value);
    u8   z_flag();
    void set_z_flag(bool value);

    friend std::ostream& operator<<(std::ostream& o, const Cpu& cpu);
    friend string        to_string(const Cpu& cpu);

    bool exec(u8 (*inst)());
};

extern Cpu cpu;
