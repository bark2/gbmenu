#pragma once

#include "types.h"

static inline void
set_bit(u8& s, u8 i, bool val)
{
    s = (s & ~(1 << i)) | (val << i);
}

static inline u8
get_bit(u8 s, u8 i)
{
    return s & (1 << i);
}

static inline u16
concat(u8 hi, u8 lo)
{
    return (hi << 8) | lo;
}

static inline u8
lower(u16 s)
{
    return (s & 0x00ff);
}

static inline u8
higher(u16 s)
{
    return (s >> 8);
}

static inline u16
swap(u16 s)
{
    return concat(lower(s), higher(s));
}

static inline char
dec_to_hex(u8 c)
{
    if (c < 10) return c + '0';
    return c - 10 + 'a';
}

static inline string
conv_word_hex(u16 r)
{
    return {
        dec_to_hex((r >> 12) & 0xf), //
        dec_to_hex((r >> 8) & 0xf),  //
        dec_to_hex((r >> 4) & 0xf),  //
        dec_to_hex(r & 0xf),         //
    };
}

static inline string
conv_byte_hex(u8 byte)
{
    return {
        dec_to_hex((byte >> 4) & 0xf), //
        dec_to_hex(byte & 0xf),        //
    };
}
