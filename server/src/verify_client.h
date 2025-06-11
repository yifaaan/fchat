#pragma once
#include "win32_lean_mean.h"
#include <grpcpp/grpcpp.h>

#include <queue>

#include "const.h"
#include "message.grpc.pb.h"
#include "singleton.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using message::GetVerifyRequest;
using message::GetVerifyResponse;
using message::VerifyService;

class RpcConnectionPool {
 public:
  RpcConnectionPool(size_t size, std::string host, std::string port);
  ~RpcConnectionPool();

  void Close();

  std::unique_ptr<VerifyService::Stub> Get();
  void Push(std::unique_ptr<VerifyService::Stub> stub);
  size_t size() { return size_; }

 private:
  std::atomic<bool> stopped_;
  size_t size_;
  std::string host_;
  std::string port_;

  std::queue<std::unique_ptr<VerifyService::Stub>> connections_;
  std::condition_variable cond_;
  std::mutex mutex_;
};

class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
 public:
  friend class Singleton<VerifyGrpcClient>;

  GetVerifyResponse GetVerifyCode(const std::string& email);

 private:
  VerifyGrpcClient();

  std::unique_ptr<RpcConnectionPool> pool_;
};