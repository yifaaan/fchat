#pragma once

#include <grpcpp/grpcpp.h>

#include "const.h"
#include "message.grpc.pb.h"
#include "singleton.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using message::GetVerifyRequest;
using message::GetVerifyResponse;
using message::VerifyService;

class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
 public:
  friend class Singleton<VerifyGrpcClient>;

  GetVerifyResponse GetVerifyCode(const std::string& email);

 private:
  VerifyGrpcClient() {
    std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    stub_ = VerifyService::NewStub(channel);
  }

  std::unique_ptr<VerifyService::Stub> stub_;
};