#ifndef SHOWPRIVATEKEY_H
#define SHOWPRIVATEKEY_H

#include <QDialog>

namespace Ui {
class privateKey;
}

class privateKey : public QDialog
{
    Q_OBJECT

public:
    explicit privateKey(QWidget *parent = nullptr, bool recover = false);
    ~privateKey();
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
    Ui::privateKey *ui;
    bool ok;
    QString privKey;
};

#endif // SHOWPRIVATEKEY_H
