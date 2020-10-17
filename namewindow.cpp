#include <QFocusEvent>
#include "namewindow.h"
#include "ui_namewindow.h"

nameWindow::nameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nameWindow)
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

nameWindow::~nameWindow()
{
    delete ui;
}

void nameWindow::on_buttonBox_accepted()
{
    ok = true;
    this->close();
}

void nameWindow::on_buttonBox_rejected()
{
    ok = false;
    this->close();
}

void nameWindow::on_nameLineEdit_textChanged(const QString &arg1)
{
    QRegExp hexMatcher("^[a-zA-Z0-9_-]*$", Qt::CaseInsensitive);
    if (!hexMatcher.exactMatch(ui->nameLineEdit->text())) {
        ui->nameLineEdit->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
    } else {
        ui->nameLineEdit->setStyleSheet("font-weight: bold; color: black; font: bold 22px");
        wallName = arg1;
    }
}
