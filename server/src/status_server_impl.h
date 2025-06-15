#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::StatusService;
using message::GetChatServerRequest;
using message::GetChatServerResponse;


struct ChatServer {
  std::string host;
  std::string port;
};

class StatusServerImpl final : public StatusService::Service {
 public:
  StatusServerImpl();

  Status GetChatServer(ServerContext* context, const GetChatServerRequest* request, GetChatServerResponse* response) override;

  std::vector<ChatServer> chat_servers_;
  int server_idx_;
};