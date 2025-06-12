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
    login_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    // 设置MainWindow当前界面
    setCentralWidget(login_dialog_);


    // 收到LoginDialog的SwitchToRegister信号后，调用槽函数切换到注册界面
    connect(login_dialog_, &LoginDialog::SwitchToRegister, this, &MainWindow::SlotSwitchToRegister);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchToRegister()
{
    // 注册界面
    register_dialog_ = new RegisterDialog{this};
    register_dialog_->hide();
    register_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    // 注册界面返回到登陆界面
    connect(register_dialog_, &RegisterDialog::sig_switch_login, this, &MainWindow::SlotSwitchToLogin);
    // 将当前界面设置为注册界面
    setCentralWidget(register_dialog_);
    login_dialog_->hide();
    register_dialog_->show();
}

void MainWindow::SlotSwitchToLogin()
{
    // 登陆界面
    login_dialog_ = new LoginDialog{this};
    login_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    // 设置MainWindow当前界面
    setCentralWidget(login_dialog_);
    register_dialog_->hide();
    // 收到LoginDialog的SwitchToRegister信号后，调用槽函数切换到注册界面
    connect(login_dialog_, &LoginDialog::SwitchToRegister, this, &MainWindow::SlotSwitchToRegister);
}

