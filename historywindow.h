#ifndef HYSTORYWINDOW_H
#define HYSTORYWINDOW_H

#include <QDialog>
#include <QIcon>

namespace Ui {
class historyWindow;
}

class historyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit historyWindow(QWidget *parent = nullptr, QString text = nullptr, QString title = nullptr, QIcon *icon = nullptr);
    ~historyWindow();

private:
    Ui::historyWindow *ui;
};

#endif // HYSTORYWINDOW_H
