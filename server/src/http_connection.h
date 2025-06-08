#pragma once

#include "const.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
 public:
  HttpConnection(tcp::socket socket);

  void Start();

 private:
  friend class LogicSystem;

  void CheckDeadline();
  void WriteResponse();
  void HandleRequest();

  tcp::socket socket_;
  beast::flat_buffer buffer_{8192};
  http::request<http::dynamic_body> request_;
  http::response<http::dynamic_body> response_;
  net::steady_timer deadline_{socket_.get_executor(), std::chrono::seconds(60)};
};