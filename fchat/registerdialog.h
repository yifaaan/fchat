#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H


#include <QDialog>
#include <QTimer>

#include "global.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_get_code_btn_clicked();
    // 处理http响应
    void slot_reg_mod_finish(ReqId id, const QString& res, ErrorCodes err);

    void on_confirm_btn_clicked();

    void on_register_return_btn_clicked();

    void on_cancel_btn_clicked();

signals:
    void sig_switch_login();

private:
    void ShowTip(const QString& str, bool ok);
    // 初始化http响应的回调函数
    void InitHttpHandlers();


    void AddTipErr(TipErr err, const QString& msg);
    void DelTipErr(TipErr err);

    bool CheckUserValid();
    bool CheckEmailValid();
    bool CheckPasswdValid();
    bool CheckVerifyValid();

    void ChangeTipPage();

    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;

    QMap<TipErr, QString> tip_errors_;

    QTimer* count_down_timer_;
    int count_down_{5};
};

#endif // REGISTERDIALOG_H
