#include <QFocusEvent>
#include <QIcon>
#include "password.h"
#include "ui_password.h"
#include "def.h"

password::password(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::password)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(WINDOW_ICO_SMALL));

    ui->lineEdit->activateWindow();
    // the event object is released then in event loop (?)
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    // posting event for forcing the focus with low priority
    qApp->postEvent(ui->lineEdit, (QEvent *)eventFocus, Qt::LowEventPriority);
    ui->lineEdit->setFocus(Qt::OtherFocusReason);
    accepted = false;
}

password::~password()
{
    delete ui;
}

QString password::getPass() {
    return pass;
}

bool password::getAccepted() {
    return accepted;
}

void password::showEvent(QShowEvent *) {
    accepted = false;
}

void password::on_selectionButtonBox_accepted()
{
    accepted = true;
}

void password::on_lineEdit_textChanged(const QString &arg1)
{
    pass = arg1;
}
