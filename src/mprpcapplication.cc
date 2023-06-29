
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "mprpcapplication.h"

/**
 * @brief 打印日志
*/
static void ShowArgsHelp(const std::string& infostr) {
    std::cout << infostr << std::endl;
}

/**
 * @brief RPC 服务初始化
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
    


    return;
}