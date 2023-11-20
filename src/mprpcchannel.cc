
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */

#include <string>

#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "mprpcchannel.h"

/**
 * @brief 
 * @param method 
 * @param controller caller 传递，在内修改，业务代码在外读取
 * @param request caller 直接按照业务需求进行传递的
 * @param response 
 * @param done 
 * @details stub.method 业务代码调用
 */
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                              google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                              google::protobuf::Message* response, google::protobuf::Closure* done) {
    const google::protobuf::ServiceDescriptor* sd = method->service();

    std::string service_name = sd->name();
    std::string method_name  = method->name();

    std::string args_str;
    int         args_size = 0;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        controller->SetFailed("request serialize to string error");
        return;
    }
    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    std::string header_str;
    uint32_t    header_size = 0;
    if (rpc_header.SerializeToString(&header_str)) {
        header_size = header_str.size();  // 直接使用序列化好的字节流
    } else {
        controller->SetFailed("rpc_header serialize to string error");
        return;
    }
    std::string send_rpc_str;

    // std::string((char*)&header_size, 4) 表示利用 substring 进行构造
    send_rpc_str.insert(0, std::string((char*)&header_size, 4));
    send_rpc_str.append(header_str);  // header_str 是 rpc_header 经过序列化
    send_rpc_str.append(args_str);    // args_str 是 request 经过序列化
    // 两种序列化格式进行拼接、发送

    std::cout << " ============================== " << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << " ============================== " << std::endl;

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        controller->SetFailed("create socket error");
        return;
    }

    // mprpc 生成得到库文件，业务逻辑代码链接库文件，最终得到可执行文件
    // 单例对象往往作为全局变量，需要能够独立生成
    std::string ip   = MprpcApplication::GetInstance().GetConfig().GetItem("rpcserverip");
    uint16_t    port = atoi(MprpcApplication::GetInstance().GetConfig().GetItem("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        close(clientfd);
        controller->SetFailed("connect error");
        return;
    }
    // 发送rpc请求
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0)) {
        close(clientfd);
        controller->SetFailed("send error"); 
        return;
    }
    // recv_buf 接收到服务端发送的序列化调用结果
    char recv_buf[1024] = {0};
    int  recv_size      = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))) {
        close(clientfd);
        controller->SetFailed("recv error");
        return;
    }

    // 反序列化rpc调用的响应数据
    // std::string response_str(recv_buf, 0, recv_size); // bug出现问题，recv_buf中遇到\0后面的数据就存不下来了，导致反序列化失败
    // if (!response->ParseFromString(response_str))

    // response 是传出参数，框架反序列化，业务逻辑调用后可读取通过 response 读取结果
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        close(clientfd);
        controller->SetFailed("mprpcchannel: response ParseFromArray error");
        return;
    }
    close(clientfd);
    return;
}
