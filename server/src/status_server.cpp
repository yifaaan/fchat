

#include <spdlog/spdlog.h>
#include <boost/asio.hpp>

#include <memory>

#include "config_manager.h"
#include "status_server_impl.h"


void RunStatusServer() {
  const auto& config_manager = ConfigManager::GetInstance();
  const auto& host = config_manager["StatusServer"]["host"];
  const auto& port = config_manager["StatusServer"]["port"];
  std::string server_address = std::format("{}:{}", host, port);
  StatusServerImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  // start server
  auto server = builder.BuildAndStart();
  spdlog::info("Status server started on {}", server_address);

  boost::asio::io_context ctx;
  boost::asio::signal_set signals(ctx, SIGINT, SIGTERM);
  signals.async_wait([&](const boost::system::error_code& error, int signal_number) {
    if (!error) {
      spdlog::info("Status server stopped by signal {}", signal_number);
      server->Shutdown();
    }
  });

  std::thread{[&ctx] { ctx.run();}}.detach();

  server->Wait();
  ctx.stop();
}

int main() {
  try {
    RunStatusServer();
  } catch (const std::exception& e) {
    spdlog::error("Status server error: {}", e.what());
    return 1;
  }
}