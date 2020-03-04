#include <atomic>
#include <iostream>
#include <thread>
#include <unordered_map>

#include "cpu.h"
#include "gui.h"
#include "instruction.h"
#include "mem.h"
#include "ppu.h"
#include "types.h"

struct Printable {
    string s;

    void
    operator()()
    {
        std::cout << s << std::endl;
    };
    void
    operator+=(string r)
    {
        s += r;
    };
    void
    clear()
    {
        s.clear();
    };
};

int
pow(int a, int n)
{
    return n == 0 ? 1 : a * pow(a, n - 1);
}

u16
hex_str_to_num(char str[4])
{
    u16 result = 0;
    for (int i = 3; i >= 0; i--) {
        int num;
        if (str[i] >= '0' && str[i] <= '9')
            num = (str[i] - '0');
        else if (str[i] >= 'a' && str[i] <= 'f')
            num = (str[i] - 'a' + 10);
        else {
            printf("char %c isnt recognized\n", str[i]);
            // fflush(stdout);
            assert(0);
        }
        result += pow(16, 3 - i) * num;
    }
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

int
main(int argc, char* argv[])
{
    // auto err = mem.init("../cpu_instrs_tests/individual/01-special.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/02-interrupts.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/03-op sp,hl.gb");
    auto err = mem.init("../cpu_instrs_tests/individual/04-op r,imm.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/05-op rp.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/06-ld r,r.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/07-jr,jp,call,ret,rst.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/08-misc instrs.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/09-op r,r.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/10-bit ops.gb");
    // auto err = mem.init("../cpu_instrs_tests/individual/11-op a,(hl).gb");
    if (err) return 1;

    cpu.boot_sequence(mem);

    struct {
        u32  last_cc;
        bool curr_line_finished;
        // array<array<Color, 160>, 144> pixels;
        array<array<Color, 256>, 256> pixels;
    } lcd;

    std::atomic<bool> is_gui_alive { true };
    // std::thread gui(gui_start, std::ref(is_gui_alive), argc, argv, std::cref(lcd.pixels));

    Printable printer;
    struct {
        char s[5];
        u16  line;
        bool reached = false;
    } breakpoint;

    // breakpoint.reached = true;
    while (true) {
        printer.clear();
        if (is_gui_alive == false) {
            breakpoint.reached = true;
            // gui.join();
        }
        // if (cpu.pc == 0xc6c7) breakpoint.reached = true;
        // if (cpu.pc == 0xDEF8) breakpoint.reached = true;

        u16  inst_addr = cpu.pc;
        auto op        = mem.buf[cpu.pc++];
        auto inst =
            op == 0xcb ? prefix_cb_instructions[mem.buf[cpu.pc++]] : instructions[op];
        if (breakpoint.reached) {
            printer += info(cpu, mem, inst, inst_addr);
            printer();
            getchar();
        }

        if (cpu.exec(inst.fun)) {
            printer += info(cpu, mem, inst, inst_addr);
            printer();
        }

        auto lcdc = mem.buf[mem_lcdc];
        if (lcd_is_enabled(lcdc)) {
            auto stat = mem.buf[mem_stat];
            auto ly   = mem.buf[mem_ly];
            auto lyc  = mem.buf[mem_lyc];

            // coincidence
            bool coincidence = ly == lyc;
            set_bit(stat, 2, coincidence);
            if (stat_ask_coincidence(stat)) set_bit(mem.buf[mem_if], 1, true);

            if (cpu.cc - lcd.last_cc > vertical_refresh_clocks) {
                lcd.last_cc = cpu.cc;

                if (ly < 144) {
                    // stat hblank interrupt
                    if (stat_ask_hblank(stat)) set_bit(mem.buf[mem_if], 1, true);

                    auto scx = mem.buf[mem_scx];
                    auto scy = mem.buf[mem_scy];
                    // lcd.pixels[ly] = get_bg(lcdc, ly);
                    // if(status.window_display_enable) cgb_render_window();
                    // if(status.ob_enable) cgb_render_ob();
                }
                else {
                    for (int iy = 0; iy < 256; iy++) lcd.pixels[iy] = get_bg(lcdc, iy);
                    // vblank interupt
                    if (ly == 144) {
                        set_bit(mem.buf[mem_if], 0, true);

                        // stat vblank interrupt
                        if (stat_ask_vblank(stat)) set_bit(mem.buf[mem_if], 1, true);
                    }
                }
                mem.buf[mem_ly] = (ly + 1) % 154;
            }
            mem.buf[mem_stat] = stat;
        }
        cpu.af = cpu.af & 0xfff0;
    }
    return 0;
}
