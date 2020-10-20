#include <QClipboard>
#include <QFocusEvent>
#include "pkeywindow.h"
#include "ui_pkeywindow.h"
#include "def.h"


pKeyWindow::pKeyWindow(QWidget *parent, QString key) :
    QDialog(parent),
    ui(new Ui::pKeyWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(WINDOW_ICO_SMALL));

    ui->copyToClipboardPushButton->setVisible(key != nullptr);
    ui->privateKeyLineEdit->setReadOnly(key != nullptr);

    ui->copyToClipboardPushButton->setIcon(QIcon(COPY_ICO_SMALL));

    ui->privateKeyLineEdit->activateWindow();
    // the event object is released then in event loop (?)
    QFocusEvent* eventFocus = new QFocusEvent(QEvent::FocusIn);
    // posting event for forcing the focus with low priority
    qApp->postEvent(ui->privateKeyLineEdit, (QEvent *)eventFocus, Qt::LowEventPriority);
    ui->privateKeyLineEdit->setFocus(Qt::OtherFocusReason);
    if(key != nullptr) {
        ui->privateKeyLineEdit->setText(key);
    }
    ok = false;

}

pKeyWindow::~pKeyWindow()
{
    delete ui;
}

void pKeyWindow::on_copyToClipboardPushButton_clicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->privateKeyLineEdit->text());

    QPoint thisWindow = this->window()->mapToGlobal(QPoint(0, 0));
    copyedWindow = new copyed(this, "Private key has been copyed to clopboard...");
    if(copyedWindow) {
        copyedWindow->setGeometry(thisWindow.x(), thisWindow.y(), this->width(), this->height());
        copyedWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        copyedWindow->exec();
    }
}

void pKeyWindow::on_buttonBox_accepted()
{
    ok = true;
    this->close();
}

void pKeyWindow::on_buttonBox_rejected()
{
    ok = false;
    this->close();
}

void pKeyWindow::on_privateKeyLineEdit_textChanged(const QString &arg1)
{
    privKey = arg1;
}

