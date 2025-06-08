#include "cserver.h"

CServer::CServer(net::io_context& ioc, unsigned short port)
    : ioc_{ioc}, acceptor_{ioc, tcp::endpoint{tcp::v4(), port}}, socket_{ioc} {}

void CServer::Start() {
  acceptor_.async_accept(socket_, [self = shared_from_this()](const beast::error_code& ec) {
    try {
      if (ec) {
        // 出错就继续监听其他连接
        self->Start();
        return;
      }
      std::make_shared<HttpConnection>(std::move(self->socket_))->Start();
      self->Start();
    } catch (const std::exception& e) {
    }
  });
}