#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPainter>
#include <QPainterPath>

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
