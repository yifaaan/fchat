#include "registerdialog.h"
#include "ui_registerdialog.h"

#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    ui->err_label->setProperty("state", "normal");
    Repolish(ui->err_label);

    // 收到HttpMgr发来的已http响应已收到的信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);

    InitHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_btn_clicked()
{

    auto email = ui->email_edit->text();
    QRegularExpression regex(R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})");
    auto match = regex.match(email);
    if (match.hasMatch())
    {
        // post get verify code
        QJsonObject json_object;
        json_object["email"] = email;

        HttpMgr::GetInstance()->PostHttpReq(QUrl(GateUrlPrefix + "/get_verifycode"), json_object, Modules::kRegisterMod, ReqId::kGetVarifyCode);
    }
    else
    {
        ShowTip(tr("邮箱格式错误"), false);
    }
}



void RegisterDialog::slot_reg_mod_finish(ReqId id, const QString &res, ErrorCodes err)
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

void RegisterDialog::ShowTip(const QString& str, bool ok)
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

void RegisterDialog::InitHttpHandlers()
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
    // 注册
    handlers_.insert(ReqId::kRegUser, [this](const QJsonObject& json)
    {
        auto error = static_cast<ErrorCodes>(json["error"].toInt());
        qDebug() << "gate server error is: " << static_cast<int>(error);
        if (error != ErrorCodes::kSuccess)
        {
            ShowTip(tr("参数错误"), false);
            return;
        }
        auto email = json["email"].toString();
        ShowTip(tr("用户注册成功"), true);
        qDebug() << "email is " << email;
    });

}


void RegisterDialog::on_confirm_btn_clicked()
{
    if (ui->user_edit->text().isEmpty()) {
        ShowTip(tr("用户名不能为空"), false);
        return;
    }
    if (ui->email_edit->text().isEmpty()) {
        ShowTip(tr("邮箱不能为空"), false);
        return;
    }
    if (ui->pass_edit->text().isEmpty()) {
        ShowTip(tr("密码不能为空"), false);
        return;
    }
    if (ui->confirm_edit->text().isEmpty()) {
        ShowTip(tr("确认密码不能为空"), false);
        return;
    }
    if (ui->pass_edit->text() != ui->confirm_edit->text()) {
        ShowTip(tr("密码不匹配，重新输入"), false);
        return;
    }
    if (ui->verify_edit->text().isEmpty()) {
        ShowTip(tr("验证码不能为空"), false);
        return;
    }
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->pass_edit->text();
    json_obj["confirm"] = ui->confirm_edit->text();
    json_obj["verify_code"] = ui->verify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl{GateUrlPrefix + "/register_user"}, json_obj, Modules::kRegisterMod, ReqId::kRegUser);
}

