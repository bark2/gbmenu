#include "ppu.h"
#include "cpu.h"
#include "mem.h"

#include <algorithm>
#include <array>
#include <cmath>

#include <chrono>
#include <thread>

using std::array;
constexpr array<RgbColor, 4> platte = { 0xc4f0c2, 0x5ab9a8, 0x1e606e, 0x2d1b00 };

/*
  LCD is 160x144 pixels

  0xff40 - lcdc
  Bit 7 - LCD Power           (0=Off, 1=On)
  Bit 6 - Window Tile Map     (0=9800h-9BFFh, 1=9C00h-9FFFh)
  Bit 5 - Window Enable       (0=Disabled, 1=Enabled)
  Bit 4 - BG & Window Tileset (0=8800h-97FFh, 1=8000h-8FFFh)
  Bit 3 - BG Tile Map         (0=9800h-9BFFh, 1=9C00h-9FFFh)
  Bit 2 - Sprite Size         (0=8×8, 1=8×16)
  Bit 1 - Sprites Enabled     (0=Disabled, 1=Enabled)
  Bit 0 - BG Enabled (in DMG) (0=Disabled, 1=Enabled)

  0xff44 - ly

  scanline takes 456 cc

  0xff41 - lcd status
*/

enum Screen_Mode {
    H_BLANK     = 0,
    V_BLANK     = 1,
    OAM         = 2,
    TRANSFARING = 3,
};

array<RgbColor, 8>
render_line(array<Color, 160> line)
{
    array<RgbColor, 8> result;
    std::transform(result.begin(), result.end(), line.begin(), [](auto shade) {
        return platte[shade];
    });
    return result;
}

array<Color, 8>
get_bg_tile_line(const Memory& mem, u16 tile_addr, u8 line)
{
    array<Color, 8> result;
    u8              ls_bits = mem.read_byte(tile_addr + 2 * line);
    u8              ms_bits = mem.read_byte(tile_addr + 2 * line + 1);
    for (int i = 0; i < 8; i++) {
        u8 shade = ((ms_bits & 0x80) << 1) | (ls_bits & 0x80);
        // u8 color_num = mem.buf[mem_bgp] & (0b11 << (2 * shade)) >> (2 * shade);
        result[i] = shade;
        ls_bits   = ls_bits << 1;
        ms_bits   = ms_bits << 1;
    }

    return result;
}

array<Color, 160>
lcdc_bg_line(u8 lcdc, u8 top_pixel, u8 left_pixel)
{
    array<Color, 160> result;
    if (!get_bit(lcdc, 0))
        std::fill(result.begin(), result.end(), platte[0]);
    else {
        auto bg_tile_map = get_bit(lcdc, 3) ? 0x9c00 : 0x9800;
        for (int ix = 0; ix < 32; ix++) {
            u16 offset    = 32 * std::floor(top_pixel / 8) + (left_pixel + ix) % 32;
            u8  chr       = mem.buf[bg_tile_map + offset];
            u16 tile_addr = get_bit(lcdc, 4) ? 0x8000 + 16 * chr : 0x9000 + 16 * (s8)chr;
            ((array<Color, 8>*)result.data())[ix] =
                get_bg_tile_line(mem, tile_addr, top_pixel % 8);
        }
    }
    return result;
}

struct Sprite {
    u8 y;
    u8 x;
    u8 tile;
    /*
       Bit7   OBJ-to-BG Priority (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
       (Used for both BG and Window. BG color 0 is always behind OBJ)
       Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
       Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
       Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
       Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
       Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
       ***
       render from right to left(left priority)
       only 10 at a time, the left most ones
    */
    u8 flags;
};

void
lcdc_obj_line(u8 lcdc, u8 top_pixel, array<Color, 160>& line)
{
    auto    sprite_size   = get_bit(lcdc, 2) ? 16 : 8;
    Sprite* obj_atr_table = (Sprite*)&mem.buf[0xfe00];

    u8 sprite_ids[40];
    for (int i = 0; i < 40; i++) sprite_ids[i] = i;

    std::make_heap(sprite_ids, sprite_ids + 40, [=](u8 i1, u8 i2) {
        // move to right all sprites which arent in the current scanline
        bool i2_out = (obj_atr_table[i2].y >= top_pixel &&
                       obj_atr_table[i2].y + sprite_size <= top_pixel);
        return i2_out || obj_atr_table[i1].x < obj_atr_table[i2].x ||
               (obj_atr_table[i1].x == obj_atr_table[i2].x && i1 < i2);
    });

    for (int i = 0; i < 30; i++) std::pop_heap(sprite_ids, sprite_ids + 40);

    for (int i = 0; i < 10; i++) {
        std::pop_heap(sprite_ids, sprite_ids + 40);
        auto si = sprite_ids[0];
        if (obj_atr_table[si].y != top_pixel) continue;
        u16 tile_addr = 0x8000 + 2 * sprite_size * obj_atr_table[si].tile;

        if (sprite_size == 8)
            ((array<Color, 8>*)&line[sprite_ids[si].x]) =
                get_obj_tile_line(mem, tile_addr, top_pixel % 8);
        else
            ((array<Color, 16>*)&line[sprite_ids[si].x]) =
                get_big_obj_tile_line(mem, tile_addr, top_pixel % 16);
    }
    return result;
}
