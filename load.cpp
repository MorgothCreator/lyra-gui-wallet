#include <QFocusEvent>
#include "load.h"
#include "ui_load.h"

loadWallet::loadWallet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loadWallet)
{
    ui->setupUi(this);
    ok = false;

    ui->nameLineEdit->activateWindow();
    // the event object is released then in event loop (?)
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    // posting event for forcing the focus with low priority
    qApp->postEvent(ui->nameLineEdit, (QEvent *)eventFocus, Qt::LowEventPriority);
    ui->nameLineEdit->setFocus();
    ui->nameLineEdit->setCursorPosition(1);
}

loadWallet::~loadWallet()
{
    delete ui;
}

void loadWallet::on_buttonBox_accepted()
{
    ok = true;
    this->close();
}

void loadWallet::on_buttonBox_rejected()
{
    ok = false;
    this->close();
}

void loadWallet::on_nameLineEdit_textChanged(const QString &arg1)
{
    QRegExp hexMatcher("^[a-zA-Z0-9_-]*$", Qt::CaseInsensitive);
    if (!hexMatcher.exactMatch(ui->nameLineEdit->text())) {
        ui->nameLineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
    } else {
        ui->nameLineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 22px");
        wallName = arg1;
    }
}
