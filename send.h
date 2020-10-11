#ifndef SEND_H
#define SEND_H

#include <QDialog>

namespace Ui {
class send;
}

class send : public QDialog
{
    Q_OBJECT

public:
    explicit send(QWidget *parent = nullptr, double myBalance = 0.0, double fee = 1.0);
    ~send();
    bool getSend();
    double getValue();
    QString getId();

private slots:
    void on_sendPlainTextEdit_textChanged();
    void on_cancelPushButton_clicked();
    void on_sendPushButton_clicked();

private:
    Ui::send *ui;
    double myBalance;
    double fee;
    bool snd;
    double value;
    QString id;
};

#endif // SEND_H
