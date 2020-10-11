#ifndef PASSWORD_H
#define PASSWORD_H

#include <QDialog>

namespace Ui {
class password;
}

class password : public QDialog
{
    Q_OBJECT

public:
    explicit password(QWidget *parent = nullptr);
    ~password();
    QString getPass();
    bool getAccepted();

private:
    Ui::password *ui;
    QString pass;
    bool accepted;
protected:
    void showEvent(QShowEvent *) override;
private slots:
    void on_selectionButtonBox_accepted();
    void on_lineEdit_textChanged(const QString &arg1);
};

#endif // PASSWORD_H
