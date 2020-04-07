#include "cpu.h"
#include "gui_.h"
#include "instruction.h"
#include "mem.h"
#include "ppu.h"
#include "types.h"

#include "time.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <qcoreevent.h>
#include <thread>
#include <vector>

#include <QApplication>
#include <QEvent>

string
info(Cpu& cpu, Memory& mem, u16 pc = 0xffff)
{
    string result;
    if (pc == 0xffff)
        pc = cpu.pc;

    bool is_pcb = mem.buf[pc] == 0xcb;
    auto inst = (is_pcb ? prefix_cb_instructions[mem.buf[pc + 1]] : instructions[mem.buf[pc]]);

    result += to_string(cpu);
    // interupt registers
    result += "ie: " + conv_byte_hex(mem.buf[reg_ie]) + "  if: " + conv_byte_hex(mem.buf[reg_if]) +
              "  ime: " + (cpu.int_master_enable ? "1" : "0") + '\n';
    result += "lcdc: " + conv_byte_hex(mem.buf[reg_lcdc]) + "  stat: " + conv_byte_hex(mem.buf[reg_stat]) +
              "  ly: " + conv_byte_hex(mem.buf[reg_ly]) + '\n';
    result += conv_word_hex(pc) + "[opcode: 0x";
    u8 op_length = (is_pcb ? 2 : 1);
    for (int i = 0; i < op_length; i++) result += conv_byte_hex(mem.read_byte(pc + i));
    result += "]|\t" + (is_pcb ? prefix_cb_instructions[mem.read_byte(pc + 1)].mnemonic : inst.mnemonic);
    if (inst.length - op_length == 1)
        result += string("\t$") + conv_byte_hex(mem.read_byte(pc + op_length));
    else if (inst.length - op_length == 2)
        result += string("\t$") + conv_word_hex(mem.read_word(pc + op_length));
    return result;
}

Memory mem;
Cpu cpu;

// TODO:
// mark all used instructions in test and make sure they are currect.
// dont optimize cc for instructions
// more on z80 cc: https://8bitnotes.com/2017/05/z80-timing/
// add a gui to see the ram values

Breakpoint breakpoint;

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
bool print_ = false;

int
main(int argc, char* argv[])
{
    // auto err = mem.init("../cpu_instrs_tests/individual/01-special.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/02-interrupts.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/03-op sp,hl.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/04-op r,imm.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/05-op rp.gb");    // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/06-ld r,r.gb");   // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/07-jr,jp,call,ret,rst.gb"); //
    // auto err = mem.init("../cpu_instrs_tests/individual/08-misc instrs.gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/09-op r,r.gb");   // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/10-bit ops.gb");  // ok
    // auto err = mem.init("../cpu_instrs_tests/individual/11-op a,(hl).gb"); // ok
    // auto err = mem.init("../cpu_instrs_tests/cpu_instrs.gb"); // ok

    // auto err = mem.init("../rom/tetris.gb");
    // auto err = mem.init("../rom/pokemon-blue.gb");
    // auto err = mem.init("../rom/bubble bobble (u) (gb) [h1].gb");
    auto err = mem.init("../rom/Dr. Mario (JU) (V1.1).gb");

    if (err)
        return 1;

    cpu.boot_sequence(mem);

    struct {
        u32 last_cc;
    } timer {};
    struct {
        u32 last_cc;
        u32 freq;
    } divider { {}, cpu_freq / 256 };

    struct {
        u32 cycles;
    } lcd {};

    Gui gui { .alive = true,
              .pause = false,
              .draw_oam = {},
              .update_lcd = {},
              .update_oam = {},
              .lcd = {},
              .oam = {},
              .app = nullptr,
              .Gui_Update_Event = {} };

    gui.Gui_Update_Event = (QEvent::Type)QEvent::registerEventType();
    std::thread gui_thread(gui_start, argc, argv, std::ref(gui));

    auto last_line_draw_time = now();
    std::pair<array<u8, 10>, u8> sprite_ids;
    while (true) {
        if (!gui.alive) {
            breakpoint.reached = true;
            if (gui_thread.joinable())
                gui_thread.join();
        }

        // if (gui.pause)
        // breakpoint.reached = true;
        if (!strcmp(breakpoint.reg, "pc") && cpu.pc == breakpoint.val)
            breakpoint.reached = true;
        else if (!strcmp(breakpoint.reg, "pc") && cpu.pc == breakpoint.val)
            breakpoint.reached = true;
        else if (!strcmp(breakpoint.reg, "sp") && cpu.sp == breakpoint.val)
            breakpoint.reached = true;
        else if (!strcmp(breakpoint.reg, "a") && cpu.a == breakpoint.val)
            breakpoint.reached = true;
        else if (!strcmp(breakpoint.reg, "ly") && mem.buf[reg_ly] == breakpoint.val)
            breakpoint.reached = true;

        if (breakpoint.reached)
            cli();

        // if (cpu.stop) {
        //     Time_Point last = now();
        //     while (cpu.stop) {
        //         auto dt = Cycle_Duration(2 * 4 * 114) - (now() - last);
        //         std::this_thread::sleep_for(dt);
        //         last = now();
        //     }
        //     cpu.cycles += 217;
        // }
        // else
        if (cpu.halt) {
            cpu.cycles++;
        }
        else if (cpu.handle_interupts()) {
            cpu.cycles++;
            cpu.last_inst_cycle = 1;
        }
        else {
            if (cpu.exec())
                ;
            // breakpoint.reached = true; // break on illegal instruction
            // std::cout << info(cpu, mem) << "\n>>>";
        }

        // hardware
        run_with_freq(divider, divider.freq, [&]() { mem.buf[reg_div]++; });

        if (get_bit(mem.buf[reg_tac], 2)) {
            u32 timer_freq;
            switch (mem.buf[reg_tac] & 0b11) {
            case 0b00: timer_freq = cpu_freq / 1024; break;
            case 0b01: timer_freq = cpu_freq / 16; break;
            case 0b10: timer_freq = cpu_freq / 64; break;
            case 0b11: timer_freq = cpu_freq / 256; break;
            }

            run_with_freq(timer, timer_freq, []() {
                mem.buf[reg_tima]++;
                if (!mem.buf[reg_tima]) {
                    mem.buf[reg_tima] = mem.buf[reg_tma];
                    cpu.set_interupt(Cpu::Interupt::TIMER);
                }
            });
        }

        auto lcdc = mem.buf[reg_lcdc];
        if (true || lcdc_is_enabled(lcdc)) {
            while (cpu.cycles >= lcd.cycles) {
                u8 stat = mem.buf[reg_stat];
                Screen_Mode mode = (Screen_Mode)(stat & 0b11);
                const u8 ly = mem.buf[reg_ly];
                const u8 lyc = mem.buf[reg_lyc];

                // coincidence
                const bool coincidence = (ly == lyc);
                set_bit(stat, 2, coincidence);
                if (stat_ask_coincidence(stat))
                    cpu.set_interupt(Cpu::Interupt::LCD_STAT);

                if (ly < 144) {
                    const u8 scx = mem.buf[reg_scx];
                    const u8 scy = mem.buf[reg_scy];

                    assert(mode != Screen_Mode::V_BLANK);
                    switch (mode) {
                    case Screen_Mode::OAM: {
                        if (!(lcd.cycles % (114 * 4)))
                            sprite_ids = lcdc_sprite_line(lcdc, (ly + scy) % 256);
                        lcd.cycles += 2;
                        if (lcd.cycles % (114 * 4) == 80)
                            mode = Screen_Mode::TRANSFARING;
                    } break;
                    case Screen_Mode::TRANSFARING: {
                        if (lcd.cycles % (114 * 4) == 80) {
                            auto color_line = lcdc_bg_line(lcdc, (ly + scy) % 256, scx);
                            if (lcdc_window_enabled(lcdc)) {
                                const u8 wx = mem.buf[reg_wx];
                                const u8 wy = mem.buf[reg_wy];
                                // if (!(wx >= 7 && wx <= 166 && wy <= 143))
                                // printf("Warning: !(wx >= 7 && wx <= 166 && wy <= 143), wx: %u, wy: %u\n", wx, wy);
                                lcdc_window_line(lcdc, ly + wy, wx - 7, color_line);
                            }
                            if (lcdc_obj_enabled(lcdc))
                                color_line = lcdc_render_sprites(lcdc, (ly + scy) % 256, sprite_ids, color_line);
                            gui.lcd[ly] = render_line(color_line);
                        }
                        lcd.cycles += 2;
                        auto transfer_cycles = static_cast<u32>(168 + 10 * sprite_ids.second);
                        assert(transfer_cycles < (114 * 4));
                        if (lcd.cycles % (114 * 4) == transfer_cycles) {
                            mode = Screen_Mode::H_BLANK;
                            if (stat_ask_hblank(stat))
                                cpu.set_interupt(Cpu::Interupt::LCD_STAT);
                        }
                    } break;
                    case Screen_Mode::H_BLANK: {
                        lcd.cycles += 2;
                        if (!(lcd.cycles % (114 * 4))) {
                            mode = (ly == 143 ? Screen_Mode::V_BLANK : Screen_Mode::OAM);
                            if (mode == Screen_Mode::V_BLANK) {
                                // vblank interupt
                                cpu.set_interupt(Cpu::Interupt::V_BLANK);
                                // stat vblank interrupt
                                if (stat_ask_vblank(stat))
                                    cpu.set_interupt(Cpu::Interupt::LCD_STAT);
                            }
                            if (mode == Screen_Mode::OAM) {
                                if (stat_ask_oam(stat))
                                    cpu.set_interupt(Cpu::Interupt::LCD_STAT);
                            }
                            mem.buf[reg_ly] = (ly + 1) % 154;

                            if (!(ly % 2)) {
                                // update gui - 30fps
                                gui.update_lcd = true;
                                // sync to real time - 30fps
                                auto dt = Cycle_Duration(2 * 4 * 114) - (now() - last_line_draw_time);
                                std::this_thread::sleep_for(dt);
                                last_line_draw_time = now();
                            }
                        }
                    } break;
                    }
                }
                else if (ly == 144) {
                    assert(mode == Screen_Mode::V_BLANK);
                    if (!(lcd.cycles % (114 * 4))) {
                        if (stat_ask_vblank(stat))
                            cpu.set_interupt(Cpu::Interupt::LCD_STAT);
                    }
                    lcd.cycles += 2;
                    if (!(lcd.cycles % (114 * 4))) {
                        mem.buf[reg_ly] = (ly + 1) % 154;

                        // update gui once a frame
                        if (gui.draw_oam) {
                            gui.update_oam = true;
                            const auto sprite_size = get_bit(mem.buf[reg_lcdc], 2) ? 16 : 8;
                            for (u8 sprite_idx = 0; sprite_idx < 40; sprite_idx++)
                                for (u8 li = 0; li < sprite_size; li++)
                                    gui.oam[sprite_idx][li] = render_tile_line(lcdc_render_sprite(sprite_size, sprite_idx, li));

                            // send event
                            if (gui.app) {
                                QApplication::postEvent(gui.app, new QEvent(gui.Gui_Update_Event));
                            }
                        }
                    }
                }
                else {
                    assert(ly > 144 && ly <= 153);
                    assert(mode == Screen_Mode::V_BLANK);
                    if (!(lcd.cycles % (114 * 4))) {
                        // interupt on the start of every line
                        if (stat_ask_vblank(stat))
                            cpu.set_interupt(Cpu::Interupt::LCD_STAT);
                    }
                    lcd.cycles += 2;
                    if (!(lcd.cycles % (114 * 4))) {
                        if (ly < 153)
                            mem.buf[reg_ly] = ly + 1;
                        else if (ly == 153 && lcd.cycles % 70224) {
                            continue;
                        }
                        else {
                            mem.buf[reg_ly] = 0;
                            mode = Screen_Mode::OAM;
                            if (stat_ask_oam(stat))
                                cpu.set_interupt(Cpu::Interupt::LCD_STAT);
                        }
                    }
                }
                mem.buf[reg_stat] = (stat & (~0b11)) | (u8)mode;
            }
        } // lcd is enabled

        cpu.flags &= 0xf0;
        mem.buf[reg_if] &= 0x1f;
        mem.buf[reg_ie] &= 0x1f;
    }
    return 0;
}
