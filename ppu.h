#pragma once

#include "bits.h"
#include "cpu.h"
#include "mem.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <thread>

using Color = u8;

void ppu_init(std::atomic<bool>& is_gui_alive, int argc, char* argv[], Memory& mem, Cpu& cpu,
              std::array<std::array<Color, 160>, 144>& lcd);

std::array<std::array<Color, 160>, 144> get_background(const Memory& mem, u8 top, u8 left);
