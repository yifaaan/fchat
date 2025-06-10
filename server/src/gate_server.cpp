#include <hiredis/hiredis.h>

#include "config_manager.h"
#include "cserver.h"
#include "redis_manager.h"

void TestRedis() {
  auto redis_manager = RedisManager::GetInstance();
  redis_manager->HSet("f", "k", "v");
  redis_manager->HSet("f", "k1", "v1");
  redis_manager->HSet("f", "k1", "v2");
  std::string value;
  redis_manager->HGet("f", "k", value);
  std::cout << value << std::endl;
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
