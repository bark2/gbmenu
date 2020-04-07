#pragma once

#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include <QGridLayout>
#include <QHBoxLayout>

#include <atomic>

#include <QApplication>
#include <QChar>
#include <QObject>

#include <cstdio>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qwidget.h>

#include "bits.h"
#include "types.h"

#include "cpu.h"
#include "mem.h"

#include "ppu.h"

constexpr int lcd_width = 160, lcd_height = 144;
constexpr int scale_factor = 5;

struct Gui {
    template <typename T> using atomic = typename std::atomic<T>;

    atomic<bool> alive;
    atomic<bool> pause;
    atomic<bool> draw_oam;
    atomic<bool> update_lcd;
    atomic<bool> update_oam;
    array<array<u32, 160>, 144> lcd;
    array<RgbColor, 8> oam[40][16];
    QObject* app;
    QEvent::Type Gui_Update_Event;

    static void
    joypad_callback(Memory::Joypad key, bool val)
    {
        set_bit(mem.jp, (u8)key, val);
        cpu.set_interupt(Cpu::Interupt::JOYPAD);
        cpu.stop = false;
    }
};

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

class Obj_Window : public QWidget {
public:
    enum { obj_width = 8, obj_height = 16 };
    const u32* image;
    u8 idx;

    void
    paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        QImage qimage((uchar const*)(image + idx * obj_width * obj_height), obj_width, obj_height, QImage::Format_RGB32);
        painter.drawImage(0, 0, qimage.scaled(scale_factor * obj_width, scale_factor * obj_height));
    }

    Obj_Window() = delete;

    Obj_Window(const u32* _image, u8 _idx) : image(_image), idx(_idx)
    {
        setFocusPolicy(Qt::StrongFocus);
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&Obj_Window::update));
        timer->start(1000 / 60);
        // resize(scale_factor * obj_width, scale_factor * obj_height);
    }
};

class Obj_Text_Info : public QLabel {
public:
    Gui& gui;
    u8 idx;

    void
    update()
    {
        const Object* obj_atr_table = (Object*)&mem.buf[0xfe00];
        auto sprite = obj_atr_table[idx];
        static char info_text[100];

        sprintf(info_text,
                "tile: 0x%x\n%s\n%s\n%s\n%s",
                sprite.tile,
                get_bit(sprite.flags, Object::OBP1) ? "platte: OBP1" : "platte: OBP0",
                get_bit(sprite.flags, Object::X_FLIP) ? "X Flipped" : "N",
                get_bit(sprite.flags, Object::Y_FLIP) ? "Y Flipped" : "N",
                get_bit(sprite.flags, Object::BG_PRIORITY) ? "Behind BG" : "N");
        setText(info_text);
    }

    Obj_Text_Info() = delete;

    Obj_Text_Info(Gui& _gui, u8 _idx) : gui(_gui), idx(_idx)
    {
        // QTimer* timer = new QTimer(this);
        // connect(timer, &QTimer::timeout, this, QOverload<>::of(&Obj_Window::update));
        // timer->start(1000 / 60);
    }
};

class Main_Window : public QWidget {
    Gui& gui;

public:
    Main_Window(Gui& gui) : gui(gui) {};

    void
    keyPressEvent(QKeyEvent* event) override
    {
        Memory::Joypad key = (Memory::Joypad)-1;
        switch (event->key()) {
        case Qt::Key_L: key = Memory::Joypad::RIGHT; break;
        case Qt::Key_J: key = Memory::Joypad::LEFT; break;
        case Qt::Key_I: key = Memory::Joypad::UP; break;
        case Qt::Key_K: key = Memory::Joypad::DOWN; break;

        case Qt::Key_S: key = Memory::Joypad::A; break;
        case Qt::Key_D: key = Memory::Joypad::B; break;
        case Qt::Key_Space: key = Memory::Joypad::SELECT; break;
        case Qt::Key_P: key = Memory::Joypad::START; break;

        case Qt::Key_F1: { // toggle pause
            if (!gui.pause)
                gui.pause = true;
            else
                gui.pause = false;
        } break;
        case Qt::Key_F3: { // toggle oam viewer
            if (!gui.draw_oam) {
                gui.draw_oam = true;
            }
            else {
                gui.draw_oam = false;
            }
        }; break;
        default: QWidget::keyPressEvent(event);
        }

        if (key != (Memory::Joypad)-1)
            Gui::joypad_callback(key, false);
    }

    void
    keyReleaseEvent(QKeyEvent* event) override
    {
        Memory::Joypad key = (Memory::Joypad)-1;
        switch (event->key()) {
        case Qt::Key_L: key = Memory::Joypad::RIGHT; break;
        case Qt::Key_J: key = Memory::Joypad::LEFT; break;
        case Qt::Key_I: key = Memory::Joypad::UP; break;
        case Qt::Key_K: key = Memory::Joypad::DOWN; break;

        case Qt::Key_S: key = Memory::Joypad::A; break;
        case Qt::Key_D: key = Memory::Joypad::B; break;
        case Qt::Key_Space: key = Memory::Joypad::SELECT; break;
        case Qt::Key_P: key = Memory::Joypad::START; break;
        // case Qt::Key_Escape: break;
        default: QWidget::keyPressEvent(event);
        }

        if (key != (Memory::Joypad)-1)
            Gui::joypad_callback(key, true);
    }
};

class Oam : public QWidget {
    Gui& gui;

public:
    Oam() = delete;

    bool
    event(QEvent* e) override
    {
        if (e->type() == gui.Gui_Update_Event) {
            QList<Obj_Text_Info*> widgets = findChildren<Obj_Text_Info*>();
            foreach (Obj_Text_Info* widget, widgets) {
                widget->update();
            }
            return true;
        }

        return QWidget::event(e);
    }

    Oam(Gui& _gui) : gui(_gui)
    {
        QGridLayout* layout = new QGridLayout;
        layout->setSizeConstraint(QLayout::SetMinimumSize);
        for (u8 i = 0; i < 40; i++) {
            QFrame* obj = new QFrame;
            // obj->setFrameStyle(QFrame::Panel | QFrame::Sunken);

            QVBoxLayout* obj_layout = new QVBoxLayout;
            obj_layout->setSizeConstraint(QLayout::SetMinimumSize);

            Obj_Window* obj_image = new Obj_Window((u32*)gui.oam, i);

            Obj_Text_Info* obj_info = new Obj_Text_Info(gui, i);
            obj_info->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
            obj_info->setWordWrap(true);

            obj_layout->addWidget(obj_image);
            obj_layout->addWidget(obj_info);
            obj->setLayout(obj_layout);
            // add obj to oam
            layout->addWidget(obj, i / 8, i % 8);
        }
        setLayout(layout);
    }
};

void gui_start(int argc, char* argv[], Gui& gui);
