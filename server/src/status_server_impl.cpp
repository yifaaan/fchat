#include "status_server_impl.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h>

#include "config_manager.h"
#include "const.h"


namespace {
std::string GenerateUniqueString() {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  return boost::uuids::to_string(uuid);
}
}  // namespace

StatusServerImpl::StatusServerImpl() : server_idx_{0} {
  const auto& config_manager = ConfigManager::GetInstance();

  ChatServer chat_server;
  chat_server.host = config_manager["ChatServer1"]["host"];
  chat_server.port = config_manager["ChatServer1"]["port"];
  chat_servers_.push_back(chat_server);

  chat_server.host = config_manager["ChatServer2"]["host"];
  chat_server.port = config_manager["ChatServer2"]["port"];
  chat_servers_.push_back(chat_server);
}

Status StatusServerImpl::GetChatServer(ServerContext* context, const GetChatServerRequest* request,
                                       GetChatServerResponse* response) {
  server_idx_ = (server_idx_++) % chat_servers_.size();
  auto& server = chat_servers_[server_idx_];
  response->set_host(server.host);
  response->set_port(server.port);
  response->set_error(static_cast<int32_t>(ErrorCodes::kSuccess));
  response->set_token(GenerateUniqueString());
  spdlog::info("in StatusServerImpl::GetChatServer success");
  return Status::OK;
}