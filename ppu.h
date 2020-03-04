#pragma once

#include "bits.h"
#include "cpu.h"
#include "mem.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <thread>

using Color = u32;

constexpr u32     vertical_refresh_clocks = 70224; // dbg mode
array<Color, 160> lcd_get_line(const Memory& mem, u8 top, u8 left);

// lcdc
// Bit 7 - LCD Display Enable             (0=Off, 1=On)
// Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
// Bit 5 - Window Display Enable          (0=Off, 1=On)
// Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
// Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
// Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
// Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
// Bit 0 - BG Display (for CGB see below) (0=Off, 1=On)

static inline bool
lcd_is_enabled(u8 lcdc)
{
    return get_bit(lcdc, 7);
}

static inline bool
lcd_bg_enabled(u8 lcdc)
{
    return get_bit(lcdc, 0);
}

// stat
// Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
// Bit 5 - Mode 2 OAM Interrupt         (1=Enable) (Read/Write)
// Bit 4 - Mode 1 V-Blank Interrupt     (1=Enable) (Read/Write)
// Bit 3 - Mode 0 H-Blank Interrupt     (1=Enable) (Read/Write)
// Bit 2 - Coincidence Flag  (0:LYC<>LY, 1:LYC=LY) (Read Only)
// Bit 1-0 - Mode Flag       (Mode 0-3, see below) (Read Only)
//           0: During H-Blank
//           1: During V-Blank
//           2: During Searching OAM-RAM
//           3: During Transfering Data to LCD Driver

static inline bool
stat_ask_oam(u8 stat)
{
    return get_bit(stat, 5);
}

static inline bool
stat_ask_vblank(u8 stat)
{
    return get_bit(stat, 4);
}

static inline bool
stat_ask_hblank(u8 stat)
{
    return get_bit(stat, 3);
}

static inline bool
stat_ask_coincidence(u8 stat)
{
    return get_bit(stat, 2);
}

//

array<Color, 8>   get_bg_tile_line(const Memory& mem, u8 lcdc, u8 chr, u8 line);
array<Color, 160> get_bg_line(const Memory& mem, u8 lcdc, u8 top, u8 left);
// array<Color, 256> get_all_bg_line(const Memory& mem, u8 lcdc, u8 top_pixel);
array<Color, 256> get_bg(u8 lcdc, u8 iy);
