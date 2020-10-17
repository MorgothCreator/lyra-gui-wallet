
#include "copyed.h"
#include "ui_copyed.h"

#include "def.h"

copyed::copyed(QWidget *parent, QString) :
    QDialog(parent),
    ui(new Ui::copyed)
{
    ui->setupUi(this);
    this->parent = parent;
    copyFadeTimer.setInterval(COPY_FADE_RATIO);
    connect(&copyFadeTimer, SIGNAL(timeout()), this, SLOT(windowFade()));
    copyFadeTimer.stop();
}

copyed::~copyed()
{
    delete ui;
}

void copyed::showEvent(QShowEvent *) {
    ui->copyedLabel->setGeometry(0, 0, this->width(), this->height());
    fadeCnt = 2.0f;
    this->setProperty("windowOpacity", fadeCnt);
    copyFadeTimer.start();
}

void copyed::windowFade() {
    if(fadeCnt > 0.2f) {
        fadeCnt = fadeCnt - 0.01f;
        this->setProperty("windowOpacity", fadeCnt);
        this->repaint();
        QApplication::processEvents();
    } else {
        copyFadeTimer.stop();
        this->setProperty("windowOpacity", 0.0);
        this->close();
    }
}
