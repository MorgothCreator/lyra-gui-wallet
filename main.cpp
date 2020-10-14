#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("resources/ico/window_32x32.png"));
#ifdef Q_OS_UNIX
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    QCoreApplication::addLibraryPath("/snap/gnome-3-34-1804/36/usr/lib/x86_64-linux-gnu");
#endif
    MainWindow w;
    w.show();
    return a.exec();
}
