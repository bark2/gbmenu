#include "ppu.h"
#include "cpu.h"
#include "mem.h"

#include <algorithm>
#include <array>
#include <cmath>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>

using std::array;
constexpr array<RgbColor, 4> platte = { 0xc4f0c2, 0x5ab9a8, 0x1e606e, 0x2d1b00 };
// constexpr array<RgbColor, 4> platte = { 0xffffff, 0xb6b6b6, 0x676767, 0x000000 };

static inline u8
translate_color(std::pair<Color, Color_Platte> p)
{
    u16 platte_register;
    switch (p.second) {
    case Color_Platte::BG: platte_register = reg_bgp; break;
    case Color_Platte::OBP0: platte_register = reg_obp0; break;
    case Color_Platte::OBP1: platte_register = reg_obp1; break;
    }

    assert(p.first < 4);
    return (mem.buf[platte_register] & (0b11 << (2 * p.first))) >> (2 * p.first);
}

array<RgbColor, 8>
render_tile_line(const array<std::pair<Color, Color_Platte>, 8>& line)
{
    array<RgbColor, 8> result;

    for (int i = 0; i < 8; i++) result[i] = platte[translate_color(line[i])];
    return result;
}

array<RgbColor, 160>
render_line(const array<std::pair<Color, Color_Platte>, 160>& line)
{
    array<RgbColor, 160> result;
    for (int i = 0; i < 160; i++) { result[i] = platte[translate_color(line[i])]; }
    return result;
}

array<std::pair<Color, Color_Platte>, 8>
get_tile_line(const Memory& mem, u16 tile_addr, u8 line, Color_Platte platte)
{
    array<std::pair<Color, Color_Platte>, 8> result;
    u8 ls_bits = mem.read_byte(tile_addr + 2 * line);
    u8 ms_bits = mem.read_byte(tile_addr + 2 * line + 1);
    for (int i = 0; i < 8; i++) {
        u8 color_num = ((ms_bits & 0x80) | ((ls_bits & 0x80) >> 1)) >> 6;
        assert(color_num < 4);
        result[i] = { color_num, platte };
        ls_bits = ls_bits << 1;
        ms_bits = ms_bits << 1;
    }

    return result;
}

array<std::pair<Color, Color_Platte>, 160>
lcdc_bg_line(u8 lcdc, u8 top_pixel, u8 left_pixel)
{
    array<std::pair<Color, Color_Platte>, 160> result;
    if (!get_bit(lcdc, 0))
        std::fill(result.begin(), result.end(), std::pair<Color, Color_Platte>(0, Color_Platte::BG));
    else {
        auto bg_tile_map = get_bit(lcdc, 3) ? 0x9c00 : 0x9800;
        for (int ix = 0; ix < 32; ix++) {
            u16 offset = 32 * std::floor(top_pixel / 8) + (left_pixel + ix) % 32;
            u8 chr = mem.buf[bg_tile_map + offset];
            u16 tile_addr = get_bit(lcdc, 4) ? 0x8000 + 16 * chr : 0x9000 + 16 * (s8)chr;
            ((array<std::pair<Color, Color_Platte>, 8>*)result.data())[ix] =
                get_tile_line(mem, tile_addr, top_pixel % 8, Color_Platte::BG);
        }
    }
    return result;
}

// TODO: check if u8 overflow
auto
lcdc_window_line(u8 lcdc, u16 top_pixel, u16 left_pixel, array<std::pair<Color, Color_Platte>, 160>& line) -> void
{
    if (left_pixel >= 160 || top_pixel >= 144)
        return;

    auto window_tile_map = get_bit(lcdc, 6) ? 0x9c00 : 0x9800;
    for (int ix = 0; ix < 32; ix++) {
        if (left_pixel + ix >= 32)
            break;

        u16 offset = 32 * std::floor(top_pixel / 8) + (left_pixel + ix);
        s8 chr = (s8)mem.buf[window_tile_map + offset];
        u16 tile_addr = 0x9000 + 16 * chr;
        auto window_line = get_tile_line(mem, tile_addr, top_pixel % 8, Color_Platte::BG);
        ((array<std::pair<Color, Color_Platte>, 8>*)line.data())[ix] = window_line;
    }
}

std::pair<array<u8, 10>, u8>
lcdc_sprite_line(u8 lcdc, u8 top_pixel)
{
    const u8 sprite_size = get_bit(lcdc, 2) ? 16 : 8;
    const Object* obj_atr_table = (Object*)&mem.buf[0xfe00];

    u8 sprite_ids[40];
    for (int i = 0; i < 40; i++) sprite_ids[i] = i;

    int il, ir;
    for (il = 0, ir = 39; il < ir;) {
        int ir_top = static_cast<int>(obj_atr_table[sprite_ids[ir]].y) - 16;
        bool ir_not_inline = (ir_top > top_pixel) || (ir_top + sprite_size - 1 < top_pixel);
        if (ir_not_inline) {
            ir--;
            continue;
        }

        int il_top = static_cast<int>(obj_atr_table[sprite_ids[il]].y) - 16;
        bool il_not_inline = (il_top > top_pixel) || (il_top + sprite_size - 1 < top_pixel);
        if (il_not_inline) {
            std::swap(sprite_ids[il++], sprite_ids[ir--]);
            continue;
        }

        il++;
    }

    if (il == ir) {
        int ir_top = static_cast<int>(obj_atr_table[sprite_ids[ir]].y) - 16;
        bool ir_not_inline = (ir_top > top_pixel) || (ir_top + sprite_size - 1 < top_pixel);
        if (ir_not_inline)
            ir--;
        else {
            int il_top = static_cast<int>(obj_atr_table[sprite_ids[il]].y) - 16;
            bool il_not_inline = (il_top > top_pixel) || (il_top + sprite_size - 1 < top_pixel);
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

    std::nth_element(sprite_ids, &sprite_ids[std::min(ir, 10)], &sprite_ids[ir + 1], comp);
    std::sort(sprite_ids, &sprite_ids[std::min(ir + 1, 10)], comp);
    return { *(array<u8, 10>*)sprite_ids, std::min(ir + 1, 10) };
}

auto
lcdc_get_next_obj(u8 lcdc, u8 top_pixel, u8 last_obj_idx) -> u8
{
    const u8 sprite_size = get_bit(lcdc, 2) ? 16 : 8;
    const Object* obj_atr_table = (Object*)&mem.buf[0xfe00];
    auto comp = [=](u8 i1, u8 i2) {
        // move to right all sprites which arent in
        // the current scanline and give priority to left ones
        return (obj_atr_table[i1].x < obj_atr_table[i2].x) || //
               (obj_atr_table[i1].x == obj_atr_table[i2].x && i1 < i2);
    };

    u8 min = last_obj_idx;
    for (int i = 0; i < 40; i++) {
        int sy = static_cast<int>(obj_atr_table[i].y) - 16;
        bool not_inline = (sy > top_pixel) || (sy + sprite_size - 1 < top_pixel);
        if (not_inline)
            continue;

        if (comp(i, min))
            continue;
    }
    return min;
}

array<std::pair<Color, Color_Platte>, 8>
lcdc_render_sprite(u8 sprite_size, u8 sprite_idx, u8 iy)
{
    const Object* obj_atr_table = (Object*)&mem.buf[0xfe00];

    const auto& sprite = obj_atr_table[sprite_idx];
    u16 tile_addr = 0x8000 + 2 * sprite_size * sprite.tile;

    array<std::pair<Color, Color_Platte>, 8> line;
    auto tile_y = get_bit(sprite.flags, Object::Y_FLIP) ? (-iy) % sprite_size : iy % sprite_size;
    auto platte = get_bit(sprite.flags, Object::OBP1) ? Color_Platte::OBP1 : Color_Platte::OBP0;
    line = get_tile_line(mem, tile_addr, tile_y, platte);

    if (get_bit(sprite.flags, Object::X_FLIP))
        std::reverse(line.begin(), line.end());

    return line;
}

auto
lcdc_render_sprites(u8 lcdc,
                    u8 iy,
                    const std::pair<array<u8, 10>, u8>& sprite_ids,
                    array<std::pair<Color, Color_Platte>, 160>& line) -> array<std::pair<Color, Color_Platte>, 160>
{
    const auto sprite_size = get_bit(lcdc, 2) ? 16 : 8;
    const Object* obj_atr_table = (Object*)&mem.buf[0xfe00];

    // render from lowest priority to highest
    for (int i = sprite_ids.second - 1; i >= 0; i--) {
        auto sprite = obj_atr_table[sprite_ids.first[i]];
        sprite.x -= 8;
        u16 tile_addr = 0x8000 + 2 * sprite_size * sprite.tile;
        auto tile_y = get_bit(sprite.flags, Object::Y_FLIP) ? sprite_size - (iy % sprite_size) : iy % sprite_size;

        array<std::pair<Color, Color_Platte>, 8> tile_line;
        auto platte = get_bit(sprite.flags, Object::OBP1) ? Color_Platte::OBP1 : Color_Platte::OBP0;
        tile_line = get_tile_line(mem, tile_addr, tile_y, platte);

        if (get_bit(sprite.flags, Object::X_FLIP))
            std::reverse(tile_line.begin(), tile_line.end());

        // bg color 0 is behind sprite
        if (!get_bit(sprite.flags, Object::BG_PRIORITY)) {
            for (auto j = 0; j < 8; j++) {
                if (tile_line[j].first)
                    line[sprite.x + j] = tile_line[j];
            }
        }
        else {
            for (auto j = 0; j < 8; j++) {
                // anyway dont draw transperent color
                if (!tile_line[j].first)
                    continue;
                // behind bg colors 1-3
                if (line[sprite.x + j].second == Color_Platte::BG && line[sprite.x + j].first)
                    continue;
                line[sprite.x + j] = tile_line[j];
            }
        }
    }
    return line;
}
