#include <QIcon>
#include <QScrollBar>
#include "ui_debugwindow.h"
#include "debugwindow.h"
#include "def.h"

debugWindow::debugWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::debugWindow)
{
    ui->setupUi(this);
    QIcon pllIcon = QIcon(WINDOW_ICO_SMALL);
    this->setWindowIcon(pllIcon);
}

debugWindow::~debugWindow()
{
    delete ui;
}

void debugWindow::append(QString text) {
    QString tmp = text.remove('\n').remove('\r');
    if(tmp.length() != 0) {
        ui->debugTextEdit->append(tmp + "\n");
        QScrollBar *sb = ui->debugTextEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

void debugWindow::append(QStringList text) {
    QStringList tmp = text;
    tmp.removeAll("");
    for(QString line : tmp) {
        ui->debugTextEdit->append("        " + line.remove('\n').remove('\r') + "\n");
        QScrollBar *sb = ui->debugTextEdit->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}
