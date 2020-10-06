#include <QApplication>
#include <QBoxLayout>
#include <QGridLayout>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QTextEdit>

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
#include <qtextedit.h>
#include <qwidget.h>

#include <cstring>

#include <vector>

#include "gui_.h"
#include "mem.h"
#include "types.h"

class Lcd_Widget : public Image_Widget<lcd_width, lcd_height, 0> {
    std::atomic<bool>& update;

public:
    Lcd_Widget(const array<array<u32, lcd_width>, lcd_height>& pixels, std::atomic<bool>& _update)
        : Image_Widget<lcd_width, lcd_height, 0>((u32 const*)pixels.data()), update(_update)
    {
    }

    void
    paintEvent(QPaintEvent* event) override
    {
        if (!update)
            return;

        update = false;
        QPainter painter(this);
        QImage qimage((uchar const*)image, lcd_width, lcd_height, QImage::Format_RGB32);
        painter.drawImage(0, 0, qimage.scaled(scale_factor * lcd_width, scale_factor * lcd_height));
    }
};

static string
info(Cpu& cpu, Memory& mem, u16 pc = 0xffff)
{
    string result;
    if (pc == 0xffff)
        pc = cpu.pc;

    bool is_pcb = (mem.get(pc) == 0xcb);
    auto inst = (is_pcb ? prefix_cb_instructions[mem.get(pc + 1)] : instructions[mem.get(pc)]);

    result += to_string(cpu);
    // interupt registers
    result += "ie: " + conv_byte_hex(mem.get(reg_ie)) + "  if: " + conv_byte_hex(mem.get(reg_if)) +
              "  ime: " + (cpu.int_master_enable ? "1" : "0") + '\n';
    result += "lcdc: " + conv_byte_hex(mem.get(reg_lcdc)) + "  stat: " + conv_byte_hex(mem.get(reg_stat)) +
              "  ly: " + conv_byte_hex(mem.get(reg_ly)) + '\n';
    result += conv_word_hex(pc) + "[opcode: 0x";
    u8 op_length = (is_pcb ? 2 : 1);
    for (int i = 0; i < op_length; i++) result += conv_byte_hex(mem.read_byte(pc + i));
    result += "]|\t" + (is_pcb ? prefix_cb_instructions[mem.read_byte(pc + 1)].mnemonic : inst.mnemonic);
    if (inst.length - op_length == 1)
        result += string("\t$") + conv_byte_hex(mem.read_byte(pc + op_length));
    else if (inst.length - op_length == 2)
        result += string("\t$") + conv_word_hex(mem.read_word(pc + op_length));
    return result;
}

class Instructions : public QTextEdit {
    Gui& gui;

public:
    Instructions() = delete;

    bool
    event(QEvent* e) override
    {
        if (e->type() == gui.Gui_Update_Event) {
            setText(info(cpu, mem).c_str());
            return true;
        }

        return false;
        return QWidget::event(e);
    }

    Instructions(Gui& gui_) : gui(gui_)
    {
        setMaximumWidth(200);
        setReadOnly(true);
    }
};

void
gui_start(int argc, char* argv[], Gui& gui)
{
    QApplication q_app(argc, argv);
    Main_Window main_window(gui);
    Lcd_Widget* lcd = new Lcd_Widget(gui.lcd, gui.update_lcd);
    // Instructions* txt = new Instructions(gui);

    Oam* oam = new Oam(gui);
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(lcd);
    // layout->addWidget(txt);
    layout->addWidget(oam);

    main_window.setLayout(layout);
    main_window.show();

    // gui.app = qobject_cast<QObject*>(&q_app);
    gui.app = qobject_cast<QObject*>(oam);
    // gui.app = qobject_cast<QObject*>(txt);
    q_app.exec();
    gui.alive = false;
}
