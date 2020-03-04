#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QSplitter>
#include <QTimer>
#include <QWidget>

#include <atomic>

#include "types.h"

constexpr int width = 256, height = 256;
constexpr int scale_factor = 5;
class Widget : public QWidget {
    constexpr static int                    width = 256, height = 256;
    const array<array<u32, width>, height>& surface;

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
    Widget(const array<array<u32, width>, height>& surface_) : surface(surface_)
    {
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&Widget::update));
        timer->start(1000 / 60);

        // setWindowTitle(tr("gbmenu"));
        resize(scale_factor * width, scale_factor * height);
    }
};

void
gui_start(std::atomic<bool>&                      is_gui_alive,
          int                                     argc,
          char*                                   argv[],
          const array<array<u32, width>, height>& surface)
{
    QApplication q_app(argc, argv);
    QSplitter    splitter {};
    Widget*      widget = new Widget(surface);

    splitter.setWindowTitle("gbmenu");
    splitter.resize(scale_factor * width, scale_factor * height);
    splitter.addWidget(widget);
    splitter.setSizes({ scale_factor * width });
    splitter.show();

    q_app.exec();
    is_gui_alive = false;
}
