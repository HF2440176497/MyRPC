#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
*/
class UserService : public fixbug::UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name:" << name << " pwd:" << pwd << std::endl;  
        return false;
    }

    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id:" << id << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    // 重写基类UserServiceRpc的虚函数 下面这些方法都是框架直接调用的
    // 1. caller   ===>   Login(LoginRequest)  => muduo =>   callee 
    // 2. callee   ===>    Login(LoginRequest)  => 交到下面重写的这个Login方法上了
    
    // 个人理解：框架需要找到这个重载的 Login
    // 框架要想找到 Login 需要接收消息，因此框架是作为一个服务器框架。如何找到 Login 留作后续

    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        // 框架给业务上报了请求参数 LoginRequest，应用获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务 Login 可以是定义为任意名称的本地方法
        bool login_result = Login(name, pwd); 

        // 把响应打包  包括错误码、错误消息、返回值
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_sucess(login_result);

        // 执行回调操作   执行响应对象数据的序列化和网络发送（都是由框架来完成的）
        done->Run();
    }
};


// main 函数实际上是一个使用框架的示例
// 框架包含：服务器的初始化，服务器处理高并发连接
int main(int argc, char **argv) {
    // 调用框架的初始化操作 在这里我们并没有创建类对象的需要
    MprpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象（个人理解是一个服务器）。把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点   Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}