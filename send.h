#ifndef SEND_H
#define SEND_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class send;
}

class send : public QDialog
{
    Q_OBJECT

public:
    explicit send(QWidget *parent = nullptr, double myBalance = 0.0, double fee = 1.0, QString *coinsName = NULL, double *coinsQty = NULL);
    ~send();
    bool getSend();
    double getValue();
    QString getId();
    QString getCoin();
    int selCoin;

private slots:
    void on_cancelPushButton_clicked();
    void on_sendPushButton_clicked();
    void on_comboBox_CoinList_currentIndexChanged(int index);
    void on_pushButton_Min_clicked();
    void on_pushButton_25_clicked();
    void on_pushButton_50_clicked();
    void on_pushButton_75_clicked();
    void on_pushButton_Max_clicked();

    void on_sendLineEdit_textChanged(const QString &arg1);

private:
    Ui::send *ui;
    double myBalance;
    double *coinsQty;
    QString *coinsName;
    double fee;
    bool snd;
    double value;
    QString id;
    QString coin;
};

#endif // SEND_H
