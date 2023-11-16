#pragma once

#include <string>
#include <unordered_map>

// MprpcApplication 中含有 static 成员变量 m_config
class MprpcConfig {
public:
    void LoadConfigFile(const char* conf_file);
    std::string GetItem(const std::string &key);
private:    
    std::unordered_map<std::string, std::string> m_conf_map;  // MyNignx 项目中使用的是自定义结构体

};