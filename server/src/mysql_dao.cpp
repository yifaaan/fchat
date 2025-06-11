#include "mysql_dao.h"

#include <spdlog/spdlog.h>

#include "config_manager.h"

MysqlDao::MysqlDao() {
  const auto& config = ConfigManager::GetInstance();
  const auto& host = config["Mysql"]["host"];
  const auto& port = config["Mysql"]["port"];
  const auto& passwd = config["Mysql"]["passwd"];
  const auto& db = config["Mysql"]["db"];
  const auto& user = config["Mysql"]["user"];
  pool_.reset(new MysqlPool(5, host, user, passwd, db, port));
}

int MysqlDao::RegisterUser(const std::string& name, const std::string& email, const std::string& passwd) {
  auto connection = pool_->Get();
  try {
    if (!connection) {
      return -1;
    }
    auto& session = *connection->session_;
    //
    session.sql("CALL reg_user(?,?,?,@result)").bind(name, email, passwd).execute();
    auto row = session.sql("SELECT @result AS result").execute().fetchOne();
    int result = row[0].get<int>();
    if (result == -1) {
      spdlog::error("Register user failed: {}", name);
      pool_->Push(std::move(connection));
      return -1;
    }
    if (result == 0) {
      spdlog::error("User already exists: {}", name);
      pool_->Push(std::move(connection));
      return 0;
    }
    spdlog::info("Register user result: {}", result);
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    connection->last_operation_time_ = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    pool_->Push(std::move(connection));
    return result;
  } catch (const mysqlx::Error& e) {
    spdlog::error("Register user failed: {}", e.what());
    pool_->Push(std::move(connection));
    return -1;
  }
}
