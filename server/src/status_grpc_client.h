#pragma once

#include "win32_lean_mean.h"
#include <grpcpp/grpcpp.h>
#include <queue>

#include "message.grpc.pb.h"
#include "singleton.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using message::GetChatServerRequest;
using message::GetChatServerResponse;
using message::StatusService;

class StatusConnectionPool {
 public:
  StatusConnectionPool(size_t size, std::string host, std::string port);
  ~StatusConnectionPool();
  
  void Close();

  std::unique_ptr<StatusService::Stub> Get();
  void Push(std::unique_ptr<StatusService::Stub> stub);
  size_t size() { return size_; }

 private:
  std::atomic<bool> stopped_;
  size_t size_;
  std::string host_;
  std::string port_;

  std::queue<std::unique_ptr<StatusService::Stub>> connections_;
  std::condition_variable cond_;
  std::mutex mutex_;
};

class StatusGrpcClient : public Singleton<StatusGrpcClient> {
 public:
  friend class Singleton<StatusGrpcClient>;

  GetChatServerResponse GetChatServer(int32_t uid);

 private:
  StatusGrpcClient();
  std::unique_ptr<StatusConnectionPool> pool_;
};