#ifndef SHOWQR_H
#define SHOWQR_H

#include <QDialog>
#include <QPixmap>
#include <QGraphicsScene>
#include <QFileDialog>


#include "qrcodegen.h"

namespace Ui {
class showqr;
}

class showqr : public QDialog
{
    Q_OBJECT

public:
    explicit showqr(QWidget *parent = nullptr, QString text = nullptr);
    ~showqr();

private slots:
    void on_savePushButton_clicked();

private:
    void printQr(const uint8_t qrcode[]);
    Ui::showqr *ui;
    QString idText;

    QPixmap image;
    QImage  *imageObject;
    QGraphicsScene scene;

};

#endif // SHOWQR_H
