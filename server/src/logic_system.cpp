#include "logic_system.h"

#include "http_connection.h"
#include "verify_server.h"

LogicSystem::LogicSystem() {
  RegisterGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
    beast::ostream(connection->response_.body()) << "received get_test\n";
    int i = 0;
    for (auto& [key, value] : connection->get_params_) {
      i++;
      beast::ostream(connection->response_.body()) << i << ". " << key << "=" << value << "\n";
    }
  });

  RegisterPost("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
    auto body_str = beast::buffers_to_string(connection->request_.body().data());
    std::cout << "received post body: " << body_str << std::endl;
    connection->response_.set(http::field::content_type, "text/json");

    nlohmann::json post_body;
    nlohmann::json response_body;
    try {
      post_body = nlohmann::json::parse(body_str);
    } catch (const nlohmann::json::parse_error& e) {
      std::cerr << "parse post json data error: " << e.what() << std::endl;
      response_body["error"] = ErrorCodes::kErrorJson;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }

    if (auto email = post_body["email"]; email.is_null()) {
      response_body["error"] = ErrorCodes::kErrorJson;
      beast::ostream(connection->response_.body()) << response_body.dump();
    } else {
      std::string email_str = email.get<std::string>();
      // Call verify grpc server
      auto verify_response = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
      std::cout << "post data: " << email_str << std::endl;
      response_body["error"] = verify_response.error();
      response_body["email"] = email_str;
      beast::ostream(connection->response_.body()) << response_body.dump();
    }
  });
}

void LogicSystem::RegisterGet(const std::string& url, HttpHandler handler) {
  get_handlers_.emplace(url, std::move(handler));
}

void LogicSystem::RegisterPost(const std::string& url, HttpHandler handler) {
  post_handlers_.emplace(url, std::move(handler));
}

bool LogicSystem::HandleGet(const std::string& url, std::shared_ptr<HttpConnection> connection) {
  if (auto it = get_handlers_.find(url); it == get_handlers_.end()) {
    return false;
  } else {
    std::cout << "handle get url: " << url << std::endl;
    it->second(connection);
    return true;
  }
}

bool LogicSystem::HandlePost(const std::string& url, std::shared_ptr<HttpConnection> connection) {
  if (auto it = post_handlers_.find(url); it == post_handlers_.end()) {
    return false;
  } else {
    std::cout << "handle post url: " << url << std::endl;
    it->second(connection);
    return true;
  }
}