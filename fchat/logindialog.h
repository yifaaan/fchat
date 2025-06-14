#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

signals:
    void SwitchToRegister();
    void SwitchToReset();

public slots:
    void slot_forget_pwd();

private:
    void InitHead();

    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
