#pragma once

#include <unordered_map>
#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"

// 发布 RPC 服务的类，属于框架代码
// NotifyService 将 
class RpcProvider {
public:
    void NotifyService(google::protobuf::Service* service);  // 发布 RPC 服务的函数，父类指针接收派生类
    void Run();  // 启动 RPC 的网络服务

private:
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

private:
    muduo::net::EventLoop m_eventLoop;

    struct ServiceInfo {  // 某服务对应的方法
        google::protobuf::Service* m_service;  // 服务方法的抽象类型
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;  // 保存 method
    };
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;  // 服务名称与服务方法结构体，m_serviceMap 成员负责管理多个服务
}; 