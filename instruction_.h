#pragma once

#include "types.h"
#include <cassert>
#include <iostream>

using Instruction_Unit = u8 (*)();
using Instruction_Seq  = array<Instruction_Unit, 3>;

struct Instruction {
    u8     length;
    string mnemonic;
    Instruction_Seq (*fun)();
};

static inline Instruction_Seq
unimplemented()
{
    return { []() -> u8 {
        std::cout << "Error: unimplemented instrution" << std::endl;
        assert(0);

        return 0;
    } };
}

extern Instruction prefix_cb_instructions[], instructions[];
