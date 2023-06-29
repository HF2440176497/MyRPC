#pragma once

#include "google/protobuf/service.h"

// 发布 RPC 服务的
class RpcProvider {
public:
    void NotifyService(google::protobuf::Service* service);  // 发布 RPC 服务的函数，父类指针接收派生类
    void Run();  // 启动 RPC 的网络服务

}; 