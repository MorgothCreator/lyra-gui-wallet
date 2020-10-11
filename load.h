#ifndef LOAD_H
#define LOAD_H

#include <QDialog>

namespace Ui {
class loadWallet;
}

class loadWallet : public QDialog
{
    Q_OBJECT

public:
    explicit loadWallet(QWidget *parent = nullptr);
    ~loadWallet();
    QString getName() {
        return wallName;
    }

    bool getOk() {
        return ok;
    }

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_nameLineEdit_textChanged(const QString &arg1);

private:
    Ui::loadWallet *ui;
    QString wallName;
    bool ok;
};

#endif // LOAD_H
