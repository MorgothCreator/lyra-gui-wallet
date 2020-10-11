#ifndef RECEIVE_H
#define RECEIVE_H

#include <QDialog>

namespace Ui {
class receive;
}

class receive : public QDialog
{
    Q_OBJECT

public:
    explicit receive(QWidget *parent = nullptr, QString id = "");
    ~receive();

private slots:
    void on_copyPushButton_clicked();

private:
    Ui::receive *ui;
};

#endif // RECEIVE_H
