#pragma once

#include "cpu.h"
#include "instructions/alu16.h"
#include "instructions/alu8.h"
#include "instructions/bits.h"
#include "instructions/call.h"
#include "instructions/jump.h"
#include "instructions/misc.h"
#include "instructions/transfer16.h"
#include "instructions/transfer8.h"
#include "mem.h"
#include "types.h"

struct Instruction {
    using InstFunc = u8 (*)(Cpu&, Memory&);
    u8     length;
    string mnemonic;

    InstFunc fun;
};

static inline u8
unimplemented(Cpu&, Memory&)
{
    std::cout << "Error: unimplemented instrution" << std::endl;
    assert(0);

    return 0;
}

extern Instruction prefix_cb_instructions[], instructions[];

static inline u8
prefix_cb(Cpu& cpu, Memory& mem)
{
    u8 op = mem.read_byte(cpu.pc++);
    return prefix_cb_instructions[op].fun(cpu, mem);
}
