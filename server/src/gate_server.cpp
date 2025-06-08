#include "cserver.h"

int main() {
  try {
    unsigned short port = 8080;
    net::io_context ioc{1};
    net::signal_set signals{ioc, SIGINT, SIGTERM};
    signals.async_wait([&ioc](const boost::system::error_code& ec, int signal_number) {
      if (ec) {
        return;
      }
      ioc.stop();
    });
    std::make_shared<CServer>(ioc, port)->Start();
    ioc.run();
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
