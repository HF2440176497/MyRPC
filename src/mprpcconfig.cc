
#include "mprpcconfig.h"
#include <iostream>

constexpr int LINESIZE = 200;  // 配置项行的最大的长度


static void Lrim(std::string &str) {
    int idx = str.find_first_not_of(' ');
    if (idx != -1) {
        str = str.substr(idx, str.size()-idx);  
    }  
    return;
}

static void Rrim(std::string &str) {
    // 截去尾部的换行符
    int idx1 = str.find_last_not_of('\n');
    int idx2 = str.find_last_not_of('\r');

    if (idx1 != -1 || idx2 != -1) {  // 存在换行符，取 idx1 idx2 两者较小者
        int temp = (idx1 < idx2)? idx1: idx2;
        str = str.substr(0, temp+1);
    }
    // 截去尾部的空格
    int idx = str.find_last_not_of(' ');
    if (idx != -1) {
        str = str.substr(0, idx+1);  
    }
    return;
}

void MprpcConfig::LoadConfigFile(const char* conf_file) {
    FILE* fp = fopen(conf_file, "r");
    if (fp == nullptr) {
        std::cout << "Can not open: " << conf_file << std::endl;
    }
    while (!feof(fp)) {
        char line[LINESIZE] = {0};
        if (fgets(line, LINESIZE-1, fp) == nullptr) 
            continue;
        std::string strline(line);

        // 去掉空格
        int idx = strline.find_first_not_of(' ');
        if (idx != -1) {
            strline = strline.substr(idx, strline.size()-idx);  // 包括了 idx 处的字符
        } else {  // 说明此行是空行
            continue;
        }
        Rrim(strline);

        // 去掉注释
        if (strline[0] == '#' || strline.empty()) {
            continue;
        }
        // 解析配置项
        idx = strline.find('=');
        if (idx == -1) {
            continue;
        }
        std::string key = strline.substr(0, idx);
        std::string value = strline.substr(idx+1, strline.size()-idx);

        // 去掉键值对可能的空格
        Lrim(key);
        Rrim(value);
        m_conf_map.insert({key, value});
    }
    return;
}

std::string MprpcConfig::GetItem(const std::string &key) {
    auto it = m_conf_map.find(key);
    if (it == m_conf_map.end()) {
        return "";
    }
    return it->second;
}
