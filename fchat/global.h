#ifndef GLOBAL_H
#define GLOBAL_H


#include <functional>
#include <mutex>
#include <iostream>
#include <QWidget>
#include <QRegularExpression>
#include <QStyle>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QSettings>


// 刷新qss
inline std::function<void(QWidget*)> Repolish = [](QWidget* w) {
    w->style()->unpolish(w);
    w->style()->polish(w);
};

enum class ReqId
{
    kGetVarifyCode = 1001,// 获取验证码
    kRegUser = 1002, // 注册用户
};

enum class Modules
{
    kRegisterMod ,
};

enum class ErrorCodes
{
    kSuccess,
    kErrJson,
    kErrNetwork
};

inline QString GateUrlPrefix{""};

#endif // GLOBAL_H
