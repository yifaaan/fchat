#include "http_connection.h"

#include "logic_system.h"

namespace {

unsigned char ToHex(unsigned char x) { return x > 9 ? x + 55 : x + 48; }

unsigned char FromHex(unsigned char x) {
  if (x >= 'A' && x <= 'F') return x - 'A' + 10;
  if (x >= 'a' && x <= 'f') return x - 'a' + 10;
  if (x >= '0' && x <= '9') return x - '0';
  return x - '0';
}

[[maybe_unused]]
std::string UrlEncode(const std::string& str) {
  std::string result;
  for (size_t i = 0; i < str.size(); i++) {
    if (std::isalnum(str[i]) || str[i] == '-' || str[i] == '_' || str[i] == '.' || str[i] == '~') {
      result += str[i];
    } else if (str[i] == ' ') {
      result += '+';
    } else {
      result += '%';
      result += ToHex(str[i] >> 4);
      result += ToHex(str[i] & 0xf);
    }
  }
  return result;
}

std::string UrlDecode(const std::string& str) {
  std::string result;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == '+') {
      result += ' ';
    } else if (str[i] == '%') {
      assert(i + 2 < str.size());
      result += FromHex(str[i + 1]) << 4 | FromHex(str[i + 2]);
      i += 2;
    } else {
      result += str[i];
    }
  }
  return result;
}

}  // namespace

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
    ParseGetParams();
    auto success = LogicSystem::GetInstance()->HandleGet(get_url_, shared_from_this());
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

  if (request_.method() == http::verb::post) {
    auto success = LogicSystem::GetInstance()->HandlePost(request_.target(), shared_from_this());
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

void HttpConnection::WriteResponse() {
  response_.content_length(response_.body().size());
  http::async_write(socket_, response_,
                    [self = shared_from_this()](const beast::error_code& ec, std::size_t bytes_transferred) {
                      boost::ignore_unused(bytes_transferred);
                      self->socket_.shutdown(tcp::socket::shutdown_send);
                      self->deadline_.cancel();
                    });
}

void HttpConnection::CheckDeadline() {
  deadline_.async_wait([self = shared_from_this()](const beast::error_code& ec) {
    if (!ec) {
      self->socket_.close();
      return;
    }
  });
}

void HttpConnection::ParseGetParams() {
  auto uri = request_.target();
  auto query_pos = uri.find('?');
  if (query_pos == std::string::npos) {
    get_url_ = uri;
    return;
  }
  get_url_ = uri.substr(0, query_pos);
  auto query_str = uri.substr(query_pos + 1);
  std::string key, value;
  std::size_t pos = 0;
  while ((pos = query_str.find('&')) != std::string::npos) {
    auto pair = query_str.substr(0, pos);
    auto eq_pos = pair.find('=');
    if (eq_pos != std::string::npos) {
      key = UrlDecode(pair.substr(0, eq_pos));
      value = UrlDecode(pair.substr(eq_pos + 1));
      get_params_[key] = value;
    }
    query_str = query_str.substr(pos + 1);
  }
  if (!query_str.empty()) {
    auto eq_pos = query_str.find('=');
    if (eq_pos != std::string::npos) {
      key = UrlDecode(query_str.substr(0, eq_pos));
      value = UrlDecode(query_str.substr(eq_pos + 1));
      get_params_[key] = value;
    }
  }
}