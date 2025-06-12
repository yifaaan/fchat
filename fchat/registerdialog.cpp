#include "registerdialog.h"

#include "ui_registerdialog.h"

#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
    , count_down_timer_{new QTimer{this}}
{
    ui->setupUi(this);

    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    ui->err_label->setProperty("state", "normal");
    Repolish(ui->err_label);
    ui->err_label->clear();





    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->pass_visible->SetState("unvisible","unvisible_hover","","visible",
                               "visible_hover","");
    connect(ui->pass_visible, &ClickLabel::clicked, this, [this]
    {
        if (auto state = ui->pass_visible->current_state(); state == ClickLabelState::kNormal)
        {
            ui->pass_edit->setEchoMode(QLineEdit::Password);
        }
        else
        {
            ui->pass_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Lable was clicked";
    });
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->SetState("unvisible","unvisible_hover","","visible",
                                  "visible_hover","");
    connect(ui->confirm_visible, &ClickLabel::clicked, this, [this]
    {
        if (auto state = ui->confirm_visible->current_state(); state == ClickLabelState::kNormal)
        {
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
        }
        else
        {
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Lable was clicked";
    });

    connect(count_down_timer_, &QTimer::timeout, [this]
    {
        if (count_down_ == 0)
        {
            count_down_timer_->stop();
            emit sig_switch_login();
            return;
        }
        count_down_--;
        auto str = QString{"注册成功, %1 s后返回登陆"}.arg(count_down_);
        ui->register_return_label->setText(str);
    });


    // 收到HttpMgr发来的已http响应已收到的信号
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);

    connect(ui->user_edit, &QLineEdit::editingFinished, this, [this]{ CheckUserValid(); });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this] { CheckEmailValid(); });
    connect(ui->pass_edit, &QLineEdit::editingFinished, this, [this]{ CheckPasswdValid(); });
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this]{ CheckVerifyValid(); });

    InitHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
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
    // 注册回包
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
        qDebug() << "uuid is " << json["uuid"].toString();
        ChangeTipPage();
     });
}

void RegisterDialog::ChangeTipPage()
{
    count_down_timer_->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    count_down_timer_->start(1000);
}


void RegisterDialog::on_confirm_btn_clicked()
{
    if (!CheckUserValid() || !CheckEmailValid() || !CheckPasswdValid() || !CheckVerifyValid()) {
        return;
    }

    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = XorString(ui->pass_edit->text());
    json_obj["confirm"] = XorString(ui->confirm_edit->text());
    json_obj["verify_code"] = ui->verify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl{GateUrlPrefix + "/register_user"}, json_obj, Modules::kRegisterMod, ReqId::kRegUser);
}

void RegisterDialog::AddTipErr(TipErr err, const QString& msg)
{
    tip_errors_[err] = msg;
    ShowTip(msg, false);
}

void RegisterDialog::DelTipErr(TipErr err)
{
    tip_errors_.remove(err);
    if (tip_errors_.empty())
    {
        ui->err_label->clear();
        return;
    }
    ShowTip(tip_errors_.first(), false);
}


bool RegisterDialog::CheckUserValid()
{
    if (ui->user_edit->text().isEmpty())
    {
        AddTipErr(TipErr::kTipUserErr, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::kTipUserErr);
    return true;
}

bool RegisterDialog::CheckEmailValid()
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

bool RegisterDialog::CheckPasswdValid()
{
    auto pwd = ui->pass_edit->text();
    auto confirm = ui->confirm_edit->text();
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
    if (pwd != confirm)
    {
        AddTipErr(TipErr::kTipPasswdConfirmErr, tr("密码和确认密码不匹配"));
        return false;
    }
    DelTipErr(TipErr::kTipPasswdConfirmErr);
    return true;
}

bool RegisterDialog::CheckVerifyValid()
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

void RegisterDialog::on_register_return_btn_clicked()
{
    count_down_timer_->stop();
    emit sig_switch_login();
}


void RegisterDialog::on_cancel_btn_clicked()
{
    count_down_timer_->stop();
    emit sig_switch_login();
}


void RegisterDialog::on_get_code_btn_clicked()
{
    qDebug() << "FDSFSDFSDFSD";
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

