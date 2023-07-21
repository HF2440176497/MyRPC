
#include <string>
#include <functional>
#include "rpcprovider.h"
#include "mprpcapplication.h"  // 提供框架的初始化
#include "google/protobuf/service.h"

MprpcApplication& g_mprpcapp = MprpcApplication::GetInstance();

// 框架实现代码

void RpcProvider::NotifyService(google::protobuf::Service* service) {


    

}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr&) {



}


void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp) {


}


/**
 * @brief 借助 muduo 完成网络收发逻辑
 * 
 */
void RpcProvider::Run() {

    std::string ip = MprpcApplication::GetInstance().GetConfig().GetItem("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().GetItem("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    // 绑定连接回调函数和消息收发回调函数 std::bind 返回可以赋值给 std::function
    // std::function<void (const TcpConnectionPtr&)> 需要传入按照此格式的可调用对象
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));

    // typedef std::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));  

    server.setThreadNum(4);
   
    std::cout << "RPC Provider start service at ip:" << ip << "; port: " << port << std::endl;
    server.start();
    m_eventLoop.loop();  // 阻塞等待连接

}