#include "ui_create.h"
#include <QFocusEvent>
#include "create.h"

createNewWallet::createNewWallet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createNewWallet)
{
    ui->setupUi(this);
    ui->wallNameLineEdit->activateWindow();
    // the event object is released then in event loop (?)
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    // posting event for forcing the focus with low priority
    qApp->postEvent(ui->wallNameLineEdit, (QEvent *)eventFocus, Qt::LowEventPriority);
    ui->wallNameLineEdit->setFocus(Qt::OtherFocusReason);
    ok = false;
}

createNewWallet::~createNewWallet()
{
    delete ui;
}

void createNewWallet::on_pass1LineEdit_textChanged(const QString &arg1)
{
    pass1 = arg1;
    ui->pass1LineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 22px");
    if(pass1.compare(pass2)) {
        ui->pass2LineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
    } else {
        ui->pass2LineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 22px");
    }
}

void createNewWallet::on_pass2LineEdit_textChanged(const QString &arg1)
{
    pass2 = arg1;
    if(pass1.compare(pass2)) {
        ui->pass2LineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
    } else {
        ui->pass2LineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 22px");
    }
}

void createNewWallet::on_okPushButton_clicked()
{
    QRegExp hexMatcher("^[a-zA-Z0-9_-]*$", Qt::CaseInsensitive);
    if (!hexMatcher.exactMatch(ui->wallNameLineEdit->text())) {
        ui->wallNameLineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
    } else if(ui->pass1LineEdit->text().length() < 8) {
        ui->pass1LineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
        ui->pass2LineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
    } else {
        ok = true;
        this->close();
    }
}

void createNewWallet::on_cancelPushButton_clicked()
{
    ok = false;
    this->close();
}

void createNewWallet::on_wallNameLineEdit_textChanged(const QString &)
{
    QRegExp hexMatcher("^[a-zA-Z0-9_-]*$", Qt::CaseInsensitive);
    if (!hexMatcher.exactMatch(ui->wallNameLineEdit->text())) {
        ui->wallNameLineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
    } else {
        ui->wallNameLineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 22px");
        wallName = ui->wallNameLineEdit->text();
    }
}
