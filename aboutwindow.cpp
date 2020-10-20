#include <QIcon>
#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include "def.h"

aboutWindow::aboutWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(WINDOW_ICO_SMALL));
}

aboutWindow::~aboutWindow()
{
    delete ui;
}
