#pragma once

#include <semaphore.h>  // 信号量
#include <zookeeper/zookeeper.h>

#include <string>

//
class ZkClient {
public:
    ZkClient();
    ~ZkClient();
    void        Start();  // 客户端使用者连接 zkserver
    void        Create(const char* path, const char* data,
                       int datalen, int state = 0);  // 创建 znode 节点
    std::string GetData(const char* path);           // 获取 znode 节点的数据

private:
    zhandle_t* m_zhandle;  // 客户端句柄
};