#ifndef COPYED_H
#define COPYED_H

#include <QEventLoop>

#include <QDialog>
#include <QTimer>

namespace Ui {
class copyed;
}

class copyed : public QDialog
{
    Q_OBJECT

public:
    explicit copyed(QWidget *parent = nullptr, QString text = nullptr);
    ~copyed();
    QWidget *parent;

private:
    Ui::copyed *ui;
    QTimer copyFadeTimer;
    double fadeCnt;
    QEventLoop *eventLoop;

private slots:
    void showEvent(QShowEvent *event);
    void windowFade();
};

#endif // COPYED_H
