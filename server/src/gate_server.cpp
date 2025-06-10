#include <hiredis/hiredis.h>

#include "config_manager.h"
#include "cserver.h"
#include "redis_manager.h"

void TestRedis() {
  RedisManager::GetInstance()->Set("test", "23123");
  std::string value;
  RedisManager::GetInstance()->Get("test", value);
  std::cout << value << std::endl;
}

int main() {
  auto& config_manager = ConfigManager::GetInstance();
  unsigned short gate_port = static_cast<unsigned short>(std::stoi(config_manager["GateServer"]["port"]));
  try {
    net::io_context ioc{1};
    net::signal_set signals{ioc, SIGINT, SIGTERM};
    signals.async_wait([&ioc](const boost::system::error_code& ec, [[maybe_unused]] int signal_number) {
      if (ec) {
        return;
      }
      ioc.stop();
    });
    std::make_shared<CServer>(ioc, gate_port)->Start();
    std::cout << "Gate Server started on port " << gate_port << std::endl;
    ioc.run();
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
