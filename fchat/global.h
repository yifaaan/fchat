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
inline auto Repolish = [](QWidget* w) {
    w->style()->unpolish(w);
    w->style()->polish(w);
};

inline auto XorString = [](const QString& str)
{
    auto result = str;
    auto len = str.length() % 255;
    for (int i = 0; i < len; i++)
    {
        result[i] = QChar{str[i].unicode() ^ static_cast<char16_t>(len)};
    }
    return result;
};

enum class ReqId
{
    kGetVarifyCode = 1001,// 获取验证码
    kRegUser = 1002, // 注册用户
    kResetPwd = 1003, // 重置密码
};

enum class Modules
{
    kRegisterMod,
    kResetPwdMod,
};

enum class ErrorCodes
{
    kSuccess,
    kErrJson,
    kErrNetwork,
};

enum class TipErr
{
    kTipSuccess,
    kTipEmailErr,
    kTipPasswdErr,
    kTipConfirmERr,
    kTipPasswdConfirmErr,
    kTipVerifyErr,
    kTipUserErr,
};

enum class ClickLabelState
{
    kNormal,
    kSelect,
};

inline QString GateUrlPrefix{""};

#endif // GLOBAL_H
