#include <QFocusEvent>
#include <QIcon>
#include "send.h"
#include "ui_send.h"
#include "def.h"

send::send(QWidget *parent, double myBalance, double fee) :
    QDialog(parent),
    ui(new Ui::send)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(WINDOW_ICO_SMALL));

    snd = false;
    this->fee = fee;
    this->myBalance = myBalance;
    ui->myBalanceLabel->setText(QString::asprintf("%.7f", myBalance) + " LYR");
    ui->myBalanceLabel->setStyleSheet("text-align:center");
    ui->sendLineEdit->setStyleSheet("text-align:center");

    ui->idLineEdit->activateWindow();
    // the event object is released then in event loop (?)
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    // posting event for forcing the focus with low priority
    qApp->postEvent(ui->idLineEdit, (QEvent *)eventFocus, Qt::LowEventPriority);
    ui->idLineEdit->setFocus(Qt::OtherFocusReason);

}

send::~send()
{
    delete ui;
}

bool send::getSend() {
    return snd;
}

double send::getValue() {
    return value;
}
QString send::getId() {
    return id;
}

void send::on_sendPlainTextEdit_textChanged()
{
    if(ui->sendLineEdit->text().toDouble() > (myBalance - fee)) {
        ui->sendLineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 16px");
    } else {
        ui->sendLineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 16px");
    }
}

void send::on_cancelPushButton_clicked()
{
    snd = false;
    this->close();
}

void send::on_sendPushButton_clicked()
{
    value = ui->sendLineEdit->text().toDouble();
    id = ui->idLineEdit->text();
    if(value) {
        snd = true;
    } else {
        snd = false;
    }
    this->close();
}
