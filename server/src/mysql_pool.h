#pragma once

#include <mysqlx/xdevapi.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

class Defer {
 public:
  Defer(std::function<void()> func) : func_{std::move(func)} {}
  ~Defer() { func_(); }

 private:
  std::function<void()> func_;
};

struct SqlConnection {
  SqlConnection(std::unique_ptr<mysqlx::Session> session, uint64_t last_operation_time);

  std::unique_ptr<mysqlx::Session> session_;
  uint64_t last_operation_time_;
};

class MysqlPool {
 public:
  MysqlPool() = delete;
  MysqlPool(const MysqlPool& other) = delete;
  MysqlPool(MysqlPool&& other) = delete;
  MysqlPool& operator=(const MysqlPool& other) = delete;
  MysqlPool& operator=(MysqlPool&& other) = delete;
  MysqlPool(size_t size, const std::string& url, const std::string& user, const std::string& passwd,
            const std::string& db, const std::string& port);
  ~MysqlPool();

  std::unique_ptr<SqlConnection> Get();

  void Push(std::unique_ptr<SqlConnection> connection);

  void Close();

  void CheckConnections();

  size_t size() const { return size_; }

 private:
  // std::string url_;
  // std::string user_;
  // std::string passwd_;
  // std::string db_;
  // std::string port_;
  mysqlx::SessionSettings setting_;
  size_t size_;

  std::queue<std::unique_ptr<SqlConnection>> connections_;
  std::mutex mutex_;
  std::atomic<bool> stopped_{false};
  std::condition_variable cond_;
  std::thread check_thread_;
};
