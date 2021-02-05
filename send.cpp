#include <QFocusEvent>
#include <QIcon>
#include "send.h"
#include "ui_send.h"
#include "def.h"

send::send(QWidget *parent, double myBalance, double fee, QString *coinsName, double *coinsQty) :
    QDialog(parent),
    ui(new Ui::send)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(WINDOW_ICO_SMALL));

    snd = false;
    ui->comboBox_CoinList->setCurrentText("LYR");
    this->fee = fee;
    this->coinsQty = coinsQty;
    this->coinsName = coinsName;
    this->selCoin = 0;
    ui->myBalanceLabel->setText(QString::asprintf("%.7f", myBalance) + " LYR");
    ui->myBalanceLabel->setStyleSheet("text-align:center");
    ui->sendLineEdit->setStyleSheet("text-align:center");

    ui->idLineEdit->activateWindow();
    // the event object is released then in event loop (?)
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    // posting event for forcing the focus with low priority
    qApp->postEvent(ui->idLineEdit, (QEvent *)eventFocus, Qt::LowEventPriority);
    ui->idLineEdit->setFocus(Qt::OtherFocusReason);

    ui->comboBox_CoinList->clear();
    for (int cnt = 0; cnt < COIN_TABLE_MAX_SIZE; cnt++) {
        if(coinsName[cnt].length() == 0)
            break;
        ui->comboBox_CoinList->addItem(coinsName[cnt]);
    }
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

QString send::getCoin() {
    return coinsName[selCoin];
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
    coin = coinsName[selCoin];
    if(ui->sendLineEdit->text().toDouble() > (myBalance - (coinsName[selCoin].compare("LYR") ? 0 : fee))) {
        ui->sendLineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 16px");
    } else {
        ui->sendLineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 16px");
    }
    if(value) {
        snd = true;
    } else {
        snd = false;
    }
    this->close();
}

void send::on_comboBox_CoinList_currentIndexChanged(int index)
{
    this->selCoin = index;
    this->myBalance = coinsQty[index];
    ui->myBalanceLabel->setText(coinsName[index] + ":\n" + QString::asprintf("%.8f", coinsQty[index]));
}

void send::on_pushButton_Min_clicked()
{
    if(!coinsName[selCoin].compare("LYR")) {
        if(coinsQty[selCoin] > 1)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", 1.0));
    } else {
        if(coinsQty[selCoin] > 0)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", 1.0));
    }
}

void send::on_pushButton_25_clicked()
{
    if(!coinsName[selCoin].compare("LYR")) {
        if(coinsQty[selCoin] > 1)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", coinsQty[selCoin] / 4.0));
    } else {
        if(coinsQty[selCoin] > 0)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", coinsQty[selCoin] / 4.0));
    }
}

void send::on_pushButton_50_clicked()
{
    if(!coinsName[selCoin].compare("LYR")) {
        if(coinsQty[selCoin] > 1)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", coinsQty[selCoin] / 2.0));
    } else {
        if(coinsQty[selCoin] > 0)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", coinsQty[selCoin] / 2.0));
    }
}

void send::on_pushButton_75_clicked()
{
    if(!coinsName[selCoin].compare("LYR")) {
        if(coinsQty[selCoin] > 1)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", (coinsQty[selCoin] / 4.0) * 3));
    } else {
        if(coinsQty[selCoin] > 0)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", (coinsQty[selCoin] / 4.0) * 3));
    }
}

void send::on_pushButton_Max_clicked()
{
    if(!coinsName[selCoin].compare("LYR")) {
        if(coinsQty[selCoin] > 1)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", coinsQty[selCoin] - 1));
    } else {
        if(coinsQty[selCoin] > 0)
            ui->sendLineEdit->setText(QString::asprintf("%.8f", coinsQty[selCoin]));
    }
}


void send::on_sendLineEdit_textChanged(const QString &arg1)
{
    if(arg1.toDouble() > (myBalance - (coinsName[selCoin].compare("LYR") ? 0 : fee))) {
        ui->sendLineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 16px");
    } else {
        ui->sendLineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 16px");
    }
}
