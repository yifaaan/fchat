#pragma once

#include "const.h"

struct SectionInfo {
  SectionInfo() = default;
  SectionInfo(const SectionInfo& other) = default;
  SectionInfo(SectionInfo&& other) = default;
  SectionInfo& operator=(const SectionInfo& other) = default;
  SectionInfo& operator=(SectionInfo&& other) = default;
  ~SectionInfo() = default;

  void Add(const std::string& key, const std::string& value) { section_datas_[key] = value; }

  const std::string& operator[](const std::string& key) const {
    if (auto it = section_datas_.find(key); it != section_datas_.end()) {
      return it->second;
    }
    return empty_value;
  }

  std::unordered_map<std::string, std::string> section_datas_;
  inline static std::string empty_value;
};

class ConfigManager {
 public:
  ConfigManager();
  ConfigManager(const ConfigManager& other) = default;
  ConfigManager(ConfigManager&& other) = default;
  ConfigManager& operator=(const ConfigManager& other) = default;
  ConfigManager& operator=(ConfigManager&& other) = default;
  ~ConfigManager() = default;

  const SectionInfo& operator[](const std::string& section) const {
    if (auto it = sections_.find(section); it != sections_.end()) {
      return it->second;
    }
    return empty_section_info_;
  }

 private:
  std::unordered_map<std::string, SectionInfo> sections_;
  inline static SectionInfo empty_section_info_;
};