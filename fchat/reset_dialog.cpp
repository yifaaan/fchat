#include "reset_dialog.h"
#include "ui_resetdialog.h"
#include "httpmgr.h"

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    ui->err_label->clear();

    connect(ui->user_edit, &QLineEdit::editingFinished, [this]
    {
        CheckUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, [this]
    {
        CheckEmailValid();
    });
    connect(ui->pass_edit, &QLineEdit::editingFinished, [this]
    {
        CheckPasswdValid();
    });
    connect(ui->verify_edit, &QLineEdit::editingFinished, [this]
    {
        CheckVerifyValid();
    });
    InitHttpHandlers();

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &ResetDialog::slot_reset_mod_finish);
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

void ResetDialog::InitHttpHandlers()
{
    // 验证码
    handlers_.insert(ReqId::kGetVarifyCode, [this](const QJsonObject& json)
    {
        auto error = static_cast<ErrorCodes>(json["error"].toInt());
        qDebug() << "gate server error is: " << static_cast<int>(error);
        if (error != ErrorCodes::kSuccess)
        {
             ShowTip(tr("参数错误"), false);
             return;
        }
        auto email = json["email"].toString();
        ShowTip(tr("验证码已经发送到邮箱，注意查收"), true);
        qDebug() << "email is " << email;
    });

    // 重置密码回包
    handlers_.insert(ReqId::kResetPwd, [this](const QJsonObject& json)
     {
         auto error = static_cast<ErrorCodes>(json["error"].toInt());
         qDebug() << "gate server error is: " << static_cast<int>(error);
         if (error != ErrorCodes::kSuccess)
         {
             ShowTip(tr("参数错误"), false);
             return;
         }
         auto email = json["email"].toString();
         ShowTip(tr("重置密码成功"), true);
         qDebug() << "email is " << email;
         qDebug() << "uuid is " << json["uuid"].toString();
     });
}

void ResetDialog::ShowTip(const QString &str, bool ok)
{
    if (ok)
    {
        ui->err_label->setProperty("state", "normal");
    }
    else
    {
        ui->err_label->setProperty("state", "err");
    }
    ui->err_label->setText(str);
    Repolish(ui->err_label);
}


void ResetDialog::AddTipErr(TipErr err, const QString& msg)
{
    tip_errors_[err] = msg;
    ShowTip(msg, false);
}

void ResetDialog::DelTipErr(TipErr err)
{
    tip_errors_.remove(err);
    if (tip_errors_.empty())
    {
        ui->err_label->clear();
        return;
    }
    ShowTip(tip_errors_.first(), false);
}


bool ResetDialog::CheckUserValid()
{
    if (ui->user_edit->text().isEmpty())
    {
        AddTipErr(TipErr::kTipUserErr, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::kTipUserErr);
    return true;
}

bool ResetDialog::CheckEmailValid()
{
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})");
    auto match = regex.match(email);
    if (!match.hasMatch())
    {
        AddTipErr(TipErr::kTipEmailErr, tr("邮箱格式错误"));
        return false;
    }
    DelTipErr(TipErr::kTipEmailErr);
    return true;
}

bool ResetDialog::CheckPasswdValid()
{
    auto pwd = ui->pass_edit->text();
    if (pwd.length() < 6 || pwd.length() > 15)
    {
        AddTipErr(TipErr::kTipPasswdErr, tr("密码长度应为6~15"));
        return false;
    }
    QRegularExpression regex{"^[a-zA-Z0-9!@#$%^&*]{6,15}$"};
    auto match = regex.match(pwd);
    if (!match.hasMatch())
    {
        AddTipErr(TipErr::kTipPasswdErr, tr("不能包含非法字符"));
        return false;
    }
    DelTipErr(TipErr::kTipPasswdErr);
    return true;
}

bool ResetDialog::CheckVerifyValid()
{
    auto code = ui->verify_edit->text();
    if (code.isEmpty())
    {
        AddTipErr(TipErr::kTipVerifyErr, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::kTipVerifyErr);
    return true;
}

void ResetDialog::on_confirm_btn_clicked()
{
    if (!CheckUserValid() || !CheckEmailValid() || !CheckPasswdValid() || !CheckVerifyValid())
    {
        return;
    }

    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = XorString(ui->pass_edit->text());
    json_obj["verify_code"] = ui->verify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl{GateUrlPrefix + "/reset_pwd"}, json_obj, Modules::kResetPwdMod, ReqId::kResetPwd);
}


void ResetDialog::on_verify_btn_clicked()
{
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})");
    auto match = regex.match(email);
    if (match.hasMatch())
    {
        // post get verify code
        QJsonObject json_object;
        json_object["email"] = email;
        ShowTip(tr("验证码已经发送到邮箱，注意查收"), true);
        HttpMgr::GetInstance()->PostHttpReq(QUrl(GateUrlPrefix + "/get_verifycode"), json_object, Modules::kResetPwdMod, ReqId::kGetVarifyCode);
    }
    else
    {
        ShowTip(tr("邮箱格式错误"), false);
    }
}

void ResetDialog::slot_reset_mod_finish(ReqId id, const QString &res, ErrorCodes err)
{
    if (err != ErrorCodes::kSuccess)
    {
        ShowTip(tr("网络请求错误"), false);
        return;
    }

    // 将字节流转化成json
    QJsonDocument json_doc = QJsonDocument::fromJson(res.toUtf8());
    if (json_doc.isNull())
    {
        ShowTip(tr("json解析失败"), false);
        return;
    }
    if (json_doc.isObject())
    {
        ShowTip(tr("json解析失败"), false);
    }
    // 执行回调
    handlers_[id](json_doc.object());
}


void ResetDialog::on_return_btn_clicked()
{
    emit SigSwitchToLogin();
}

