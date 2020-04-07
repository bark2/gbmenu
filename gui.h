#pragma once

#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include <atomic>

#include "types.h"

#include "cpu.h"
#include "mem.h"

#include "ppu.h"

constexpr int lcd_width = 160, lcd_height = 144;
constexpr int scale_factor = 5;
template <u32 Width, u32 Height> class Image_Widget : public QWidget {
public:
    const uchar* image;

    void
    paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        QImage qimage(image, Width, Height, QImage::Format_RGB32);
        painter.drawImage(0, 0, qimage.scaled(scale_factor * Width, scale_factor * Height));
    }

    Image_Widget() = delete;

    Image_Widget(const array<array<u32, Width>, Height>& _image) : image((const uchar*)_image.data())
    {
        setFocusPolicy(Qt::StrongFocus);
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&Image_Widget::update));
        timer->start(1000 / 60);
        resize(scale_factor * Width, scale_factor * Height);
    }
};

// class Oam_Frame : public QGridLayout {
// public:
//     Oam_Frame() { addLayout(this, 8, 5); }
// };

struct Gui {
    template <typename T> using atomic = typename std::atomic<T>;

    atomic<bool> alive;
    atomic<bool> draw;
    atomic<bool> oam_enabled;
    array<array<u32, 160>, 144> lcd;
    // array<RgbColor, 8>          oam[8][16 * 5];
    array<RgbColor, 8> oam[8 * 16 * 5];

    static void
    joypad_callback(Memory::Joypad key, bool val)
    {
        set_bit(mem.jp, (u8)key, val);
        cpu.set_interupt(Cpu::Interupt::JOYPAD);
    }
};

void gui_start(int argc, char* argv[], Gui& gui);
