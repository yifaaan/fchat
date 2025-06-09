#include "config_manager.h"
#include "cserver.h"

int main() {
  ConfigManager config_manager;
  unsigned short gate_port = std::stoi(config_manager["GateServer"]["port"]);
  try {
    net::io_context ioc{1};
    net::signal_set signals{ioc, SIGINT, SIGTERM};
    signals.async_wait([&ioc](const boost::system::error_code& ec, int signal_number) {
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
