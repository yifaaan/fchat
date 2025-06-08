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
        // send code
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
    handlers_.insert(ReqId::kGetVarifyCode, [this](const QJsonObject& json)
    {
        auto error = static_cast<ErrorCodes>(json["error"].toInt());
        if (error != ErrorCodes::kSuccess)
        {
            ShowTip(tr("参数错误"), false);
            return;
        }
        auto email = json["email"].toString();
        ShowTip(tr("验证码已经发送到邮箱，注意查收"), true);
        qDebug() << "email is " << email;
    });
}

