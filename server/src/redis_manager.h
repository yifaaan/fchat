#pragma once

#include <hiredis/hiredis.h>

#include "singleton.h"

class RedisManager : public Singleton<RedisManager> {
 public:
  friend class Singleton<RedisManager>;

  ~RedisManager() = default;
  bool Connect(std::string_view host, int port);
  bool Auth(std::string_view password);
  bool Set(std::string_view key, std::string_view value);
  bool Get(std::string_view key, std::string& value);
  bool LPush(std::string_view key, std::string_view value);
  bool RPush(std::string_view key, std::string_view value);
  bool LPop(std::string_view key, std::string& value);
  bool RPop(std::string_view key, std::string& value);
  bool HSet(std::string_view key, std::string_view field, std::string_view value);
  bool HGet(std::string_view key, std::string_view field, std::string& value);

 private:
  static constexpr auto RedisFree = [](redisContext* c) { redisFree(c); };
  static constexpr auto RedisReplyFree = [](redisReply* r) { freeReplyObject(r); };
  RedisManager();

  std::unique_ptr<redisContext, decltype(RedisFree)> redis_context_;
  std::unique_ptr<redisReply, decltype(RedisReplyFree)> redis_reply_;
};