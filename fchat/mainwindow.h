#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "reset_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // 收到logindialog的SwitchToRegister信号后，需要切换到注册界面
    void SlotSwitchToRegister();
    void SlotRegisterSwitchToLogin();
    void SlotResetSwitchToLogin();
    void SlotSwitchToReset();
private:
    Ui::MainWindow *ui;

    LoginDialog* login_dialog_;
    RegisterDialog* register_dialog_;
    ResetDialog* reset_dialog_;
};
#endif // MAINWINDOW_H
