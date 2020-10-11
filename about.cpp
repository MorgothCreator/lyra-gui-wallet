#include <QIcon>
#include "about.h"
#include "ui_about.h"
#include "def.h"

about::about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);

    QIcon pllIcon = QIcon(QCoreApplication::applicationDirPath() + WINDOW_ICO_SMALL);
    this->setWindowIcon(pllIcon);
}

about::~about()
{
    delete ui;
}
