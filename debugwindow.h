#ifndef DEBUG_H
#define DEBUG_H

#include <QDialog>

namespace Ui {
class debugWindow;
}

class debugWindow : public QDialog
{
    Q_OBJECT

public:
    explicit debugWindow(QWidget *parent = nullptr);
    ~debugWindow();
    void append(QString text);
    void append(QStringList text);

private:
    Ui::debugWindow *ui;
};

#endif // DEBUG_H
