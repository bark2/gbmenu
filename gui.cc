#include <QApplication>
#include <QImage>
#include <QKeyEvent>
#include <QPainter>
#include <QSplitter>
#include <QTimer>
#include <QWidget>

#include <atomic>
#include <qnamespace.h>

#include "gui.h"
#include "mem.h"
#include "types.h"

class Lcd_Widget : public Image_Widget<lcd_width, lcd_height> {
public:
    Joypad_Callback callback;

    Lcd_Widget(const array<array<u32, lcd_width>, lcd_height>& pixels, Joypad_Callback cb)
        : Image_Widget<lcd_width, lcd_height>(pixels), callback(cb)
    {
    }

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
        // case Qt::Key_Escape: break;
        default: QWidget::keyPressEvent(event);
        }

        if (key != (Memory::Joypad)-1)
            callback(key, false);
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
            callback(key, true);
    }
};

void
gui_start(std::atomic<bool>&                              is_gui_alive,
          int                                             argc,
          char*                                           argv[],
          const array<array<u32, lcd_width>, lcd_height>& pixels,
          Joypad_Callback                                 callback)
{
    QApplication q_app(argc, argv);
    QSplitter    splitter {};

    Lcd_Widget*                          lcd = new Lcd_Widget(pixels, callback);
    // Image_Widget<lcd_width, lcd_height>* vram =
        // new Image_Widget<lcd_width, lcd_height>(pixels);

    splitter.setWindowTitle("gbmenu");
    splitter.resize(scale_factor * lcd_width, scale_factor * lcd_height);
    splitter.addWidget(lcd);

    // splitter.addWidget(vram);
    // splitter.setSizes({ scale_factor * lcd_width, scale_factor * lcd_width });

    splitter.show();

    q_app.exec();
    is_gui_alive = false;
}
