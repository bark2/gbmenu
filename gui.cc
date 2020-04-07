#include <QApplication>
#include <QBoxLayout>
#include <QGridLayout>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>

#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>
#include <atomic>
#include <qboxlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qwidget.h>

#include <vector>

#include "gui_.h"
#include "mem.h"
#include "types.h"

class Lcd_Widget : public Image_Widget<lcd_width, lcd_height> {
    std::atomic<bool>& update;

public:
    Lcd_Widget(const array<array<u32, lcd_width>, lcd_height>& pixels, std::atomic<bool>& _update)
        : Image_Widget<lcd_width, lcd_height>(pixels), update(_update)
    {
    }

    void
    paintEvent(QPaintEvent* event) override
    {
        if (!update)
            return;

        update = false;
        QPainter painter(this);
        QImage qimage(image, lcd_width, lcd_height, QImage::Format_RGB32);
        painter.drawImage(0, 0, qimage.scaled(scale_factor * lcd_width, scale_factor * lcd_height));
    }
};

void
gui_start(int argc, char* argv[], Gui& gui)
{
    QApplication q_app(argc, argv);
    Main_Window main_window(gui);
    Lcd_Widget* lcd = new Lcd_Widget(gui.lcd, gui.update_lcd);

    Oam* oam = new Oam(gui);
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(lcd);
    layout->addWidget(oam);
    main_window.setLayout(layout);
    main_window.show();

    // gui.app = qobject_cast<QObject*>(&q_app);
    gui.app = qobject_cast<QObject*>(oam);
    q_app.exec();
    gui.alive = false;
}
