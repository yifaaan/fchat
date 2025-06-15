#include "mysql_manager.h"

MysqlManager::MysqlManager() {}

int MysqlManager::RegisterUser(const std::string& name, const std::string& email, const std::string& passwd) {
  return dao_.RegisterUser(name, email, passwd);
}

bool MysqlManager::CheckPasswd(const std::string& name, const std::string& passwd, UserInfo& user_info) {
  return dao_.CheckPasswd(name, passwd, user_info);
}