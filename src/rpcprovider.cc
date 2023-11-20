
#include "rpcprovider.h"

#include <functional>
#include <iostream>
#include <string>

#include "mprpcapplication.h"  // 提供框架的初始化
#include "rpcheader.pb.h"
#include "logger.h"

MprpcApplication& g_mprpcapp = MprpcApplication::GetInstance();

// service_name -> 获得服务的描述 -> 找到对应的方法对象

/**
 * @brief 因为双方通信时，proto 序列化为 descriptor 格式，框架注册时也要按照此格式
 * @param service
 */
void RpcProvider::NotifyService(google::protobuf::Service* service) {
    ServiceInfo service_info;
    auto        pserviceDesc = service->GetDescriptor();

    // ServiceDescriptor 提供的方法
    const std::string& service_name = pserviceDesc->name();
    int                methodCnt    = pserviceDesc->method_count();
    LOG_INFO("srevice_name: [%s] with [%d] methods", service_name.c_str(), methodCnt);

    for (int i = 0; i < methodCnt; i++) {
        // 获取服务对象包含的各种方法的描述
        auto pmethodDesc = pserviceDesc->method(i);  // const MethodDescriptor* method(int index) const
        auto method_name = pmethodDesc->name();      // string 类型
        LOG_INFO("The [%d] nd method: [%s] ", i + 1, method_name.c_str());
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    // 对于单个服务对象 service，当然要插入 m_serviceMap 作为 value
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {  // 连接断开了
        conn->shutdown();
    }
}

// 远端若有 RPC 请求，会进行此回调函数，格式由 muduo setMessageCallback 所规定
// caller callee 双方已约定服务请求的参数格式 例如 LoginRequest
// 但是接收的字节流总共包含：header_size service method args
//
// 为了解决粘包问题，字节流需要包含 消息长度 header_size 参数长度 args_size
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp) {
    // 先使用 muduo 读取
    std::string recv = buffer->retrieveAllAsString();  // 转为 recv 包含了 method 和 参数

    uint32_t header_size = 0;
    recv.copy((char*)&header_size, 4, 0);  // string 中的按字节读取一个 int 长度

    // header_str 即包头 args 即包体
    std::string      rpc_header_str = recv.substr(4, header_size);
    mprpc::RpcHeader rpc_header;

    // 解析包头数据
    std::string service_name;
    std::string method_name;
    uint32_t    args_size;

    if (rpc_header.ParseFromString(rpc_header_str)) {  // 反序列化成功（从网络字节流解析）
        // 对于已定义的 message 可直接进行读取
        service_name = rpc_header.service_name();
        method_name  = rpc_header.method_name();
        args_size    = rpc_header.args_size();
    } else {
        std::cout << "rpc_header parse error" << std::endl;
        return;
    }
    // 解析包体数据，即 args
    std::string args_str = recv.substr(4 + header_size, args_size);

    // 打印信息
    std::cout << " ============================== " << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << " ============================== " << std::endl;

    auto it = m_serviceMap.find(service_name);  // 查找服务对象
    if (it == m_serviceMap.end()) {
        std::cout << service_name << " is not found " << std::endl;
        return;
    }
    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end()) {
        std::cout << service_name << ": " << method_name << " is not found " << std::endl;
        return;
    }
    // service 拿到的是业务中注册的服务 例如 UserService
    google::protobuf::Service*                service = it->second.m_service;
    const google::protobuf::MethodDescriptor* method  = mit->second;

    // GetRequestPrototype 拿到的是业务层 proto method 约定的参数类型，生成的对象待填充
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();

    // ParseFromString 是对于二进制字节流进行反序列化，之后才可用 才能通过 CallMethod 进行上报
    if (!request->ParseFromString(args_str)) {
        std::cout << "request parse error" << std::endl;
        return;
    }
    // 定位到业务 RPC 服务的 virtual Login，是依靠 CallMethod
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // google::protobuf::Closure *done: CallMethod 的回调: 也就是发送 method 返回结果

    // 指定函数模板实参类型，不采用自动类型推到
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
    (this, &RpcProvider::SendRpcResponse, conn, response);
    service->CallMethod(method, nullptr, request, response, done);
}

/**
 * @brief 返回调用结果，应当作为 CallMethod 的回调
 * @param conn 
 * @param response 传出参数时
 */
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    // 进行序列化再进行传输
    if (response->SerializeToString(&response_str)) {
        conn->send(response_str);
    } else {
        std::cout << "serialize response_str error" << std::endl;
    }
    conn->shutdown();  // 作为服务端关闭连接，认为此次服务完成
}

/**
 * @brief 借助 muduo 完成网络收发逻辑
 *
 * Run() 是 RPC 服务器启动
 * 内部封装了服务器程序启动的过程（指定了 ConnectionCallback 和 MessageCallback）
 *
 * 服务器接收的请求是 RPC 调用请求，因此我们编写的 RPC 服务是包含了网络通信的部分
 * OnMessage 的任务就是根据消息找到所请求的的服务
 */
void RpcProvider::Run() {
    std::string             ip   = MprpcApplication::GetInstance().GetConfig().GetItem("rpcserverip");
    uint16_t                port = atoi(MprpcApplication::GetInstance().GetConfig().GetItem("rpcserverport").c_str());
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