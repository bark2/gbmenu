#include "cpu.h"
#include "gui.h"
#include "instruction.h"
#include "mem.h"
#include "ppu.h"
#include "types.h"

#include <atomic>
#include <cstring>
#include <iostream>
#include <thread>
#include <unordered_map>

string
info(Memory& mem, Instruction& inst, u16 inst_addr)
{
    string result;
    result += conv_word_hex(inst_addr) + "[opcode: 0x";
    bool is_pcb    = mem.read_byte(inst_addr) == 0xcb;
    u8   op_length = (is_pcb ? 2 : 1);
    for (int i = 0; i < op_length; i++)
        result += conv_byte_hex(mem.read_byte(inst_addr + i));
    result +=
        "]|\t" + (is_pcb ? prefix_cb_instructions[mem.read_byte(inst_addr + 1)].mnemonic
                         : inst.mnemonic);
    if (inst.length - op_length == 1)
        result += string("\t$") + conv_byte_hex(mem.read_byte(inst_addr + op_length));
    else if (inst.length - op_length == 2)
        result += string("\t$") + conv_word_hex(mem.read_word(inst_addr + op_length));
    return result;
}

string
info(Cpu& cpu, Memory& mem, Instruction& inst, u16 inst_addr)
{
    string result;
    result += to_string(cpu);
    result += conv_word_hex(inst_addr) + "[opcode: 0x";
    bool is_pcb    = mem.read_byte(inst_addr) == 0xcb;
    u8   op_length = (is_pcb ? 2 : 1);
    for (int i = 0; i < op_length; i++)
        result += conv_byte_hex(mem.read_byte(inst_addr + i));
    result +=
        "]|\t" + (is_pcb ? prefix_cb_instructions[mem.read_byte(inst_addr + 1)].mnemonic
                         : inst.mnemonic);
    if (inst.length - op_length == 1)
        result += string("\t$") + conv_byte_hex(mem.read_byte(inst_addr + op_length));
    else if (inst.length - op_length == 2)
        result += string("\t$") + conv_word_hex(mem.read_word(inst_addr + op_length));
    return result;
}

Memory mem;
Cpu    cpu;

// TODO:
// mark all used instructions in test and make sure they are currect.
// dont optimize cc for instructions
// more on z80 cc: https://8bitnotes.com/2017/05/z80-timing/
// add a gui to see the ram values
// FIXME:
// could read_byte(ld r, (a16)) read from register ? if so, this is a mistake.

int
main(int argc, char* argv[])
{
    // auto err = mem.init("../cpu_instrs_tests/individual/01-special.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/02-interrupts.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/03-op sp,hl.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/04-op r,imm.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/05-op rp.gb");    // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/06-ld r,r.gb");   // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/07-jr,jp,call,ret,rst.gb"); //
    // auto err = mem.init("../cpu_instrs_tests/individual/08-misc instrs.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/09-op r,r.gb");   // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/10-bit ops.gb");  // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/11-op a,(hl).gb"); // daa
    // auto err = mem.init("../rom/tetris.gb");
    auto err = mem.init("../rom/Dr. Mario (JU) (V1.1).gb");
    if (err) return 1;

    cpu.boot_sequence(mem);

    struct {
        u32                              last_cc;
        bool                             curr_line_finished;
        array<array<RgbColor, 160>, 144> pixels;
    } lcd {};
    struct {
        u32 last_cc;
        u32 freq;
    } divider { 0, 16384 };
    u32 tima_last_cc {};

    std::atomic<bool> is_gui_alive { true };
    std::thread       gui(gui_start,
                          std::ref(is_gui_alive),
                          argc,
                          argv,
                          std::cref(lcd.pixels),
                          [](Memory::Joypad key) -> void {
                        set_bit(mem.jp, (u8)key, true);
                        set_bit(mem.buf[mem_if], 4, true);
                          });

    struct {
        char command;
        char reg[3];
        u32  val;
        u16  line;
        bool reached = false;
    } breakpoint;

    while (true) {
        if (is_gui_alive == false) {
            breakpoint.reached = true;
            if (gui.joinable()) gui.join();
        }

        if (!strcmp(breakpoint.reg, "pc") && cpu.pc == breakpoint.val)
            breakpoint.reached = true;
        if (!strcmp(breakpoint.reg, "hl") && cpu.hl == breakpoint.val)
            breakpoint.reached = true;
        if (!strcmp(breakpoint.reg, "sp") && cpu.sp == breakpoint.val)
            breakpoint.reached = true;
        if (!strcmp(breakpoint.reg, "a") && cpu.a == breakpoint.val)
            breakpoint.reached = true;

        u16  inst_addr = cpu.pc;
        auto op        = mem.buf[cpu.pc++];
        auto inst =
            op == 0xcb ? prefix_cb_instructions[mem.buf[cpu.pc++]] : instructions[op];
        if (breakpoint.reached) {
            std::cout << info(cpu, mem, inst, inst_addr) << "\n>>>";

            breakpoint.command = getchar();
            if (breakpoint.command != '\n') getchar();
            switch (breakpoint.command) {
            case 'n': break;
            case 'c': {
                breakpoint.reached = false;
            } break;
            case 'b': {
                printf("breakpoint: ");
                scanf("%s %x", breakpoint.reg, &breakpoint.val);
                getchar();
                breakpoint.reached = false;
            } break;
            }
        }

        if (cpu.exec(inst.fun))
            ; // std::cout << info(mem, inst, inst_addr) << "\n";

        if (cpu.cc - divider.last_cc > hz_to_cc(divider.freq / 4)) {
            divider.last_cc = cpu.cc;
            mem.buf[mem_div]++;
        }

        auto timer_freq = []() {
            u32 freq;
            switch (mem.buf[mem_tac] & 0b11) {
            case 0b00: freq = 4096;
            case 0b01: freq = 262144;
            case 0b10: freq = 65536;
            case 0b11: freq = 16384;
            }
            return freq / 4;
        };

        if (get_bit(mem.buf[mem_tac], 2) &&
            (cpu.cc - tima_last_cc > hz_to_cc(timer_freq()))) {
            tima_last_cc = cpu.cc;
            mem.buf[mem_tima]++;
            if (!mem.buf[mem_tima]) {
                set_bit(mem.buf[mem_if], 2, true);
                mem.buf[mem_tima] = mem.buf[mem_tma];
            }
        }

        auto lcdc = mem.buf[mem_lcdc];
        if (lcdc_is_enabled(lcdc)) {
            auto stat = mem.buf[mem_stat];
            auto ly   = mem.buf[mem_ly];
            auto lyc  = mem.buf[mem_lyc];

            // coincidence
            bool coincidence = (ly == lyc);
            set_bit(stat, 2, coincidence);
            if (stat_ask_coincidence(stat)) set_bit(mem.buf[mem_if], 1, true);

            if (cpu.cc - lcd.last_cc > 114) {
                if (ly < 144) {
                    // stat hblank interrupt
                    // if (stat_ask_hblank(stat)) set_bit(mem.buf[mem_if], 1, true);

                    auto scx        = mem.buf[mem_scx];
                    auto scy        = mem.buf[mem_scy];
                    auto shade_line = lcdc_bg_line(lcdc, (ly + scy) % 256, scx);
                    // if(status.window_display_enable) cgb_render_window();
                    if (lcdc_obj_enabled(lcdc))
                        ;
                    // does sprites and bg-tiles have different color pallate?
                    lcd.pixels[ly] = render_line(shade_line);
                }
                else {
                    // vblank interupt
                    if (ly == 144) {
                        set_bit(mem.buf[mem_if], 0, true);

                        // stat vblank interrupt
                        if (stat_ask_vblank(stat)) set_bit(mem.buf[mem_if], 1, true);
                    }
                }

                mem.buf[mem_ly] = (ly + 1) % 154;
                lcd.last_cc     = cpu.cc;
            } // cpu.cc - lcd.last_cc > line cc

            mem.buf[mem_stat] = stat;
        } // lcd is enabled

        cpu.af = cpu.af & 0xfff0;
    } // while

    return 0;
}
