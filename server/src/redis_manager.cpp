#include "redis_manager.h"

#include <spdlog/spdlog.h>

#include "config_manager.h"

RedisConnectionPool::RedisConnectionPool(size_t size, std::string_view host, int port, std::string_view password)
    : size_{size}, host_{host}, port_{port}, password_{password} {
  for (size_t i = 0; i < size; ++i) {
    auto redis_context = redisConnect(host.data(), port);
    if (!redis_context || redis_context->err) {
      redisFree(redis_context);
      continue;
    }
    auto reply = static_cast<redisReply*>(redisCommand(redis_context, "AUTH %b", password.data(), password.size()));
    if (!reply || reply->type != REDIS_REPLY_STATUS ||
        (std::string_view{reply->str, reply->len} != "OK" && std::string_view{reply->str, reply->len} != "ok")) {
      spdlog::error("Auth failed");
      freeReplyObject(reply);
      continue;
    }
    freeReplyObject(reply);
    connections_.push(RedisContextPtr(redis_context, RedisFree));
  }
}

std::optional<RedisConnectionPool::Handler> RedisConnectionPool::Get() {
  std::unique_lock lock{mutex_};
  cond_.wait(lock, [this] { return !connections_.empty() || stopped_; });
  if (stopped_) {
    return std::nullopt;
  }
  auto connection = std::move(connections_.front());
  connections_.pop();
  return std::make_optional<Handler>(*this, std::move(connection));
}

void RedisConnectionPool::Push(RedisContextPtr connection) {
  if (!connection || connection->err) {
    return;
  }
  std::lock_guard lock{mutex_};
  connections_.push(std::move(connection));
  cond_.notify_one();
}

void RedisConnectionPool::Close() {
  stopped_ = true;
  cond_.notify_all();
}

RedisConnectionPool::~RedisConnectionPool() { Close(); }

RedisManager::RedisManager() {
  auto& config_manager = ConfigManager::GetInstance();
  const auto& host = config_manager["Redis"]["host"];
  unsigned short port = static_cast<unsigned short>(std::stoi(config_manager["Redis"]["port"]));
  const auto& password = config_manager["Redis"]["passwd"];
  spdlog::info("Redis host: {}, port: {}, password: {}", host, port, password);
  connection_pool_ = std::make_unique<RedisConnectionPool>(5, host, port, password);
}

bool RedisManager::Set(std::string_view key, std::string_view value) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ = RedisReplyPtr{static_cast<redisReply*>(
      redisCommand(handler->Get(), "SET %b %b", key.data(), key.size(), value.data(), value.size()))};
  if (!redis_reply_) {
    spdlog::error("[ SET \"{}\" \"{}\" ] failed", key, value);
    return false;
  }
  if (redis_reply_->type == REDIS_REPLY_STATUS && redis_reply_->str &&
      (std::string_view{redis_reply_->str, redis_reply_->len} == "OK" ||
       std::string_view{redis_reply_->str, redis_reply_->len} == "ok")) {
    spdlog::info("Successfully execute command [ SET \"{}\" \"{}\" ]", key, value);
    return true;
  }
  spdlog::error("[ SET \"{}\" \"{}\" ] failed", key, value);
  return false;
}

bool RedisManager::Get(std::string_view key, std::string& value) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ =
      RedisReplyPtr{static_cast<redisReply*>(redisCommand(handler->Get(), "GET %b", key.data(), key.size()))};
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_STRING) {
    spdlog::error("[ GET \"{}\" ] failed", key);
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  spdlog::info("Successfully execute command [ GET \"{}\" ]", key);
  return true;
}

bool RedisManager::LPush(std::string_view key, std::string_view value) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ = RedisReplyPtr{static_cast<redisReply*>(
      redisCommand(handler->Get(), "LPUSH %b %b", key.data(), key.size(), value.data(), value.size()))};
  if (!redis_reply_) {
    spdlog::error("[ LPUSH \"{}\" \"{}\" ] failed", key, value);
    return false;
  }
  if (redis_reply_->type != REDIS_REPLY_INTEGER || redis_reply_->integer <= 0) {
    spdlog::error("[ LPUSH \"{}\" \"{}\" ] failed", key, value);
    return false;
  }
  spdlog::info("Successfully execute command [ LPUSH \"{}\" \"{}\" ]", key, value);
  return true;
}

bool RedisManager::RPush(std::string_view key, std::string_view value) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ = RedisReplyPtr{static_cast<redisReply*>(
      redisCommand(handler->Get(), "RPUSH %b %b", key.data(), key.size(), value.data(), value.size()))};
  if (!redis_reply_) {
    spdlog::error("[ RPush \"{}\" \"{}\" ] failed", key, value);
    return false;
  }
  if (redis_reply_->type != REDIS_REPLY_INTEGER || redis_reply_->integer <= 0) {
    spdlog::error("[ RPush \"{}\" \"{}\" ] failed", key, value);
    return false;
  }
  spdlog::info("Successfully execute command [ RPush \"{}\" \"{}\" ]", key, value);
  return true;
}

bool RedisManager::LPop(std::string_view key, std::string& value) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ =
      RedisReplyPtr{static_cast<redisReply*>(redisCommand(handler->Get(), "LPOP %b", key.data(), key.size()))};
  if (!redis_reply_ || redis_reply_->type == REDIS_REPLY_NIL) {
    spdlog::error("[ LPop \"{}\" ] failed", key);
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  spdlog::info("Successfully execute command [ LPop \"{}\" ]", key);
  return true;
}

bool RedisManager::RPop(std::string_view key, std::string& value) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ =
      RedisReplyPtr{static_cast<redisReply*>(redisCommand(handler->Get(), "RPOP %b", key.data(), key.size()))};
  if (!redis_reply_ || redis_reply_->type == REDIS_REPLY_NIL) {
    spdlog::error("[ RPop \"{}\" ] failed", key);
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  spdlog::info("Successfully execute command [ RPop \"{}\" ]", key);
  return true;
}

bool RedisManager::HSet(std::string_view key, std::string_view field, std::string_view value) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ =
      RedisReplyPtr{static_cast<redisReply*>(redisCommand(handler->Get(), "HSET %b %b %b", key.data(), key.size(),
                                                          field.data(), field.size(), value.data(), value.size()))};
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_INTEGER) {
    spdlog::error("[ HSet \"{}\" \"{}\" \"{}\" ] failed", key, field, value);
    return false;
  }
  spdlog::info("Successfully execute command [ HSet \"{}\" \"{}\" \"{}\" ]", key, field, value);
  return true;
}

bool RedisManager::HGet(std::string_view key, std::string_view field, std::string& value) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ = RedisReplyPtr{static_cast<redisReply*>(
      redisCommand(handler->Get(), "HGET %b %b", key.data(), key.size(), field.data(), field.size()))};
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_STRING) {
    spdlog::error("[ HGet \"{}\" \"{}\" ] failed", key, field);
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  spdlog::info("Successfully execute command [ HGet \"{}\" \"{}\" ]", key, field);
  return true;
}

bool RedisManager::Delete(std::string_view key) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ =
      RedisReplyPtr{static_cast<redisReply*>(redisCommand(handler->Get(), "DEL %b", key.data(), key.size()))};
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_INTEGER) {
    spdlog::error("[ Delete \"{}\" ] failed", key);
    return false;
  }
  spdlog::info("Successfully execute command [ Delete \"{}\" ]", key);
  return true;
}

bool RedisManager::ExistsKey(std::string_view key) {
  auto handler = connection_pool_->Get();
  if (!handler) {
    return false;
  }
  auto redis_reply_ =
      RedisReplyPtr{static_cast<redisReply*>(redisCommand(handler->Get(), "EXISTS %b", key.data(), key.size()))};
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_INTEGER || redis_reply_->integer == 0) {
    spdlog::error("Not found [ Key \"{}\" ]", key);
    return false;
  }
  spdlog::info("Found [ Key \"{}\" ]", key);
  return true;
}