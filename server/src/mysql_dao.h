#pragma once

#include <memory>
#include <string>

#include "mysql_pool.h"

class MysqlDao {
 public:
  MysqlDao();
  ~MysqlDao() = default;
  int RegisterUser(const std::string& name, const std::string& email, const std::string& passwd);
  bool CheckEmail(const std::string& name, const std::string& email);
  bool UpdatePasswd(const std::string& name, const std::string& passwd);
  bool CheckPasswd(const std::string& name, const std::string& passwd);

 private:
  std::unique_ptr<MysqlPool> pool_;
};