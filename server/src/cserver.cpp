#include "cserver.h"

#include "http_connection.h"
#include "io_context_pool.h"

CServer::CServer(net::io_context& ioc, unsigned short port)
    : ioc_{ioc}, acceptor_{ioc, tcp::endpoint{net::ip::address_v4::loopback(), port}} {}

void CServer::Start() {
  auto& ctx = IoContextPool::GetInstance()->GetIoContext();
  auto new_connection = std::make_shared<HttpConnection>(ctx);
  acceptor_.async_accept(new_connection->socket(),
                         [self = shared_from_this(), new_connection](const beast::error_code& ec) {
                           try {
                             if (ec) {
                               // error: continue to listen for other connections
                               self->Start();
                               return;
                             }
                             new_connection->Start();
                             self->Start();
                           } catch (const std::exception& e) {
                             // TODO:
                           }
                         });
}