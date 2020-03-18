#pragma once

#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include <atomic>

#include "mem.h"
#include "types.h"

constexpr int lcd_width = 160, lcd_height = 144;
constexpr int scale_factor = 5;
template <u32 Width, u32 Height> class Image_Widget : public QWidget {
public:
    const uchar* image;

    void
    paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        QImage   qimage(image, Width, Height, QImage::Format_RGB32);
        painter.drawImage(0,
                          0,
                          qimage.scaled(scale_factor * Width, scale_factor * Height));
    }

    Image_Widget() = delete;

    Image_Widget(const array<array<u32, Width>, Height>& _image)
        : image((const uchar*)_image.data())
    {
        setFocusPolicy(Qt::StrongFocus);
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&Image_Widget::update));
        timer->start(1000 / 60);
        resize(scale_factor * Width, scale_factor * Height);
    }
};

using Joypad_Callback = void (*)(Memory::Joypad, bool);
void gui_start(std::atomic<bool>&                 is_gui_alive,
               int                                argc,
               char*                              argv[],
               const array<array<u32, 160>, 144>& surface,
               Joypad_Callback                    callback);
