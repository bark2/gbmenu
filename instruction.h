#pragma once

#include "types.h"

struct Instruction {
    u8 length;
    string mnemonic;
    u8 (*fun)();
};

u8 illegal();

extern Instruction prefix_cb_instructions[], instructions[];
