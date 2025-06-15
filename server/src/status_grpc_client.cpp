#include "status_grpc_client.h"

#include <spdlog/spdlog.h>

#include "config_manager.h"

StatusConnectionPool::StatusConnectionPool(size_t size, std::string host, std::string port)
    : size_{size}, host_{std::move(host)}, port_{std::move(port)} {
  spdlog::info("StatusConnectionPool connecting to: {} {}", host_, port_);
  for (int i = 0; i < size; i++) {
    auto channel = grpc::CreateChannel(std::format("{}:{}", host_, port_), grpc::InsecureChannelCredentials());
    connections_.emplace(StatusService::NewStub(channel));
  }
  spdlog::info("StatusConnectionPool initialized successfully");
}

std::unique_ptr<StatusService::Stub> StatusConnectionPool::Get() {
    std::unique_lock lock{mutex_};
    cond_.wait(lock, [this] { return stopped_ || !connections_.empty(); });
    if (stopped_) {
        return nullptr;
    }
    auto stub = std::move(connections_.front());
    connections_.pop();
    return stub;
}

void StatusConnectionPool::Push(std::unique_ptr<StatusService::Stub> stub) {
    std::lock_guard lock{mutex_};
    if (stopped_) {
        return;
    }
    connections_.push(std::move(stub));
    cond_.notify_one();
}

void StatusConnectionPool::Close() {
  stopped_ = true;
  cond_.notify_all();
}

StatusConnectionPool::~StatusConnectionPool() {
  std::unique_lock lock{mutex_};
  Close();
  while (!connections_.empty()) {
    connections_.pop();
  }
}

StatusGrpcClient::StatusGrpcClient() {
  const auto& config_manager = ConfigManager::GetInstance();
  const auto& host = config_manager["StatusServer"]["host"];
  const auto& port = config_manager["StatusServer"]["port"];
  pool_ = std::make_unique<StatusConnectionPool>(8, host, port);
}

GetChatServerResponse StatusGrpcClient::GetChatServer(int32_t uid) {
  ClientContext ctx;
  GetChatServerResponse response;
  GetChatServerRequest request;
  request.set_uid(uid);

  // Send request to verify grpc server
  auto stub = pool_->Get();
  if (!stub) {
    spdlog::error("Failed to get RPC stub from connection pool");
    response.set_error(static_cast<int32_t>(ErrorCodes::kRpcFailed));
    return response;
  }

  auto status = stub->GetChatServer(&ctx, request, &response);
  if (!status.ok()) {
    spdlog::error("RPC call failed - Code: {}, Message: {}, Details: {}", static_cast<int>(status.error_code()), status.error_message(), status.error_details());
    response.set_error(static_cast<int32_t>(ErrorCodes::kRpcFailed));
    pool_->Push(std::move(stub));
    return response;
  }
  pool_->Push(std::move(stub));
  return response;
}