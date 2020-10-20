#include <QIcon>
#include "historywindow.h"
#include "ui_historywindow.h"
#include "def.h"

historyWindow::historyWindow(QWidget *parent, QString text, QString title, QIcon *icon) :
    QDialog(parent),
    ui(new Ui::historyWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(WINDOW_ICO_SMALL));

    ui->historyTextEdit->setText(text);
    this->setWindowTitle(title);
    this->setWindowIcon(*icon);
}

historyWindow::~historyWindow()
{
    delete ui;
}
