#include <QIcon>
#include <QScrollBar>
#include "ui_debug.h"
#include "debug.h"
#include "def.h"

debug::debug(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::debug)
{
    ui->setupUi(this);
    QIcon pllIcon = QIcon(QCoreApplication::applicationDirPath() + WINDOW_ICO_SMALL);
    this->setWindowIcon(pllIcon);
}

debug::~debug()
{
    delete ui;
}

void debug::append(QString text) {
    QString tmp = text.remove('\n').remove('\r');
    if(tmp.length() != 0) {
        ui->debugTextEdit->append(tmp + "\n");
        QScrollBar *sb = ui->debugTextEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

void debug::append(QStringList text) {
    QStringList tmp = text;
    tmp.removeAll("");
    for(QString line : tmp) {
        ui->debugTextEdit->append(line.remove('\n').remove('\r') + "\n");
        QScrollBar *sb = ui->debugTextEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}
