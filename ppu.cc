#include "ppu.h"
#include "cpu.h"
#include "mem.h"

#include <algorithm>
#include <array>

#include <chrono>
#include <thread>

using std::array;

static array<u32, 4> platte = { 0xc4f0c2, 0x5ab9a8, 0x1e606e, 0x2d1b00 };

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

using Tile = array<array<Color, 8>, 8>;

array<Color, 8>
get_bg_tile_line(const Memory& mem, u8 lcdc, u8 tile, u8 line)
{
    array<Color, 8> result;
    auto tile_addr = get_bit(lcdc, 4) ? 0x8000 + 16 * tile : 0x9000 + 16 * (s8)tile;
    u8   ls_bits   = mem.read_byte(tile_addr + 2 * line);
    u8   ms_bits   = mem.read_byte(tile_addr + 2 * line + 1);
    for (int i = 0; i < 8; i++) {
        u8    shade     = ((((ms_bits >> i) & 1) << 1) | ((ls_bits >> i) & 1));
        u8    color_num = mem.buf[mem_bgp] & (0b11 << (2 * shade)) >> (2 * shade);
        Color color     = platte[color_num];
        result[i]       = color;
    }

    return result;
}

array<Color, 160>
get_bg_line(const Memory& mem, u8 lcdc, u8 top_pixel, u8 left_pixel)
{
    array<Color, 160> result;
    auto              is_bg_enabled = get_bit(lcdc, 0);
    auto              bg_tile_map   = get_bit(lcdc, 3) ? 0x9c00 : 0x9800;
    // for now background only
    auto is_window_enabled = get_bit(lcdc, 5);
    // auto is_obj_enabled    = get_bit(lcdc, 1);
    // auto window_tile_map   = get_bit(lcdc, 6) ? 0x9c00 : 0x9900;

    if (!is_bg_enabled)
        std::fill(result.begin(), result.end(), platte[0]);
    else {
        array<Color, 256> data_line;
        u8                tile_y = top_pixel / 8;
        for (int i = 0; i < 32; i++) {
            u8 chr = mem.buf[bg_tile_map + 32 * tile_y + i];
            ((array<Color, 8>*)data_line.data())[i] =
                get_bg_tile_line(mem, lcdc, chr, top_pixel % 8);
        }

        for (int i = 0; i < 160; i++) result[i] = data_line[(left_pixel + i) % 256];
    }

    return result;
}

array<Color, 256>
get_bg(u8 lcdc, u8 iy)
{
    array<Color, 256> result;
    std::fill(result.begin(), result.end(), platte[0]);

    auto bg_tile_map = get_bit(lcdc, 3) ? 0x9c00 : 0x9800;
    for (int ix = 0; ix < 32; ix++) {
        u8 offset                             = 32 * iy / 8 + ix;
        u8 chr                                = mem.buf[bg_tile_map + offset];
        ((array<Color, 8>*)result.data())[ix] = get_bg_tile_line(mem, lcdc, chr, iy % 8);
    }

    return result;
}
