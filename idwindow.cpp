#include <QClipboard>
#include "idwindow.h"
#include "ui_idwindow.h"
#include "def.h"
#include "showqr.h"

idWindow::idWindow(QWidget *parent, QString title, QString id) :
    QDialog(parent),
    ui(new Ui::idWindow)
{
    ui->setupUi(this);
    ok = false;
    this->setWindowTitle(title);
    ui->idLineEdit->setText(id);
    ui->idLineEdit->setStyleSheet("text-align:center");
    QIcon pllIcon = QIcon(COPY_ICO_SMALL);
    ui->copyPushButton->setIcon(pllIcon);


}

idWindow::~idWindow()
{
    delete ui;
}

void idWindow::on_copyPushButton_clicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->idLineEdit->text());

    QPoint thisWindow = this->window()->mapToGlobal(QPoint(0, 0));
    copyedWindow = new copyed(this, "Received address has been copyed to clopboard...");
    if(copyedWindow) {
        copyedWindow->setGeometry(thisWindow.x(), thisWindow.y(), this->width(), this->height());
        copyedWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        copyedWindow->exec();
    }
}


void idWindow::on_selectionButtonBox_accepted()
{
    ok = true;
}

void idWindow::on_selectionButtonBox_rejected()
{
    ok = false;
}

void idWindow::on_idLineEdit_textChanged(const QString &arg1)
{
    id = arg1;
}

void idWindow::on_showQrPushButton_clicked()
{
    showqr qrWindow(this, ui->idLineEdit->text());
    //qrWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    qrWindow.exec();
}
