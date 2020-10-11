#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("resources/ico/window_32x32.png"));
    MainWindow w;
    w.show();
    return a.exec();
}
