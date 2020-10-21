#include <QtDebug>

#include "showqr.h"
#include "ui_showqr.h"
#include "def.h"

showqr::showqr(QWidget *parent, QString text) :
    QDialog(parent),
    ui(new Ui::showqr)
{
    ui->setupUi(this);
    idText = text;
    ui->graphicsView->setScene(&scene);

    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
    bool ok = qrcodegen_encodeText(text.toUtf8(), tempBuffer, qrcode,
        qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
    if (ok) {
        scene.clear();
        int border = 4;
        int size = qrcodegen_getSize(qrcode);
        this->setGeometry(parent->geometry().x(), parent->geometry().y(), (size * 8) + 20, (size * 8) + 80);
        ui->savePushButton->setGeometry((((size * 8) + 20) / 2) - 50, (size * 8) + 40, 100, 20);
        ui->graphicsView->setGeometry(0, 0, (size * 8), (size * 8));
        ui->graphicsView->setMinimumSize((size * 8) + 20, (size * 8) + 20);
        ui->graphicsView->setMaximumSize((size * 8) + 20, (size * 8) + 20);
        for (int y = -border; y < size + border; y++) {
            for (int x = -border; x < size + border; x++) {
                if(qrcodegen_getModule(qrcode, x, y)) {
                    //scene.addRect(QRectF(x * 8, y * 8, 8, 8));
                    scene.addRect(QRectF(x * 8 + 1, y * 8 + 1, 6, 6));
                    scene.addRect(QRectF(x * 8 + 2, y * 8 + 2, 4, 4));
                    scene.addRect(QRectF(x * 8 + 3, y * 8 + 3, 2, 2));
                    scene.addRect(QRectF(x * 8 + 4, y * 8 + 4, 1, 1));
                }
            }
        }
    }
}

showqr::~showqr()
{
    delete ui;
}



void showqr::on_savePushButton_clicked()
{
    QString imagePath = QFileDialog::getSaveFileName(
    this,
    tr("Save File"),
    idText,
    tr("PNG (*.png);;JPEG (*.jpg *.jpeg)" )
    );
#ifdef Q_OS_UNIX
    QFileInfo fi(imagePath);
    if(!imagePath.isNull() && !fi.suffix().contains("jpg") && !fi.suffix().contains("jpeg") && !fi.suffix().contains("png")) {
        imagePath.append(".png");
    }
#endif
    if (!imagePath.isNull())
    {
        QPixmap pixMap = this->ui->graphicsView->grab();
        if(pixMap.save(imagePath)) {
            qDebug() << "INFO: QR code saved successfully." << endLine;
        } else {
            qDebug() << "ERROR: Saving QR code." << endLine;
        }
    }
}
