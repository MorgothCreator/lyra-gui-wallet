#include <QClipboard>
#include "receive.h"
#include "ui_receive.h"

receive::receive(QWidget *parent, QString id) :
    QDialog(parent),
    ui(new Ui::receive)
{
    ui->setupUi(this);
    ui->idLineEdit->setText(id);
    ui->idLineEdit->setStyleSheet("text-align:center");
}

receive::~receive()
{
    delete ui;
}

void receive::on_copyPushButton_clicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->idLineEdit->text());
}
