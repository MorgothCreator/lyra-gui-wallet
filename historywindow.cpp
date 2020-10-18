#include "historywindow.h"
#include "ui_historywindow.h"

historyWindow::historyWindow(QWidget *parent, QString text, QString title, QIcon *icon) :
    QDialog(parent),
    ui(new Ui::historyWindow)
{
    ui->setupUi(this);
    ui->historyTextEdit->setText(text);
    this->setWindowTitle(title);
    this->setWindowIcon(*icon);
}

historyWindow::~historyWindow()
{
    delete ui;
}
