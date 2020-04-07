#pragma once

#include "bits.h"
#include "cpu.h"
// #include "mem.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <thread>

using Color = u8;
using RgbColor = u32;

constexpr u32 vertical_refresh_clocks = 70224; // dbg mode

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
lcdc_is_enabled(u8 lcdc)
{
    return get_bit(lcdc, 7);
}

static inline bool
lcdc_bg_enabled(u8 lcdc)
{
    return get_bit(lcdc, 0);
}

static inline bool
lcdc_obj_enabled(u8 lcdc)
{
    return get_bit(lcdc, 1);
}

static inline bool
lcdc_window_enabled(u8 lcdc)
{
    return get_bit(lcdc, 5);
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
stat_ask_coincidence(u8 stat)
{
    return get_bit(stat, 6);
}

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

//

array<Color, 160> lcdc_render_line(u8 lcdc, u8 top, u8 left, const array<u8, 10>& sprites);

std::pair<array<u8, 10>, u8> lcdc_sprite_line(u8 lcdc, u8 top_pixel);
auto lcdc_get_next_sprite(u8 lcdc, u8 top_pixel, u8 last_obj_idx) -> u8;

enum class Color_Platte { BG, OBP0, OBP1 };
array<std::pair<Color, Color_Platte>, 160> lcdc_bg_line(u8 lcdc, u8 top, u8 left);
auto lcdc_window_line(u8 lcdc, u16 top_pixel, u16 left_pixel, array<std::pair<Color, Color_Platte>, 160>& line) -> void;

array<RgbColor, 8> render_tile_line(const array<std::pair<Color, Color_Platte>, 8>& line);
array<RgbColor, 160> render_line(const array<std::pair<Color, Color_Platte>, 160>& line);

array<std::pair<Color, Color_Platte>, 8> lcdc_render_sprite(u8 lcdc, u8 sprite_idx, u8 iy);
auto lcdc_render_sprites(u8 lcdc,
                         u8 iy,
                         const std::pair<array<u8, 10>, u8>& sprite_ids,
                         array<std::pair<Color, Color_Platte>, 160>& line) -> array<std::pair<Color, Color_Platte>, 160>;

struct Object {
    u8 y;
    u8 x;
    u8 tile;
    /* Bit7   OBJ-to-BG Priority (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
       (Used for both BG and Window. BG color 0 is always behind OBJ)
       Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
       Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
       Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
       Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
       Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
    */
    enum Flags { OBP1 = 4, X_FLIP, Y_FLIP, BG_PRIORITY };
    u8 flags;
};
