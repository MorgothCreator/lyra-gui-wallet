#ifndef SHOWPRIVATEKEY_H
#define SHOWPRIVATEKEY_H

#include <QDialog>
#include <QTimer>

#include "copyed.h"

namespace Ui {
class pKeyWindow;
}

class pKeyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit pKeyWindow(QWidget *parent = nullptr, QString key = nullptr);
    ~pKeyWindow();
    bool getOk() {
        return ok;
    }

    QString getPrivate() {
        return privKey;
    }

private slots:
    void on_copyToClipboardPushButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_privateKeyLineEdit_textChanged(const QString &arg1);

private:
    Ui::pKeyWindow *ui;
    bool ok;
    QString privKey;
    copyed *copyedWindow;
};

#endif // SHOWPRIVATEKEY_H
