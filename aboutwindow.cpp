#include <QIcon>
#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include "def.h"

aboutWindow::aboutWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutWindow)
{
    ui->setupUi(this);

    QIcon pllIcon = QIcon(QCoreApplication::applicationDirPath() + WINDOW_ICO_SMALL);
    this->setWindowIcon(pllIcon);
}

aboutWindow::~aboutWindow()
{
    delete ui;
}
