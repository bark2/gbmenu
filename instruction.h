#pragma once

#include "types.h"

struct Instruction {
    u8     length;
    string mnemonic;
    u8 (*fun)();
};

static inline u8
unimplemented()
{
    /* printf("Error: unimplemented instrution\n"); */
    return 100;
}

extern Instruction prefix_cb_instructions[], instructions[];

