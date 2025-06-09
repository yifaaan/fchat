#include <hiredis/hiredis.h>

#include "config_manager.h"
#include "cserver.h"

void TestRedis() {
  auto c = redisConnect("127.0.0.1", 6379);
  if (c->err) {
    std::cout << "Connect to redisServer failed: " << c->errstr << std::endl;
    redisFree(c);
  }
  std::cout << "Connect to redisServer success" << std::endl;
  std::string passwd{"123456"};
  auto reply = (redisReply*)redisCommand(c, "AUTH %s", passwd.c_str());
  if (reply->type == REDIS_REPLY_ERROR) {
    std::cout << "Auth failed: " << reply->str << std::endl;
    freeReplyObject(reply);
    redisFree(c);
    return;
  } else {
    std::cout << "Auth success" << std::endl;
  }
  freeReplyObject(reply);  // 释放AUTH命令的reply

  // 执行SET命令
  // auto cmd1 = "set key1 value1";
  reply = (redisReply*)redisCommand(c, "set key1 value1");
  if (!reply) {
    std::cout << "Execute cmd failed - no reply" << std::endl;
    redisFree(c);
    return;
  }

  std::cout << std::format("SET command - type: {}, response: {}", reply->type, reply->str ? reply->str : "null")
            << std::endl;

  if (reply->type == REDIS_REPLY_STATUS && reply->str &&
      (std::strcmp(reply->str, "OK") == 0 || std::strcmp(reply->str, "ok") == 0)) {
    std::cout << "SET command executed successfully" << std::endl;
  } else {
    std::cout << "SET command failed" << std::endl;
  }

  freeReplyObject(reply);  // 释放SET命令的reply

  // 测试GET命令
  reply = (redisReply*)redisCommand(c, "GET key1");
  if (reply) {
    if (reply->type == REDIS_REPLY_STRING) {
      std::cout << "GET key1 result: " << reply->str << std::endl;
    } else if (reply->type == REDIS_REPLY_NIL) {
      std::cout << "GET key1 result: key not found" << std::endl;
    } else {
      std::cout << "GET key1 failed with type: " << reply->type << std::endl;
    }
    freeReplyObject(reply);
  }

  redisFree(c);
  std::cout << "Redis test completed" << std::endl;
}

int main() {
  TestRedis();
  // auto& config_manager = ConfigManager::GetInstance();
  // unsigned short gate_port = static_cast<unsigned short>(std::stoi(config_manager["GateServer"]["port"]));
  // try {
  //   net::io_context ioc{1};
  //   net::signal_set signals{ioc, SIGINT, SIGTERM};
  //   signals.async_wait([&ioc](const boost::system::error_code& ec, [[maybe_unused]] int signal_number) {
  //     if (ec) {
  //       return;
  //     }
  //     ioc.stop();
  //   });
  //   std::make_shared<CServer>(ioc, gate_port)->Start();
  //   std::cout << "Gate Server started on port " << gate_port << std::endl;
  //   ioc.run();
  // } catch (const std::exception& e) {
  //   std::cerr << "Exception: " << e.what() << "\n";
  // }
}
