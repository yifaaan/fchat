#ifndef RESET_DIALOG_H
#define RESET_DIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

signals:
    void SigSwitchToLogin();

private slots:
    void on_confirm_btn_clicked();

    void on_verify_btn_clicked();
    void slot_reset_mod_finish(ReqId id, const QString &res, ErrorCodes err);
    void on_return_btn_clicked();

private:
    void InitHttpHandlers();

    void ShowTip(const QString& str, bool ok);

    void AddTipErr(TipErr err, const QString& msg);
    void DelTipErr(TipErr err);

    bool CheckUserValid();
    bool CheckEmailValid();
    bool CheckPasswdValid();
    bool CheckVerifyValid();

    Ui::ResetDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;
    QMap<TipErr, QString> tip_errors_;
};

#endif // RESET_DIALOG_H
