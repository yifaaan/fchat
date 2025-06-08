#include "httpmgr.h"

HttpMgr::HttpMgr() {}

void HttpMgr::PostHttpReq(QUrl url, const QJsonObject& json, Modules mod, ReqId req_id)
{
    QByteArray data = QJsonDocument{json}.toJson();
    QNetworkRequest request{url};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

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
