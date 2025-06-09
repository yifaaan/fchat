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

  GetVerifyResponse GetVerifyCode(const std::string& email) {
    ClientContext ctx;
    GetVerifyResponse response;
    GetVerifyRequest request;
    request.set_email(email);

    // Send request to verify grpc server
    auto status = stub_->GetVerifyCode(&ctx, request, &response);
    if (!status.ok()) {
      response.set_error(static_cast<int32_t>(ErrorCodes::kRpcFailed));
      return response;
    }
    return response;
  }

 private:
  VerifyGrpcClient() {
    std::shared_ptr<Channel> channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
    stub_ = VerifyService::NewStub(channel);
  }

  std::unique_ptr<VerifyService::Stub> stub_;
};