#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"


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
    void slot_login_mod_finish(ReqId id, const QString& res, ErrorCodes err);

private slots:
    void on_login_btn_clicked();

private:
    void ShowTip(const QString& str, bool ok);
    void InitHead();
    void InitHttpHandlers();
    bool CheckUserValid();
    bool CheckPasswdValid();

    Ui::LoginDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;
};

#endif // LOGINDIALOG_H
