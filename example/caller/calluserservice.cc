
#include <iostream>
#include <string>
#include "user.pb.h"  // 业务 proto 协议
#include "mprpcapplication.h"
#include "mprpcchannel.h"

// caller 使用框架的示例
MprpcApplication& g_mprpcapp = MprpcApplication::GetInstance();

int main(int argc, char** argv) {
    MprpcApplication::Init(argc, argv);

    // UserServiceRpc_Stub 的构造需要 channel 参数
    // caller 不需要发布服务，只需要使用 proto 的 service 的 Stub class
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    
    fixbug::LoginRequest login_request;
    login_request.set_name("zhang san");
    login_request.set_pwd("123456");

    fixbug::LoginResponse login_response;

    // 业务使用时，不用关心 CallMethod（其在 stub.Login 里面）
    stub.Login(nullptr, &login_request, &login_response, nullptr);

    // 序列化与反序列化都不需要使用者负责
    // stub.Login() 已经将返回结果反序列化后保存到 response 

    if (login_response.result().errcode() == 0) {
        std::cout << "Rpc login response success: " << login_response.sucess() << std::endl;
    }
    fixbug::RegisterRequest reg_request;
    reg_request.set_id(2000);
    reg_request.set_name("mprpc");
    reg_request.set_pwd("123456");

    fixbug::RegisterResponse reg_response;
    stub.Register(nullptr, &reg_request, &reg_response, nullptr);

    if (reg_response.result().errcode() == 0) {
        std::cout << "Rpc register response success: " << login_response.sucess() << std::endl;
    }

    return 0;
}