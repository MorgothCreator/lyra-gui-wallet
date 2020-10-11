#ifndef DEBUG_H
#define DEBUG_H

#include <QDialog>

namespace Ui {
class debug;
}

class debug : public QDialog
{
    Q_OBJECT

public:
    explicit debug(QWidget *parent = nullptr);
    ~debug();
    void append(QString text);
    void append(QStringList text);

private:
    Ui::debug *ui;
};

#endif // DEBUG_H
