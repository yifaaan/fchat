#include "verify_grpc_client.h"

#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include "config_manager.h"

RpcConnectionPool::RpcConnectionPool(size_t size, std::string host, std::string port)
    : size_{size}, host_{std::move(host)}, port_{std::move(port)} {
  spdlog::info("Creating RPC connection pool with {} connections to {} {}", size, host_, port_);

  for (size_t i = 0; i < size; i++) {
    auto channel = grpc::CreateChannel(std::format("{}:{}", host_, port_), grpc::InsecureChannelCredentials());
    connections_.emplace(VerifyService::NewStub(channel));
  }
  spdlog::info("RPC connection pool initialized successfully");
}

void RpcConnectionPool::Close() {
  stopped_ = true;
  cond_.notify_all();
}

RpcConnectionPool::~RpcConnectionPool() {
  std::unique_lock lock{mutex_};
  Close();
  while (!connections_.empty()) {
    connections_.pop();
  }
}

std::unique_ptr<VerifyService::Stub> RpcConnectionPool::Get() {
  std::unique_lock lock{mutex_};
  cond_.wait(lock, [this] { return stopped_ || !connections_.empty(); });
  if (stopped_) {
    return nullptr;
  }
  auto stub = std::move(connections_.front());
  connections_.pop();
  return stub;
}

void RpcConnectionPool::Push(std::unique_ptr<VerifyService::Stub> stub) {
  std::lock_guard lock{mutex_};
  if (stopped_) {
    return;
  }
  connections_.push(std::move(stub));
  cond_.notify_one();
}

VerifyGrpcClient::VerifyGrpcClient() {
  auto& config_manger = ConfigManager::GetInstance();
  auto& host = config_manger["VerifyServer"]["host"];
  auto& port = config_manger["VerifyServer"]["port"];

  std::cout << "VerifyGrpcClient connecting to: " << host << ":" << port << std::endl;
  pool_ = std::make_unique<RpcConnectionPool>(8, host, port);
}

GetVerifyResponse VerifyGrpcClient::GetVerifyCode(const std::string& email) {
  ClientContext ctx;
  GetVerifyResponse response;
  GetVerifyRequest request;
  request.set_email(email);

  // Send request to verify grpc server
  auto stub = pool_->Get();
  if (!stub) {
    std::cerr << "Failed to get RPC stub from connection pool" << std::endl;
    response.set_error(static_cast<int32_t>(ErrorCodes::kRpcFailed));
    return response;
  }

  auto status = stub->GetVerifyCode(&ctx, request, &response);
  if (!status.ok()) {
    std::cerr << "RPC call failed - Code: " << status.error_code() << ", Message: " << status.error_message()
              << ", Details: " << status.error_details() << std::endl;
    response.set_error(static_cast<int32_t>(ErrorCodes::kRpcFailed));
    pool_->Push(std::move(stub));
    return response;
  }
  pool_->Push(std::move(stub));
  return response;
}