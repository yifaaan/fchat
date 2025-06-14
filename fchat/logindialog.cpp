#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPainter>
#include <QPainterPath>

#include "httpmgr.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    InitHead();
    // 点击注册按钮后，LoginDialog收到clicked信号，然后发送SwitchToRegister信号给MainWindow
    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::SwitchToRegister);

    ui->forget_label->SetState("normal", "hover", "", "selected", "selected_hover", "");
    connect(ui->forget_label, &ClickLabel::clicked, this, &LoginDialog::slot_forget_pwd);

    // 收到登陆的回包
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this, &LoginDialog::slot_login_mod_finish);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::slot_forget_pwd()
{
    qDebug() << "Login dialog Switch to reset";
    emit SwitchToReset();
}

void LoginDialog::slot_login_mod_finish(ReqId id, const QString& res, ErrorCodes err)
{
    if (err != ErrorCodes::kSuccess)
    {
        ShowTip(tr("网络请求错误"), false);
        return;
    }
    auto json_doc = QJsonDocument::fromJson(res.toUtf8());
    if (json_doc.isNull() || !json_doc.isObject())
    {
        ShowTip(tr("json 解析错误"), false);
        return;
    }
    handlers_[id](json_doc.object());
}



void LoginDialog::ShowTip(const QString& str, bool ok)
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

void LoginDialog::InitHead()
{
    QPixmap pix_map{":/app_login"};
    pix_map = pix_map.scaled(ui->head_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap rounded_pix_map{pix_map.size()};
    rounded_pix_map.fill(Qt::transparent);

    QPainter painter{&rounded_pix_map};
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addRoundedRect(0, 0, pix_map.width(), pix_map.height(), 10, 10);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pix_map);
    ui->head_label->setPixmap(rounded_pix_map);
}

void LoginDialog::InitHttpHandlers()
{
    handlers_.insert(ReqId::kUserLogin, [this](const QJsonObject& json)
                     {
        auto error = static_cast<ErrorCodes>(json["error"].toInt());
        if (error != ErrorCodes::kSuccess)
        {
            ShowTip(tr("参数错误"), false);
            return;
        }
        auto user = json["user"].toString();
        ShowTip(tr("登陆成功"), true);
        qDebug() << "user is " << user;
    });
}

bool LoginDialog::CheckUserValid()
{
    if (ui->user_edit->text().isEmpty())
    {
        qDebug() << "user empty";
        return false;
    }
    return true;
}

bool LoginDialog::CheckPasswdValid()
{
    auto pwd = ui->pass_edit->text();
    if (pwd.length() < 6 || pwd.length() > 15)
    {
        qDebug() << "密码长度应为6~15";
        return false;
    }
    return true;
}

void LoginDialog::on_login_btn_clicked()
{
    qDebug() << "longin button clicked";
    if (!CheckPasswdValid() || !CheckUserValid())
    {
        return;
    }
    auto user = ui->user_edit->text();
    auto pwd = ui->pass_edit->text();
    QJsonObject json;
    json["user"] = user;
    json["pwd"] = pwd;
    HttpMgr::GetInstance()->PostHttpReq(QUrl{GateUrlPrefix + "/user_login"}, json, Modules::kLoginMod, ReqId::kUserLogin);
}

