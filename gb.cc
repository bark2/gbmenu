#include "cpu.h"
#include "instruction.h"
#include "mem.h"
#include "ppu.h"
#include "types.h"
#include <QApplication>
#include <iostream>
#include <thread>

array<array<Color, 160>, 144> get_background(const Memory& mem, u8 top, u8 left);

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

// TODO: add a gui to see the ram values

int
pow(int a, int n)
{
    return n == 0 ? 1 : a * pow(a, n - 1);
}

u16
hex_str_to_num(char str[4])
{
    u16 result;
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
    for (int i = 0; i < op_length; i++) result += conv_byte_hex(mem.read_byte(inst_addr + i));
    result += "]|\t" +
              (is_pcb ? prefix_cb_instructions[mem.read_byte(inst_addr + 1)].mnemonic : inst.mnemonic);
    if (inst.length - op_length == 1)
        result += string("\t$") + conv_byte_hex(mem.read_byte(inst_addr + op_length));
    else if (inst.length - op_length == 2)
        result += string("\t$") + conv_word_hex(mem.read_word(inst_addr + op_length));
    return result;
}

int
main(int argc, char* argv[])
{
    Memory mem;
    auto   err = mem.init("../cpu_instrs_tests/individual/04-op r,imm.gb");
    if (err) return 1;

    Cpu cpu;
    cpu.boot_sequence(mem);

    std::atomic<bool> is_gui_alive { true };
    std::thread       ppu(ppu_init, std::ref(is_gui_alive), argc, argv, std::ref(mem), std::ref(cpu),
                    std::ref(lcd));

    Printable printed;
    struct {
        char s[5];
        u16  line;
        bool reached = false;
    } breakpoint;
    // printf("set pc breakpoint: ");
    // scanf("%s", breakpoint.s);
    // fflush(stdin);
    // breakpoint.line = hex_str_to_num(breakpoint.s);

    // breakpoint.reached = true;
    while (true) {
        printed.clear();
        if (is_gui_alive == false) breakpoint.reached = true;

        u16  inst_addr = cpu.pc;
        auto inst      = cpu.parse_next_inst(mem);
        if (breakpoint.reached) {
            printed += info(cpu, mem, inst, inst_addr);
            printed();
            getchar();
        }
        u8 cc = inst.fun(cpu, mem);

        constexpr u32 vertical_refresh_clocks = 70224; // dbg mode
        if (cpu.cc() - init_time > vertical_refresh_clocks) { lcd = get_background(); }
    }

    return 0;
}
