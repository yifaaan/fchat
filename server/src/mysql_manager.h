#pragma once

#include "mysql_dao.h"
#include "singleton.h"

class MysqlManager : public Singleton<MysqlManager> {
 public:
  int RegisterUser(const std::string& name, const std::string& email, const std::string& passwd);
  bool CheckEmail(const std::string& name, const std::string& email);
  bool UpdatePasswd(const std::string& name, const std::string& passwd);
  bool CheckPasswd(const std::string& name, const std::string& passwd, UserInfo& user_info);

 private:
  friend class Singleton<MysqlManager>;
  MysqlManager();

  MysqlDao dao_;
};