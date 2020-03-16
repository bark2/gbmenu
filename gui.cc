#include <QApplication>
#include <QImage>
#include <QKeyEvent>
#include <QPainter>
#include <QSplitter>
#include <QTimer>
#include <QWidget>

#include <atomic>
#include <qnamespace.h>

#include "mem.h"
#include "types.h"

constexpr int width = 160, height = 144;
constexpr int scale_factor = 5;
class Widget : public QWidget {
    constexpr static int                    width = 160, height = 144;
    const array<array<u32, width>, height>& surface;
    void (*input_callback)(Memory::Joypad key, bool val);

    void
    keyPressEvent(QKeyEvent* event) override
    {
        switch (event->key()) {
        case Qt::Key_L: input_callback(Memory::Joypad::RIGHT, false); break;
        case Qt::Key_J: input_callback(Memory::Joypad::LEFT, false); break;
        case Qt::Key_I: input_callback(Memory::Joypad::UP, false); break;
        case Qt::Key_K: input_callback(Memory::Joypad::DOWN, false); break;

        case Qt::Key_S: input_callback(Memory::Joypad::A, false); break;
        case Qt::Key_D: input_callback(Memory::Joypad::B, false); break;
        case Qt::Key_Backspace: input_callback(Memory::Joypad::SELECT, false); break;
        case Qt::Key_Enter: input_callback(Memory::Joypad::START, false); break;
        // case Qt::Key_Escape: break;
        default: QWidget::keyPressEvent(event);
        }
    }

    void
    keyReleaseEvent(QKeyEvent* event) override
    {
        switch (event->key()) {
        case Qt::Key_L: input_callback(Memory::Joypad::RIGHT, true); break;
        case Qt::Key_J: input_callback(Memory::Joypad::LEFT, true); break;
        case Qt::Key_I: input_callback(Memory::Joypad::UP, true); break;
        case Qt::Key_K: input_callback(Memory::Joypad::DOWN, true); break;

        case Qt::Key_S: input_callback(Memory::Joypad::A, true); break;
        case Qt::Key_D: input_callback(Memory::Joypad::B, true); break;
        case Qt::Key_Backspace: input_callback(Memory::Joypad::SELECT, true); break;
        case Qt::Key_Enter: input_callback(Memory::Joypad::START, true); break;
        // case Qt::Key_Escape: break;
        default: QWidget::keyPressEvent(event);
        }
    }
    void
    paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        QImage   image(&reinterpret_cast<const uchar&>(surface[0]),
                     width,
                     height,
                     QImage::Format_RGB32);
        // image.loadFromData(&reinterpret_cast<const uchar&>(bg[0]), width * 140);
        painter.drawImage(0,
                          0,
                          image.scaled(scale_factor * width, scale_factor * height));
    }

public:
    Widget(const array<array<u32, width>, height>& surface_,
           void (*input_callback_)(Memory::Joypad, bool))
        : surface(surface_), input_callback(input_callback_)
    {
        setFocusPolicy(Qt::StrongFocus);
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
        timer->start(1000 / 60);
        resize(scale_factor * width, scale_factor * height);
    }
};

void
gui_start(std::atomic<bool>&                      is_gui_alive,
          int                                     argc,
          char*                                   argv[],
          const array<array<u32, width>, height>& surface,
          void (*input_callback)(Memory::Joypad, bool))
{
    QApplication q_app(argc, argv);
    QSplitter    splitter {};
    Widget*      widget = new Widget(surface, input_callback);

    splitter.setWindowTitle("gbmenu");
    splitter.resize(scale_factor * width, scale_factor * height);
    splitter.addWidget(widget);
    splitter.setSizes({ scale_factor * width });
    splitter.show();

    q_app.exec();
    is_gui_alive = false;
}
