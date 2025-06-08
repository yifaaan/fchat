#pragma once

#include "const.h"
#include "singleton.h"

class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;

class LogicSystem : public Singleton<LogicSystem> {
 public:
  ~LogicSystem() = default;

  bool HandleGet(const std::string& url, std::shared_ptr<HttpConnection> connection);

  void RegisterGet(const std::string& url, HttpHandler handler);

 private:
  friend class Singleton<LogicSystem>;

  LogicSystem();

  std::map<std::string, HttpHandler> post_handlers_;
  std::map<std::string, HttpHandler> get_handlers_;
};