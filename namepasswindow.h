#ifndef CREATE_H
#define CREATE_H

#include <QDialog>

namespace Ui {
class namePassWindow;
}

class namePassWindow : public QDialog
{
    Q_OBJECT

public:
    explicit namePassWindow(QWidget *parent = nullptr);
    ~namePassWindow();
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
    Ui::namePassWindow *ui;
    QString wallName;
    QString pass1;
    QString pass2;
    bool ok;
};

#endif // CREATE_H
