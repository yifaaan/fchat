#include "mysql_manager.h"

MysqlManager::MysqlManager() {}

int MysqlManager::RegisterUser(const std::string& name, const std::string& email, const std::string& passwd) {
  return dao_.RegisterUser(name, email, passwd);
}
