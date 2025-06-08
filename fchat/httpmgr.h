#ifndef HTTPMGR_H
#define HTTPMGR_H

#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

#include "singleton.h"

class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

public:
    ~HttpMgr() = default;
private:
    friend class Singleton<HttpMgr>;
    HttpMgr();


    void PostHttpReq(QUrl url, const QJsonObject& json, Modules mod, ReqId req_id);

    QNetworkAccessManager manager_;

private slots:
    void slot_http_finish(Modules mod, ReqId id, const QString& res, ErrorCodes err);

signals:
    void sig_http_finish(Modules mod, ReqId id, const QString& res, ErrorCodes err);
    // 通知注册模块已收到其http响应
    void sig_reg_mod_finish(ReqId id, const QString& res, ErrorCodes err);
};

#endif // HTTPMGR_H
