#include "config_manager.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

ConfigManager::ConfigManager() {
  auto config_file_path = std::filesystem::current_path() / "config.ini";
  std::cout << "config path: " << config_file_path << std::endl;

  boost::property_tree::ptree pt;

  boost::property_tree::read_ini(config_file_path.string(), pt);
  for (const auto& [section, section_info] : pt) {
    for (const auto& [key, value] : section_info) {
      sections_[section].Add(key, value.get_value<std::string>());
    }
  }

  // Print all sections
  for (const auto& [section, section_info] : sections_) {
    std::cout << "section: " << section << std::endl;
    for (const auto& [key, value] : section_info.section_datas_) {
      std::cout << "  key: " << key << ", value: " << value << std::endl;
    }
  }
}