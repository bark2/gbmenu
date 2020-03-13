#pragma once

#include "types.h"
#include <chrono>
#include <iostream>
#include <istream>

#include "mem.h"

#include "instruction.h"

constexpr u32 cpu_freq = 4 * 1048576;
using cc_duration      = std::chrono::duration<int, std::ratio<1, cpu_freq>>;

static inline u32
hz_to_cc(u32 freq)
{
    assert(cpu_freq > freq);
    return cpu_freq / freq;
}

struct Cpu {
    u32 cc;
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
    u8   last_cc;
    bool int_master_enable; // enable the jump to the interrupt vectors.
    bool halt;

    void boot_sequence(Memory& mem);

    enum class Interupt { VBLANK = 0, LCD_STAT, TIMER, SERIAL, JOYPAD };
    void set_interupt(Interupt i);

    enum Flag : u8 { C = 4, H = 5, N = 6, Z = 7 };
    u8   get_flag(Flag flag);
    void set_flag(u8 flag, bool value);
    void set_c_flag(bool value);
    void set_h_flag(bool value);
    void set_n_flag(bool value);
    void set_z_flag(bool value);

    bool handle_interupts();
    bool exec();

    friend std::ostream& operator<<(std::ostream& o, const Cpu& cpu);
    friend string        to_string(const Cpu& cpu);
};

extern Cpu cpu;
