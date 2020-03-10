#pragma once

#include <atomic>

#include "types.h"

void gui_start(std::atomic<bool>&                 is_gui_alive,
               int                                argc,
               char*                              argv[],
               const array<array<u32, 160>, 144>& surface,
          void (*input_callback)(Memory::Joypad));
/* void gui_start(std::atomic<bool>&                 is_gui_alive, */
/* int                                argc, */
/* char*                              argv[], */
/* const array<array<u32, 256>, 256>& surface); */
