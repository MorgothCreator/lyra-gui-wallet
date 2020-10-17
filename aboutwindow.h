#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui {
class aboutWindow;
}

class aboutWindow : public QDialog
{
    Q_OBJECT

public:
    explicit aboutWindow(QWidget *parent = nullptr);
    ~aboutWindow();

private:
    Ui::aboutWindow *ui;
};

#endif // ABOUT_H
