#include "verify_client.h"

GetVerifyResponse VerifyGrpcClient::GetVerifyCode(const std::string& email) {
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