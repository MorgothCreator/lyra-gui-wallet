#include "historywindow.h"
#include "ui_historywindow.h"

historyWindow::historyWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::historyWindow)
{
    ui->setupUi(this);
}

historyWindow::~historyWindow()
{
    delete ui;
}
