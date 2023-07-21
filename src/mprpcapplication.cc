
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "mprpcapplication.h"

MprpcConfig MprpcApplication::m_config = MprpcConfig();

MprpcApplication& MprpcApplication::GetInstance() {
    static MprpcApplication m_instance;
    return m_instance;
}

// 返回静态对象的引用
// 单例类中的静态成员变量，对于此成员变量来讲，也是只有一份的
MprpcConfig& MprpcApplication::GetConfig() {
    return m_config;
}

/**
 * @brief 打印日志
*/
static void ShowArgsHelp(const std::string& infostr) {
    std::cout << infostr << std::endl;
}

/**
 * @brief RPC 服务初始化,
*/
void MprpcApplication::Init(int argc, char** argv) {
    if (argc < 2) {  // 未指定配置文件
        ShowArgsHelp("arguments don't need requirements");
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string conf_file;

    while ((c = getopt(argc, argv, "i:")) != -1) {  // 必须提供选项 i 的参数
        switch (c)
        {
        case 'i':
            conf_file = optarg;
            break;
        case '?':  // 未知参数
            ShowArgsHelp("Unknown option");
            exit(EXIT_FAILURE);
        case ':':  // 存在选项，但是无选项参数
            ShowArgsHelp("No arguments");
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    m_config.LoadConfigFile(conf_file.c_str());

    std::cout << "rpcserverip: " << m_config.GetItem("rpcserverip") << std::endl;
    std::cout << "rpcserverport: " << m_config.GetItem("rpcserverport") << std::endl;
    std::cout << "zookeeperip: " << m_config.GetItem("zookeeperip") << std::endl;
    std::cout << "zookeeperport: " << m_config.GetItem("zookeeperport") << std::endl;

    return;
}
