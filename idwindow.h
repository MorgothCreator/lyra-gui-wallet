#ifndef RECEIVE_H
#define RECEIVE_H

#include <QDialog>
#include <QTimer>

#include "copyed.h"

namespace Ui {
class idWindow;
}

class idWindow : public QDialog
{
    Q_OBJECT

public:
    explicit idWindow(QWidget *parent = nullptr, QString title = nullptr, QString id = "");
    ~idWindow();
    bool getOk() {
        return ok;
    }

    QString getId() {
        return id;
    }

private:
    Ui::idWindow *ui;
    copyed *copyedWindow;
    bool ok;
    QString id;

private slots:
    void on_copyPushButton_clicked();
    void on_selectionButtonBox_accepted();
    void on_idLineEdit_textChanged(const QString &arg1);
    void on_selectionButtonBox_rejected();
    void on_showQrPushButton_clicked();
};

#endif // RECEIVE_H
