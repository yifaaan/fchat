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

    connect(login_dialog_, &LoginDialog::SwitchToReset, this,  &MainWindow::SlotSwitchToReset);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchToRegister()
{
    // 注册界面
    register_dialog_ = new RegisterDialog{this};

    register_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    // register_dialog_->hide();
    setCentralWidget(register_dialog_);
    // 注册界面返回到登陆界面
    connect(register_dialog_, &RegisterDialog::sig_switch_login, this, &MainWindow::SlotRegisterSwitchToLogin);
    // 将当前界面设置为注册界面

    login_dialog_->hide();
    // register_dialog_->show();
}

void MainWindow::SlotRegisterSwitchToLogin()
{
    // 登陆界面
    login_dialog_ = new LoginDialog{this};
    login_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    // 设置MainWindow当前界面
    setCentralWidget(login_dialog_);
    register_dialog_->hide();
    login_dialog_->show();
    // 收到LoginDialog的SwitchToRegister信号后，调用槽函数切换到注册界面
    connect(login_dialog_, &LoginDialog::SwitchToRegister, this, &MainWindow::SlotSwitchToRegister);
    connect(login_dialog_, &LoginDialog::SwitchToReset, this,  &MainWindow::SlotSwitchToReset);
}

void MainWindow::SlotResetSwitchToLogin()
{
    login_dialog_ = new LoginDialog{this};
    login_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    // 设置MainWindow当前界面
    setCentralWidget(login_dialog_);
    reset_dialog_->hide();
    login_dialog_->show();
    // 收到LoginDialog的SwitchToReset信号后，调用槽函数切换到重置密码界面
    connect(login_dialog_, &LoginDialog::SwitchToReset, this, &MainWindow::SlotSwitchToReset);
    connect(login_dialog_, &LoginDialog::SwitchToRegister, this, &MainWindow::SlotSwitchToRegister);
}

void MainWindow::SlotSwitchToReset()
{
    // 重置密码
    reset_dialog_ = new ResetDialog{this};
    reset_dialog_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(reset_dialog_);
    login_dialog_->hide();
    reset_dialog_->show();
    // 重置密码界面返回到登陆界面
    connect(reset_dialog_, &ResetDialog::SigSwitchToLogin, this, &MainWindow::SlotResetSwitchToLogin);
}
