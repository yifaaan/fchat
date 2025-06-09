#include "cserver.h"

#include "http_connection.h"

CServer::CServer(net::io_context& ioc, unsigned short port)
    : ioc_{ioc}, acceptor_{ioc, tcp::endpoint{net::ip::address_v4::loopback(), port}}, socket_{ioc} {}

void CServer::Start() {
  acceptor_.async_accept(socket_, [self = shared_from_this()](const beast::error_code& ec) {
    try {
      if (ec) {
        // error: continue to listen for other connections
        self->Start();
        return;
      }
      std::make_shared<HttpConnection>(std::move(self->socket_))->Start();
      self->Start();
    } catch (const std::exception& e) {
    }
  });
}