#pragma once

#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include "google/protobuf/service.h"

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
}; 