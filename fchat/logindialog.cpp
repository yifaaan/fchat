#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // 点击注册按钮后，LoginDialog收到clicked信号，然后发送SwitchToRegister信号给MainWindow
    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::SwitchToRegister);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
