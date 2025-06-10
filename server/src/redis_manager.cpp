#include "redis_manager.h"

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
      std::cerr << "Auth failed" << std::endl;
      freeReplyObject(reply);
      continue;
    }
    freeReplyObject(reply);
    connections_.push(RedisContextPtr(redis_context, RedisFree));
  }
}

RedisConnectionPool::RedisContextPtr RedisConnectionPool::Get() {
  std::unique_lock lock{mutex_};
  cond_.wait(lock, [this] { return !connections_.empty() || stopped_; });
  if (stopped_) {
    return nullptr;
  }
  auto connection = std::move(connections_.front());
  connections_.pop();
  return connection;
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
  connection_pool_ = std::make_unique<RedisConnectionPool>(5, host, port, password);
}

bool RedisManager::Set(std::string_view key, std::string_view value) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(
      redisCommand(connection.get(), "SET %b %b", key.data(), key.size(), value.data(), value.size()));
  if (!redis_reply_) {
    std::cerr << std::format("[ SET \"{}\" \"{}\" ] failed", key, value) << std::endl;
    return false;
  }
  if (redis_reply_->type == REDIS_REPLY_STATUS && redis_reply_->str &&
      (std::string_view{redis_reply_->str, redis_reply_->len} == "OK" ||
       std::string_view{redis_reply_->str, redis_reply_->len} == "ok")) {
    std::cout << std::format("Successfully execute command [ SET \"{}\" \"{}\" ]", key, value) << std::endl;
    freeReplyObject(redis_reply_);
    return true;
  }
  std::cerr << std::format("[ SET \"{}\" \"{}\" ] failed", key, value) << std::endl;
  freeReplyObject(redis_reply_);
  return false;
}

bool RedisManager::Get(std::string_view key, std::string& value) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(redisCommand(connection.get(), "GET %b", key.data(), key.size()));
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_STRING) {
    std::cerr << std::format("[ GET \"{}\" ] failed", key) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  std::cout << std::format("Successfully execute command [ GET \"{}\" ]", key) << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}

bool RedisManager::LPush(std::string_view key, std::string_view value) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(
      redisCommand(connection.get(), "LPUSH %b %b", key.data(), key.size(), value.data(), value.size()));
  if (!redis_reply_) {
    std::cerr << std::format("[ LPUSH \"{}\" \"{}\" ] failed", key, value) << std::endl;
    return false;
  }
  if (redis_reply_->type != REDIS_REPLY_INTEGER || redis_reply_->integer <= 0) {
    std::cerr << std::format("[ LPUSH \"{}\" \"{}\" ] failed", key, value) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  std::cout << std::format("Successfully execute command [ LPUSH \"{}\" \"{}\" ]", key, value) << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}

bool RedisManager::RPush(std::string_view key, std::string_view value) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(
      redisCommand(connection.get(), "RPUSH %b %b", key.data(), key.size(), value.data(), value.size()));
  if (!redis_reply_) {
    std::cerr << std::format("[ RPush \"{}\" \"{}\" ] failed", key, value) << std::endl;
    return false;
  }
  if (redis_reply_->type != REDIS_REPLY_INTEGER || redis_reply_->integer <= 0) {
    std::cerr << std::format("[ RPush \"{}\" \"{}\" ] failed", key, value) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  std::cout << std::format("Successfully execute command [ RPush \"{}\" \"{}\" ]", key, value) << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}

bool RedisManager::LPop(std::string_view key, std::string& value) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(redisCommand(connection.get(), "LPOP %b", key.data(), key.size()));
  if (!redis_reply_ || redis_reply_->type == REDIS_REPLY_NIL) {
    std::cerr << std::format("[ LPop \"{}\" ] failed", key) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  std::cout << std::format("Successfully execute command [ LPop \"{}\" ]", key) << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}

bool RedisManager::RPop(std::string_view key, std::string& value) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(redisCommand(connection.get(), "RPOP %b", key.data(), key.size()));
  if (!redis_reply_ || redis_reply_->type == REDIS_REPLY_NIL) {
    std::cerr << std::format("[ RPop \"{}\" ] failed", key) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  std::cout << std::format("Successfully execute command [ RPop \"{}\" ]", key) << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}

bool RedisManager::HSet(std::string_view key, std::string_view field, std::string_view value) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(redisCommand(connection.get(), "HSET %b %b %b", key.data(), key.size(),
                                                            field.data(), field.size(), value.data(), value.size()));
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_INTEGER) {
    std::cerr << std::format("[ HSet \"{}\" \"{}\" \"{}\" ] failed", key, field, value) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  std::cout << std::format("Successfully execute command [ HSet \"{}\" \"{}\" \"{}\" ]", key, field, value)
            << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}

bool RedisManager::HGet(std::string_view key, std::string_view field, std::string& value) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(
      redisCommand(connection.get(), "HGET %b %b", key.data(), key.size(), field.data(), field.size()));
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_STRING) {
    std::cerr << std::format("[ HGet \"{}\" \"{}\" ] failed", key, field) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  std::cout << std::format("Successfully execute command [ HGet \"{}\" \"{}\" ]", key, field) << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}

bool RedisManager::Delete(std::string_view key) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(redisCommand(connection.get(), "DEL %b", key.data(), key.size()));
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_INTEGER) {
    std::cerr << std::format("[ Delete \"{}\" ] failed", key) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  std::cout << std::format("Successfully execute command [ Delete \"{}\" ]", key) << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}

bool RedisManager::ExistsKey(std::string_view key) {
  auto connection = connection_pool_->Get();
  if (!connection) {
    return false;
  }
  auto redis_reply_ = static_cast<redisReply*>(redisCommand(connection.get(), "EXISTS %b", key.data(), key.size()));
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_INTEGER || redis_reply_->integer == 0) {
    std::cerr << std::format("Not found [ Key \"{}\" ]", key) << std::endl;
    freeReplyObject(redis_reply_);
    return false;
  }
  std::cout << std::format("Found [ Key \"{}\" ]", key) << std::endl;
  freeReplyObject(redis_reply_);
  return true;
}