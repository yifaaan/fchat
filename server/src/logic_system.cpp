#include "logic_system.h"

#include "http_connection.h"

LogicSystem::LogicSystem() {
  RegisterGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
    beast::ostream(connection->response_.body()) << "received get_test\n";
    int i = 0;
    for (auto& [key, value] : connection->get_params_) {
      i++;
      beast::ostream(connection->response_.body()) << i << ". " << key << "=" << value << "\n";
    }
  });
}

void LogicSystem::RegisterGet(const std::string& url, HttpHandler handler) {
  get_handlers_.emplace(url, std::move(handler));
}

bool LogicSystem::HandleGet(const std::string& url, std::shared_ptr<HttpConnection> connection) {
  if (auto it = get_handlers_.find(url); it == get_handlers_.end()) {
    return false;
  } else {
    std::cout << "handle get url: " << url << std::endl;
    it->second(connection);
    return true;
  }
}