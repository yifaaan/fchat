#include "redis_manager.h"

#include "config_manager.h"

RedisManager::RedisManager() {
  auto& config_manager = ConfigManager::GetInstance();
  const auto& host = config_manager["Redis"]["host"];
  unsigned short port = static_cast<unsigned short>(std::stoi(config_manager["Redis"]["port"]));
  Connect(host, port);
  Auth(config_manager["Redis"]["passwd"]);
}

bool RedisManager::Connect(std::string_view host, int port) {
  redis_context_.reset(redisConnect(host.data(), port));
  if (!redis_context_ || redis_context_->err) {
    std::cerr << "Connect to redisServer failed" << std::endl;
    return false;
  }
  return true;
}

bool RedisManager::Set(std::string_view key, std::string_view value) {
  redis_reply_.reset(static_cast<redisReply*>(
      redisCommand(redis_context_.get(), "SET %b %b", key.data(), key.size(), value.data(), value.size())));
  if (!redis_reply_) {
    std::cerr << std::format("[ SET \"{}\" \"{}\" ] failed", key, value) << std::endl;
    return false;
  }
  if (redis_reply_->type == REDIS_REPLY_STATUS && redis_reply_->str &&
      (std::string_view{redis_reply_->str, redis_reply_->len} == "OK" ||
       std::string_view{redis_reply_->str, redis_reply_->len} == "ok")) {
    std::cout << std::format("Successfully execute command [ SET \"{}\" \"{}\" ]", key, value) << std::endl;
    return true;
  }
  std::cerr << std::format("[ SET \"{}\" \"{}\" ] failed", key, value) << std::endl;
  return false;
}

bool RedisManager::Get(std::string_view key, std::string& value) {
  redis_reply_.reset(static_cast<redisReply*>(redisCommand(redis_context_.get(), "GET %b", key.data(), key.size())));
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_STRING) {
    std::cerr << std::format("[ GET \"{}\" ] failed", key) << std::endl;
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  std::cout << std::format("Successfully execute command [ GET \"{}\" ]", key) << std::endl;
  return true;
}

bool RedisManager::Auth(std::string_view password) {
  redis_reply_.reset(
      static_cast<redisReply*>(redisCommand(redis_context_.get(), "AUTH %b", password.data(), password.size())));
  if (!redis_reply_) {
    std::cerr << std::format("[ AUTH \"{}\" ] failed", password) << std::endl;
    return false;
  }
  if (redis_reply_->type == REDIS_REPLY_ERROR) {
    std::cerr << std::format("[ AUTH \"{}\" ] failed", password) << std::endl;
    return false;
  }
  std::cout << std::format("Successfully execute command [ AUTH \"{}\" ]", password) << std::endl;
  return true;
}
bool RedisManager::LPush(std::string_view key, std::string_view value) {
  redis_reply_.reset(static_cast<redisReply*>(
      redisCommand(redis_context_.get(), "LPUSH %b %b", key.data(), key.size(), value.data(), value.size())));
  if (!redis_reply_) {
    std::cerr << std::format("[ LPUSH \"{}\" \"{}\" ] failed", key, value) << std::endl;
    return false;
  }
  if (redis_reply_->type != REDIS_REPLY_INTEGER || redis_reply_->integer <= 0) {
    std::cerr << std::format("[ LPUSH \"{}\" \"{}\" ] failed", key, value) << std::endl;
    return false;
  }
  std::cout << std::format("Successfully execute command [ LPUSH \"{}\" \"{}\" ]", key, value) << std::endl;
  return true;
}

bool RedisManager::RPush(std::string_view key, std::string_view value) {
  redis_reply_.reset(static_cast<redisReply*>(
      redisCommand(redis_context_.get(), "RPUSH %b %b", key.data(), key.size(), value.data(), value.size())));
  if (!redis_reply_) {
    std::cerr << std::format("[ RPush \"{}\" \"{}\" ] failed", key, value) << std::endl;
    return false;
  }
  if (redis_reply_->type != REDIS_REPLY_INTEGER || redis_reply_->integer <= 0) {
    std::cerr << std::format("[ RPush \"{}\" \"{}\" ] failed", key, value) << std::endl;
    return false;
  }
  std::cout << std::format("Successfully execute command [ RPush \"{}\" \"{}\" ]", key, value) << std::endl;
  return true;
}

bool RedisManager::LPop(std::string_view key, std::string& value) {
  redis_reply_.reset(static_cast<redisReply*>(redisCommand(redis_context_.get(), "LPOP %b", key.data(), key.size())));
  if (!redis_reply_ || redis_reply_->type == REDIS_REPLY_NIL) {
    std::cerr << std::format("[ LPop \"{}\" ] failed", key) << std::endl;
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  std::cout << std::format("Successfully execute command [ LPop \"{}\" ]", key) << std::endl;
  return true;
}

bool RedisManager::RPop(std::string_view key, std::string& value) {
  redis_reply_.reset(static_cast<redisReply*>(redisCommand(redis_context_.get(), "RPOP %b", key.data(), key.size())));
  if (!redis_reply_ || redis_reply_->type == REDIS_REPLY_NIL) {
    std::cerr << std::format("[ RPop \"{}\" ] failed", key) << std::endl;
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  std::cout << std::format("Successfully execute command [ RPop \"{}\" ]", key) << std::endl;
  return true;
}

bool RedisManager::HSet(std::string_view key, std::string_view field, std::string_view value) {
  redis_reply_.reset(
      static_cast<redisReply*>(redisCommand(redis_context_.get(), "HSET %b %b %b", key.data(), key.size(), field.data(),
                                            field.size(), value.data(), value.size())));
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_INTEGER) {
    std::cerr << std::format("[ HSet \"{}\" \"{}\" \"{}\" ] failed", key, field, value) << std::endl;
    return false;
  }
  std::cout << std::format("Successfully execute command [ HSet \"{}\" \"{}\" \"{}\" ]", key, field, value)
            << std::endl;
  return true;
}

bool RedisManager::HGet(std::string_view key, std::string_view field, std::string& value) {
  redis_reply_.reset(static_cast<redisReply*>(
      redisCommand(redis_context_.get(), "HGET %b %b", key.data(), key.size(), field.data(), field.size())));
  if (!redis_reply_ || redis_reply_->type != REDIS_REPLY_STRING) {
    std::cerr << std::format("[ HGet \"{}\" \"{}\" ] failed", key, field) << std::endl;
    return false;
  }
  value.assign(redis_reply_->str, redis_reply_->len);
  std::cout << std::format("Successfully execute command [ HGet \"{}\" \"{}\" ]", key, field) << std::endl;
  return true;
}