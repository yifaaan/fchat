#include "httpmgr.h"

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, const QJsonObject& json, Modules mod, ReqId req_id)
{
    QByteArray data = QJsonDocument{json}.toJson();
    QNetworkRequest request{url};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    qDebug() << url;
    auto reply = manager_.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, [self = shared_from_this(), reply, mod, req_id]
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            emit self->sig_http_finish(mod, req_id, "", ErrorCodes::kErrNetwork);
            reply->deleteLater();
            return;
        }
        QString res = reply->readAll();
        emit self->sig_http_finish(mod, req_id, res, ErrorCodes::kSuccess);
        reply->deleteLater();
    });
}

void HttpMgr::slot_http_finish(Modules mod, ReqId id, const QString &res, ErrorCodes err)
{
    if (mod == Modules::kRegisterMod)
    {
        // 通知对应模块其http响应已经收到
        emit sig_reg_mod_finish(id, res, err);
    }
    else if (mod == Modules::kResetPwdMod)
    {
        emit sig_rest_mod_finish(id, res, err);
    }
    else if (mod == Modules::kLoginMod)
    {
        emit sig_login_mod_finish(id, res, err);
    }
}
