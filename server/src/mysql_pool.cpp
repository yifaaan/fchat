#include "mysql_pool.h"

#include "spdlog/spdlog.h"

SqlConnection::SqlConnection(std::unique_ptr<mysqlx::Session> session, uint64_t last_operation_time)
    : session_{std::move(session)}, last_operation_time_{last_operation_time} {}

MysqlPool::MysqlPool(size_t size, const std::string& url, const std::string& user, const std::string& passwd,
                     const std::string& db, const std::string& port)
    : setting_{url, static_cast<unsigned short>(std::stoi(port)), user, passwd, db} {
  try {
    for (size_t i = 0; i < size; ++i) {
      auto duration = std::chrono::steady_clock::now().time_since_epoch();
      auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
      auto session = std::make_unique<mysqlx::Session>(setting_);
      connections_.push(std::make_unique<SqlConnection>(std::move(session), seconds));
    }

    check_thread_ = std::thread([this]() {
      while (!stopped_) {
        CheckConnections();
        std::this_thread::sleep_for(std::chrono::seconds(60));
      }
    });
    check_thread_.detach();
  } catch (const mysqlx::Error& e) {
    spdlog::error("Failed to create mysql pool: {}", e.what());
  }
}

std::unique_ptr<SqlConnection> MysqlPool::Get() {
  std::unique_lock lock{mutex_};
  cond_.wait(lock, [this] {
    if (stopped_ || !connections_.empty()) {
      return true;
    }
    return false;
  });
  if (stopped_) {
    return nullptr;
  }
  auto connection = std::move(connections_.front());
  connections_.pop();
  return connection;
}

void MysqlPool::Push(std::unique_ptr<SqlConnection> connection) {
  if (stopped_) {
    return;
  }
  std::lock_guard lock{mutex_};
  connections_.push(std::move(connection));
  cond_.notify_one();
}

void MysqlPool::Close() {
  stopped_ = true;
  cond_.notify_all();
}

MysqlPool::~MysqlPool() { Close(); }

void MysqlPool::CheckConnections() {
  std::lock_guard lock{mutex_};
  auto size = connections_.size();
  auto now = std::chrono::steady_clock::now().time_since_epoch();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now).count();
  for (size_t i = 0; i < size; ++i) {
    auto connection = std::move(connections_.front());
    connections_.pop();
    Defer defer([this, &connection]() { connections_.push(std::move(connection)); });
    if (seconds - connection->last_operation_time_ < 60) {
      continue;
    }
    // keep connection alive
    try {
      connection->session_->sql("SELECT 1").execute();
      connection->last_operation_time_ = seconds;
    } catch (const mysqlx::Error& e) {
      spdlog::error("MysqlPool: keep connection alive failed: {}", e.what());
      try {
        // create a new connection
        auto session = std::make_unique<mysqlx::Session>(setting_);
        connection.reset(new SqlConnection(std::move(session), seconds));
      } catch (const mysqlx::Error& e) {
        spdlog::error("MysqlPool: recreate a new connection failed: {}", e.what());
      }
    }
  }
}
