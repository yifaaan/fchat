#include "http_connection.h"

HttpConnection::HttpConnection(tcp::socket socket) : socket_{std::move(socket)} {}

void HttpConnection::Start() {
  http::async_read(socket_, buffer_, request_,
                   [self = shared_from_this()](const beast::error_code& ec, std::size_t bytes_transferred) {
                     try {
                       if (ec) {
                         std::cerr << "Error reading request: " << ec.what() << std::endl;
                         return;
                       }
                       boost::ignore_unused(bytes_transferred);
                       self->HandleRequest();
                       self->CheckDeadline();
                     } catch (const std::exception& e) {
                       std::cerr << "Error handling request: " << e.what() << std::endl;
                       self->socket_.close();
                     }
                   });
}

void HttpConnection::HandleRequest() {
  response_.version(request_.version());
  response_.keep_alive(false);
  if (request_.method() == http::verb::get) {
    auto success = LogicSystem::GetInstance()->HandleGet(request_.target(), shared_from_this());
    if (!success) {
      response_.result(http::status::not_found);
      response_.set(http::field::content_type, "text/plain");
      beast::ostream(response_.body()) << "url not found";
      WriteResponse();
      return;
    }
    response_.result(http::status::ok);
    response_.set(http::field::server, "GateServer");
    WriteResponse();
    return;
  }
}