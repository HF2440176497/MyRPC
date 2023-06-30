#pragma once

#include <string>
#include <unordered_map>

class MprpcConfig {
public:
    void LoadConfigFile(const char* conf_file);
    std::string GetItem(const std::string &key);
private:    
    std::unordered_map<std::string, std::string> m_conf_map;  // MyNignx 项目中使用的是自定义结构体

};