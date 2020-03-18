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

array<RgbColor, 160>
render_line(const array<std::pair<Color, Color_Platte>, 160>& line)
{
    array<RgbColor, 160> result;

    for (int i = 0; i < 160; i++) {
        const auto& [color, cp] = line[i];
        u16 platte_register;
        switch (cp) {
        case Color_Platte::BG: platte_register = mem_bgp; break;
        case Color_Platte::OBP0: platte_register = mem_obp0; break;
        case Color_Platte::OBP1: platte_register = mem_obp1; break;
        }

        result[i] =
            platte[(mem.buf[platte_register] & (0b11 << (2 * color))) >> (2 * color)];
    }
    return result;
}

template <Color_Platte platte>
array<std::pair<Color, Color_Platte>, 8>
get_tile_line(const Memory& mem, u16 tile_addr, u8 line)
{
    array<std::pair<Color, Color_Platte>, 8> result;
    u8 ls_bits = mem.read_byte(tile_addr + 2 * line);
    u8 ms_bits = mem.read_byte(tile_addr + 2 * line + 1);
    for (int i = 0; i < 8; i++) {
        u8 color_num = ((ms_bits & 0x80) | ((ls_bits & 0x80) >> 1)) >> 6;
        result[i]    = { color_num, platte };
        ls_bits      = ls_bits << 1;
        ms_bits      = ms_bits << 1;
    }

    return result;
}

array<std::pair<Color, Color_Platte>, 160>
lcdc_bg_line(u8 lcdc, u8 top_pixel, u8 left_pixel)
{
    array<std::pair<Color, Color_Platte>, 160> result;
    if (!get_bit(lcdc, 0))
        std::fill(result.begin(),
                  result.end(),
                  std::pair<Color, Color_Platte>(platte[0], Color_Platte::BG));
    else {
        auto bg_tile_map = get_bit(lcdc, 3) ? 0x9c00 : 0x9800;
        for (int ix = 0; ix < 32; ix++) {
            u16 offset    = 32 * std::floor(top_pixel / 8) + (left_pixel + ix) % 32;
            u8  chr       = mem.buf[bg_tile_map + offset];
            u16 tile_addr = get_bit(lcdc, 4) ? 0x8000 + 16 * chr : 0x9000 + 16 * (s8)chr;
            // auto color_line =
            // get_tile_line<Color_Platte::BG>(mem, tile_addr, top_pixel % 8);
            // for (int tx = 0; tx < 8; tx++)
            // result[8 * ix + tx] = { color_line[tx], Color_Platte::BG };
            ((array<std::pair<Color, Color_Platte>, 8>*)result.data())[ix] =
                get_tile_line<Color_Platte::BG>(mem, tile_addr, top_pixel % 8);
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
    */
    enum Flags { OBP1 = 4, X_FLIP, Y_FLIP, BG_PRIORITY };
    u8 flags;
};

// array<Color, 160>
std::pair<array<u8, 10>, u8>
lcdc_obj_line(u8 lcdc, u8 top_pixel)
{
    const u8      sprite_size   = get_bit(lcdc, 2) ? 16 : 8;
    const Sprite* obj_atr_table = (Sprite*)&mem.buf[0xfe00];

    u8 sprite_ids[40];
    for (int i = 0; i < 40; i++) sprite_ids[i] = i;

    int il, ir;
    for (il = 0, ir = 39; il < ir;) {
        int  ir_top        = static_cast<int>(obj_atr_table[sprite_ids[ir]].y) - 16;
        bool ir_not_inline = (ir_top > top_pixel) || (ir_top + sprite_size < top_pixel);
        if (ir_not_inline) {
            ir--;
            continue;
        }

        int  il_top        = static_cast<int>(obj_atr_table[sprite_ids[il]].y) - 16;
        bool il_not_inline = (il_top > top_pixel) || (il_top + sprite_size < top_pixel);
        if (il_not_inline) {
            std::swap(sprite_ids[il++], sprite_ids[ir--]);
            continue;
        }

        il++;
    }

    if (il == ir) {
        int  ir_top        = static_cast<int>(obj_atr_table[sprite_ids[ir]].y) - 16;
        bool ir_not_inline = (ir_top > top_pixel) || (ir_top + sprite_size < top_pixel);
        if (ir_not_inline)
            ir--;
        else {
            int  il_top = static_cast<int>(obj_atr_table[sprite_ids[il]].y) - 16;
            bool il_not_inline =
                (il_top > top_pixel) || (il_top + sprite_size < top_pixel);
            if (il_not_inline)
                std::swap(sprite_ids[il++], sprite_ids[ir--]);
        }
    }

    auto comp = [=](u8 i1, u8 i2) {
        // move to right all sprites which arent in
        // the current scanline and give priority to left ones
        return (obj_atr_table[i1].x < obj_atr_table[i2].x) || //
               (obj_atr_table[i1].x == obj_atr_table[i2].x && i1 < i2);
    };

    std::nth_element(sprite_ids,
                     &sprite_ids[std::min(ir, 10)],
                     &sprite_ids[ir + 1],
                     comp);
    std::sort(sprite_ids, &sprite_ids[std::min(ir + 1, 10)], comp);
    return { *(array<u8, 10>*)sprite_ids, ir + 1 };
}

array<std::pair<Color, Color_Platte>, 160>
lcdc_render_obj(u8                                         lcdc,
                u8                                         iy,
                const std::pair<array<u8, 10>, u8>&        sprite_ids,
                array<std::pair<Color, Color_Platte>, 160> line)
{
    const auto    sprite_size   = get_bit(lcdc, 2) ? 16 : 8;
    const Sprite* obj_atr_table = (Sprite*)&mem.buf[0xfe00];

    for (int i = sprite_ids.second - 1; i >= 0; i--) {
        const auto& sprite    = obj_atr_table[sprite_ids.first[i]];
        u16         tile_addr = 0x8000 + 2 * sprite_size * sprite.tile;

        if (get_bit(sprite.flags, Sprite::OBP1))
            *((array<std::pair<Color, Color_Platte>, 8>*)&line[sprite.x - 8]) =
                get_tile_line<Color_Platte::OBP1>(mem, tile_addr, iy % sprite_size);
        else
            *((array<std::pair<Color, Color_Platte>, 8>*)&line[sprite.x - 8]) =
                get_tile_line<Color_Platte::OBP0>(mem, tile_addr, iy % sprite_size);

        auto& curr_line =
            *((array<std::pair<Color, Color_Platte>, 8>*)&line[sprite.x - 8]);
        if (get_bit(sprite.flags, Sprite::X_FLIP))
            std::reverse(curr_line.begin(), curr_line.end());
    }
    return line;
}
