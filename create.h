#ifndef CREATE_H
#define CREATE_H

#include <QDialog>

namespace Ui {
class createNewWallet;
}

class createNewWallet : public QDialog
{
    Q_OBJECT

public:
    explicit createNewWallet(QWidget *parent = nullptr);
    ~createNewWallet();
    bool getOk() {
        return ok;
    }
    QString getWalletName() {
        return wallName;
    }
    QString getPassword() {
        return pass1;
    }

private slots:
    void on_pass1LineEdit_textChanged(const QString &arg1);
    void on_pass2LineEdit_textChanged(const QString &arg1);
    void on_okPushButton_clicked();
    void on_cancelPushButton_clicked();
    void on_wallNameLineEdit_textChanged(const QString &arg1);

private:
    Ui::createNewWallet *ui;
    QString wallName;
    QString pass1;
    QString pass2;
    bool ok;
};

#endif // CREATE_H
