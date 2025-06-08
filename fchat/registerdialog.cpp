#include "registerdialog.h"
#include "ui_registerdialog.h"

#include "global.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);

    ui->err_label->setProperty("state", "normal");
    Repolish(ui->err_label);
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

