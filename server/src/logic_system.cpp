#include "logic_system.h"

#include <spdlog/spdlog.h>

#include "http_connection.h"
#include "mysql_manager.h"
#include "redis_manager.h"
#include "verify_client.h"

namespace {

static constexpr std::string_view CodePrefix{"code_"};
}

LogicSystem::LogicSystem() {
  RegisterGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
    beast::ostream(connection->response_.body()) << "received get_test\n";
    int i = 0;
    for (auto& [key, value] : connection->get_params_) {
      i++;
      beast::ostream(connection->response_.body()) << i << ". " << key << "=" << value << "\n";
    }
  });

  // client get verify code callback
  RegisterPost("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
    auto body_str = beast::buffers_to_string(connection->request_.body().data());
    std::cout << "GateServer received client post body: " << body_str << std::endl;
    connection->response_.set(http::field::content_type, "text/json");

    nlohmann::json post_body;
    nlohmann::json response_body;
    try {
      post_body = nlohmann::json::parse(body_str);
    } catch (const nlohmann::json::parse_error& e) {
      spdlog::error("parse post json data error: {}", e.what());
      response_body["error"] = ErrorCodes::kErrorJson;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }

    auto CheckJsonParams = [&](std::string_view key) {
      if (auto value = post_body[key]; value.is_null()) {
        response_body["error"] = ErrorCodes::kErrorJson;
        beast::ostream(connection->response_.body()) << response_body.dump();
        return false;
      }
      return true;
    };

    if (!CheckJsonParams("email")) {
      return;
    }
    auto email = post_body["email"].get<std::string>();
    // Call verify grpc server
    auto verify_response = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
    std::cout << "post data: " << email << std::endl;
    response_body["error"] = verify_response.error();
    response_body["email"] = email;
    beast::ostream(connection->response_.body()) << response_body.dump();
  });

  // client register user callback
  RegisterPost("/register_user", [](std::shared_ptr<HttpConnection> connection) {
    auto body_str = beast::buffers_to_string(connection->request_.body().data());
    std::cout << "GateServer received client post body: " << body_str << std::endl;
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

    auto CheckJsonParams = [&](std::string_view key) {
      if (auto value = post_body[key]; value.is_null()) {
        spdlog::error("json params error: {}", key);
        response_body["error"] = ErrorCodes::kErrorJson;
        beast::ostream(connection->response_.body()) << response_body.dump();
        return false;
      }
      return true;
    };
    if (!CheckJsonParams("user") || !CheckJsonParams("email") || !CheckJsonParams("passwd") ||
        !CheckJsonParams("confirm") || !CheckJsonParams("verify_code")) {
      return;
    }

    auto user = post_body["user"].get<std::string>();
    auto passwd = post_body["passwd"].get<std::string>();
    auto confirm = post_body["confirm"].get<std::string>();
    auto email = post_body["email"].get<std::string>();

    // check passwd and confirm
    if (passwd != confirm) {
      spdlog::error("passwd and confirm not match");
      response_body["error"] = ErrorCodes::kErrorUserPasswdNotMatch;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }

    // check varify code
    std::string verify_code;
    auto ok = RedisManager::GetInstance()->Get(std::string{CodePrefix} + email, verify_code);
    if (!ok) {
      spdlog::info("get verify code expired");
      response_body["error"] = ErrorCodes::kErrorVarifyCodeExpired;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }
    if (verify_code != post_body["verify_code"].get<std::string>()) {
      spdlog::info("verify code not match");
      response_body["error"] = ErrorCodes::kErrorVarifyCodeNotMatch;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }

    // check user in mysql
    int uid = MysqlManager::GetInstance()->RegisterUser(user, email, passwd);
    if (uid == 0 || uid == -1) {
      spdlog::info("User or email already exists");
      response_body["error"] = ErrorCodes::kErrorUserAlreadyExists;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }

    response_body["error"] = ErrorCodes::kSuccess;
    response_body["email"] = email;
    response_body["user"] = user;
    response_body["passwd"] = passwd;
    response_body["confirm"] = confirm;
    response_body["verify_code"] = verify_code;
    beast::ostream(connection->response_.body()) << response_body.dump();
  });

  // reset pwd
  RegisterPost("/reset_pwd", [](std::shared_ptr<HttpConnection> connection) {
    auto body_str = beast::buffers_to_string(connection->request_.body().data());
    std::cout << "GateServer received client post body: " << body_str << std::endl;
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

    auto CheckJsonParams = [&](std::string_view key) {
      if (auto value = post_body[key]; value.is_null()) {
        spdlog::error("json params error: {}", key);
        response_body["error"] = ErrorCodes::kErrorJson;
        beast::ostream(connection->response_.body()) << response_body.dump();
        return false;
      }
      return true;
    };
    if (!CheckJsonParams("user") || !CheckJsonParams("email") || !CheckJsonParams("passwd") ||
        !CheckJsonParams("verify_code")) {
      return;
    }

    auto user = post_body["user"].get<std::string>();
    auto passwd = post_body["passwd"].get<std::string>();
    auto email = post_body["email"].get<std::string>();
    // check varify code
    std::string verify_code;
    auto ok = RedisManager::GetInstance()->Get(std::string{CodePrefix} + email, verify_code);
    if (!ok) {
      spdlog::info("get verify code expired");
      response_body["error"] = ErrorCodes::kErrorVarifyCodeExpired;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }
    if (verify_code != post_body["verify_code"].get<std::string>()) {
      spdlog::info("verify code not match");
      response_body["error"] = ErrorCodes::kErrorVarifyCodeNotMatch;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }

    // check user in mysql
    int uid = MysqlManager::GetInstance()->RegisterUser(user, email, passwd);
    if (uid == 0 || uid == -1) {
      spdlog::info("User or email already exists");
      response_body["error"] = ErrorCodes::kErrorUserAlreadyExists;
      beast::ostream(connection->response_.body()) << response_body.dump();
      return;
    }

    response_body["error"] = ErrorCodes::kSuccess;
    response_body["email"] = email;
    response_body["user"] = user;
    response_body["passwd"] = passwd;
    response_body["verify_code"] = verify_code;
    beast::ostream(connection->response_.body()) << response_body.dump();
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