#include "cpu.h"
#include "mem.h"

#include <algorithm>
#include <array>

#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include <chrono>
#include <thread>

using std::array;

static array<u32, 4> platte = { 0xc4f0c2, 0x5ab9a8, 0x1e606e, 0x2d1b00 };

constexpr u32 vertical_refresh_clocks = 70224; // dbg mode
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

bool is_ly_regs_eq_check_enable();
bool is_oam_check_enable();
bool is_vbi_check_enable();
bool is_hbi_check_enable();
bool is_ly_regs_eq();

enum Screen_Mode {
    H_BLANK     = 0,
    V_BLANK     = 1,
    OAM         = 2,
    TRANSFARING = 3,
};

using Color = u8;
using Tile  = array<array<Color, 8>, 8>;

array<Color, 8>
get_background_tile_line(const Memory& mem, u8 chr_code, u8 line)
{
    array<Color, 8> result;

    // u16 tile_addr = 0x8000 + chr_code;
    u16 addr    = 0x9000 + chr_code;
    u8  ls_bits = mem.read_byte(addr + line);
    u8  ms_bits = mem.read_byte(addr + line + 1);
    for (int i = 0; i < 8; i++) {
        u8    lsb          = ls_bits & (1 << i);
        u8    msb          = ms_bits & (1 << i);
        u8    color_number = ((msb << 1) | lsb) >> i;
        Color color        = platte.at(color_number);
        result[i]          = color;
    }

    return result;
}

array<array<Color, 160>, 144>
get_background(const Memory& mem, u8 top, u8 left)
{
    // array of 8 tile lines over 144 pixels
    array<array<array<Color, 8>, 20>, 144> background;
    for (int y = 0; y < 18; y++) {
        for (int x = 0; x < 20; x++) {
            int tile_offset = 32 * (top + y) + (x + left > 32 ? left + x - 32 : left + x);
            int chr_code    = mem.read_byte(0x9800 + tile_offset);
            int tile_line   = y % 8;
            background[8 * y + tile_line][x] = get_background_tile_line(mem, chr_code, tile_line % 8);
        }
    }

    array<array<Color, 160>, 144> result;
    assert(sizeof(background) == 144 * 160);
    memcpy(result.data(), background.data(), sizeof(background));
    return result;
    // return std::bit_cast<array<array<Color, 160>, 144>>>(background);
}

class Widget : public QWidget {
    Memory&                              mem;
    Cpu&                                 cpu;
    const array<array<Color, 160>, 144>& lcd;

    void
    paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        // const auto bg = get_background(mem, 0, 0);
        QImage image(&reinterpret_cast<const uchar&>(lcd[0]), 160, 144, QImage::Format_RGB32);
        // image.loadFromData(&reinterpret_cast<const uchar&>(bg[0]), 160 * 140);
        painter.drawImage(0, 0, image.scaled(5 * 160, 5 * 144));
    }

public:
    Widget(Memory& mem_, Cpu& cpu_, const array<array<Color, 160>, 144>& lcd_)
        : mem(mem_), cpu(cpu_), lcd(lcd_)
    {
        // from another thread the ppu should send draw requests to the gui?
        // should the ppu be stateless? it would be intresting if it did
        // should it be multithreaded? even more intresting..!

        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
        timer->start(1000 / 60);

        setWindowTitle(tr("gbmenu"));
        resize(5 * 160, 5 * 144);
    }
};

void
ppu_init(std::atomic<bool>& is_gui_alive, int argc, char* argv[], Memory& mem, Cpu& cpu,
         array<array<Color, 160>, 144>& lcd)
{
    QApplication q_app(argc, argv);
    Widget       widget(mem, cpu, lcd);
    widget.show();

    q_app.exec();
    is_gui_alive = false;
}

// 0xff41 - lcd status
bool is_ly_regs_eq_check_enable();
bool is_oam_check_enable();
bool is_vbi_check_enable();
bool is_hbi_check_enable();
bool is_ly_regs_eq();
