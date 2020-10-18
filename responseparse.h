#ifndef RESPONSEPARSE_H
#define RESPONSEPARSE_H

#include <QMainWindow>
#include <QObject>

class responseParse : public QMainWindow
{
    Q_OBJECT
public:
    //responseParse();
    //~responseParse();

    static QString print(QString text, QString line);
    static QStringList print(QString text, QStringList line);
};

#endif // RESPONSEPARSE_H
