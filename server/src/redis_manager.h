#pragma once

#include <hiredis/hiredis.h>

#include <queue>

#include "singleton.h"

class RedisConnectionPool {
 private:
  static constexpr auto RedisFree = [](redisContext* c) { redisFree(c); };

 public:
  using RedisContextPtr = std::unique_ptr<redisContext, decltype(RedisFree)>;
  RedisConnectionPool(size_t size, std::string_view host, int port, std::string_view password);
  ~RedisConnectionPool();

  RedisContextPtr Get();

  void Push(RedisContextPtr connection);

  void Close();

  size_t size() const { return size_; }

 private:
  size_t size_;
  std::string host_;
  int port_;
  std::string password_;
  std::mutex mutex_;
  std::condition_variable cond_;
  std::atomic<bool> stopped_;
  std::queue<std::unique_ptr<redisContext, decltype(RedisFree)>> connections_;
};

class RedisManager : public Singleton<RedisManager> {
 public:
  friend class Singleton<RedisManager>;

  RedisManager(const RedisManager&) = delete;
  RedisManager& operator=(const RedisManager&) = delete;
  RedisManager(RedisManager&&) = delete;
  RedisManager& operator=(RedisManager&&) = delete;
  ~RedisManager() = default;

  bool Set(std::string_view key, std::string_view value);
  bool Get(std::string_view key, std::string& value);
  bool LPush(std::string_view key, std::string_view value);
  bool RPush(std::string_view key, std::string_view value);
  bool LPop(std::string_view key, std::string& value);
  bool RPop(std::string_view key, std::string& value);
  bool HSet(std::string_view key, std::string_view field, std::string_view value);
  bool HGet(std::string_view key, std::string_view field, std::string& value);
  bool Delete(std::string_view key);
  bool ExistsKey(std::string_view key);

 private:
  RedisManager();

  std::unique_ptr<RedisConnectionPool> connection_pool_;
};