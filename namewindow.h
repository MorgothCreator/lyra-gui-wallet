#ifndef LOAD_H
#define LOAD_H

#include <QDialog>

namespace Ui {
class nameWindow;
}

class nameWindow : public QDialog
{
    Q_OBJECT

public:
    explicit nameWindow(QWidget *parent = nullptr);
    ~nameWindow();
    QString getName() {
        return wallName;
    }

    bool getOk() {
        return ok;
    }

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_nameLineEdit_textChanged(const QString &arg1);

private:
    Ui::nameWindow *ui;
    QString wallName;
    bool ok;
};

#endif // LOAD_H
