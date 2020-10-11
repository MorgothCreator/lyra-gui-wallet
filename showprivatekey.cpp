#include <QClipboard>
#include <QFocusEvent>
#include "showprivatekey.h"
#include "ui_showprivatekey.h"


privateKey::privateKey(QWidget *parent, bool recover) :
    QDialog(parent),
    ui(new Ui::privateKey)
{
    ui->setupUi(this);
    ui->copyToClipboardPushButton->setVisible(!recover);
    ui->privateKeyLineEdit->setReadOnly(!recover);

    ui->privateKeyLineEdit->activateWindow();
    // the event object is released then in event loop (?)
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    // posting event for forcing the focus with low priority
    qApp->postEvent(ui->privateKeyLineEdit, (QEvent *)eventFocus, Qt::LowEventPriority);
    ui->privateKeyLineEdit->setFocus(Qt::OtherFocusReason);
    ok = false;
}

privateKey::~privateKey()
{
    delete ui;
}

void privateKey::on_copyToClipboardPushButton_clicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->privateKeyLineEdit->text());
}

void privateKey::on_buttonBox_accepted()
{
    ok = true;
    this->close();
}

void privateKey::on_buttonBox_rejected()
{
    ok = false;
    this->close();
}

void privateKey::on_privateKeyLineEdit_textChanged(const QString &arg1)
{
    privKey = arg1;
}
