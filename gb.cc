#include "cpu.h"
#include "gui.h"
#include "instruction.h"
#include "mem.h"
#include "ppu.h"
#include "types.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <unordered_map>

string
info(Cpu& cpu, Memory& mem, u16 pc = 0xffff)
{
    string result;
    if (pc == 0xffff)
        pc = cpu.pc;
    auto inst = (mem.buf[pc] == 0xcb ? prefix_cb_instructions[mem.buf[pc + 1]]
                                     : instructions[mem.buf[pc]]);

    result += to_string(cpu);
    // interupt registers
    result += "ie: " + conv_byte_hex(mem.buf[mem_ie]) +
              "  if: " + conv_byte_hex(mem.buf[mem_if]) +
              "  ime: " + (cpu.int_master_enable ? "1" : "0") + '\n';

    result += conv_word_hex(pc) + "[opcode: 0x";
    bool is_pcb    = mem.read_byte(pc) == 0xcb;
    u8   op_length = (is_pcb ? 2 : 1);
    for (int i = 0; i < op_length; i++) result += conv_byte_hex(mem.read_byte(pc + i));
    result += "]|\t" + (is_pcb ? prefix_cb_instructions[mem.read_byte(pc + 1)].mnemonic
                               : inst.mnemonic);
    if (inst.length - op_length == 1)
        result += string("\t$") + conv_byte_hex(mem.read_byte(pc + op_length));
    else if (inst.length - op_length == 2)
        result += string("\t$") + conv_word_hex(mem.read_word(pc + op_length));
    return result;
}

Memory mem;
Cpu    cpu;

// TODO:
// mark all used instructions in test and make sure they are currect.
// dont optimize cc for instructions
// more on z80 cc: https://8bitnotes.com/2017/05/z80-timing/
// add a gui to see the ram values

static struct {
    char command;
    char reg[3];
    u32  val;
    u16  line;
    bool reached = false;
} breakpoint;

void
cli()
{
    std::cout << info(cpu, mem) << "\n>>>";

    breakpoint.command = getchar();
    if (breakpoint.command != '\n')
        getchar();
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

int
main(int argc, char* argv[])
{
    // auto err = mem.init("../cpu_instrs_tests/individual/01-special.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/02-interrupts.gb");
    auto err = mem.init("../cpu_instrs_tests/individual/03-op sp,hl.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/04-op r,imm.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/05-op rp.gb");    // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/06-ld r,r.gb");   // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/07-jr,jp,call,ret,rst.gb"); //
    // auto err = mem.init("../cpu_instrs_tests/individual/08-misc instrs.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/09-op r,r.gb");   // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/10-bit ops.gb");  // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/11-op a,(hl).gb"); // daa
    // auto err = mem.init("../rom/tetris.gb");
    // auto err = mem.init("../rom/Dr. Mario (JU) (V1.1).gb");
    if (err)
        return 1;

    cpu.boot_sequence(mem);

    auto clock_begin =
        std::chrono::time_point_cast<cc_duration>(std::chrono::steady_clock::now());

    struct {
        u32                              last_cc;
        bool                             curr_line_finished;
        array<array<RgbColor, 160>, 144> pixels;
    } lcd {};

    std::chrono::time_point<std::chrono::steady_clock, cc_duration> last_cc = clock_begin;
    std::chrono::time_point<std::chrono::steady_clock, cc_duration> tima_last_cc =
        clock_begin;
    struct {
        u32 last_cc;
        u32 freq;
    } divider { 0, 16384 };

    std::atomic<bool> is_gui_alive { true };
    std::thread       gui(gui_start,
                          std::ref(is_gui_alive),
                          argc,
                          argv,
                          std::cref(lcd.pixels),
                          [](Memory::Joypad key) -> void {
                        set_bit(mem.jp, (u8)key, true);
                        cpu.set_interupt(Cpu::Interupt::JOYPAD);
                          });

    while (true) {
        if (is_gui_alive == false) {
            breakpoint.reached = true;
            if (gui.joinable())
                gui.join();
        }

        auto now =
            std::chrono::time_point_cast<cc_duration>(std::chrono::steady_clock::now());
        if ((now - last_cc).count() < cpu.last_cc) {
            continue;
        }
        last_cc = now;

        if (!strcmp(breakpoint.reg, "pc") && cpu.pc == breakpoint.val)
            breakpoint.reached = true;
        if (!strcmp(breakpoint.reg, "hl") && cpu.hl == breakpoint.val)
            breakpoint.reached = true;
        if (!strcmp(breakpoint.reg, "sp") && cpu.sp == breakpoint.val)
            breakpoint.reached = true;
        if (!strcmp(breakpoint.reg, "a") && cpu.a == breakpoint.val)
            breakpoint.reached = true;

        if (breakpoint.reached)
            cli();

        if (!cpu.halt) {
            do {
                if (cpu.handle_interupts())
                    break;
                if (bool pc = cpu.pc; cpu.exec()) {
                    breakpoint.reached = true;
                    std::cout << info(cpu, mem, pc) << "\n";
                }
            } while (false);
        }

        if (cpu.cc - divider.last_cc > hz_to_cc(divider.freq / 4)) {
            divider.last_cc = cpu.cc;
            mem.buf[mem_div]++;
        }

        if (get_bit(mem.buf[mem_tac], 2)) {
            u32 timer_freq;
            switch (mem.buf[mem_tac] & 0b11) {
            case 0b00: timer_freq = 4096; break;
            case 0b01: timer_freq = 262144; break;
            case 0b10: timer_freq = 65536; break;
            case 0b11: timer_freq = 16384; break;
            }

            if (static_cast<u32>((now - tima_last_cc).count()) > hz_to_cc(timer_freq)) {
                tima_last_cc = now;
                mem.buf[mem_tima]++;
                if (!mem.buf[mem_tima]) {
                    cpu.set_interupt(Cpu::Interupt::TIMER);
                    mem.buf[mem_tima] = mem.buf[mem_tma];
                }
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
            if (stat_ask_coincidence(stat))
                cpu.set_interupt(Cpu::Interupt::LCD_STAT);

            if (cpu.cc - lcd.last_cc > 114) {
                if (ly < 144) {
                    // stat hblank interrupt
                    if (stat_ask_hblank(stat))
                        cpu.set_interupt(Cpu::Interupt::LCD_STAT);

                    auto scx        = mem.buf[mem_scx];
                    auto scy        = mem.buf[mem_scy];
                    auto color_line = lcdc_bg_line(lcdc, (ly + scy) % 256, scx);
                    // if(status.window_display_enable) cgb_render_window();
                    if (lcdc_obj_enabled(lcdc))
                        color_line = lcdc_obj_line(lcdc, (ly + scy) % 256, color_line);
                    // does sprites and bg-tiles have different color pallate?
                    lcd.pixels[ly] = render_line(color_line);
                }
                else {
                    // vblank interupt
                    if (ly == 144) {
                        cpu.set_interupt(Cpu::Interupt::VBLANK);
                        // stat vblank interrupt
                        if (stat_ask_vblank(stat))
                            cpu.set_interupt(Cpu::Interupt::LCD_STAT);
                    }
                }

                mem.buf[mem_ly] = (ly + 1) % 154;
                lcd.last_cc     = cpu.cc;
            } // cpu.cc - lcd.last_cc > line cc

            mem.buf[mem_stat] = stat;
        } // lcd is enabled

        cpu.flags &= 0xf0;
        mem.buf[mem_if] &= 0x1f;
        mem.buf[mem_ie] &= 0x1f;
    } // while

    return 0;
}
