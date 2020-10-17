#ifndef HYSTORYWINDOW_H
#define HYSTORYWINDOW_H

#include <QDialog>

namespace Ui {
class historyWindow;
}

class historyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit historyWindow(QWidget *parent = nullptr);
    ~historyWindow();

private:
    Ui::historyWindow *ui;
};

#endif // HYSTORYWINDOW_H
