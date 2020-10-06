#pragma once

#include "mem.h"
#include "types.h"
#include <chrono>
#include <iostream>
#include <istream>

// #include "mem.h"

#include "instruction.h"

struct Memory;

struct Cpu {
    u16 sp, pc;
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
    u32 cycles;
    u8 last_inst_cycle;
    bool int_master_enable; // enable the jump to the interrupt vectors.
    bool halt;

    bool stop;
    Time_Point stop_cc;

    void boot_sequence(Memory& mem);

    enum class Interupt { V_BLANK = 0, LCD_STAT, TIMER, SERIAL, JOYPAD };
    void set_interupt(Interupt i);

    enum Flag : u8 { C = 4, H = 5, N = 6, Z = 7 };
    u8 get_flag(Flag flag);
    void set_flag(u8 flag, bool value);
    void set_c_flag(bool value);
    void set_h_flag(bool value);
    void set_n_flag(bool value);
    void set_z_flag(bool value);

    bool handle_interupts();
    bool exec();

    friend std::ostream& operator<<(std::ostream& o, const Cpu& cpu);
    friend string to_string(const Cpu& cpu);
};

extern Cpu cpu;
