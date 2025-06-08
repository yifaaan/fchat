#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/app_icon"));

    // 登陆界面
    login_dialog_ = new LoginDialog{this};
    // 设置MainWindow当前界面
    setCentralWidget(login_dialog_);

    // 注册界面
    register_dialog_ = new RegisterDialog{this};
    // 收到LoginDialog的SwitchToRegister信号后，调用槽函数切换到注册界面
    connect(login_dialog_, &LoginDialog::SwitchToRegister, this, &MainWindow::SlotSwitchToRegister);

    login_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    register_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchToRegister()
{
    // 将当前界面设置为注册界面
    setCentralWidget(register_dialog_);
    login_dialog_->hide();
}
