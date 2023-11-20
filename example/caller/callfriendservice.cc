#include <iostream>
#include <string>
#include "friend.pb.h"  // 业务 proto 协议
#include "mprpcapplication.h"


int main(int argc, char** argv) {
    MprpcApplication::Init(argc, argv);
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    
    fixbug::GetFriendsListRequest request;
    request.set_userid(1000);

    // conrtroller 控制参数
    MprpcController controller;

    fixbug::GetFriendsListResponse response;
    // stub.GetFriendsList(nullptr, &request, &response, nullptr);
    stub.GetFriendsList(&controller, &request, &response, nullptr);

    // channel->CallMethod 会传出 controller 参数
    if (controller.Failed()) {
        std::cout << "Error Meaasge = " << controller.ErrorText() << std::endl;
        return -1;
    }
    if (response.result().errcode() == 0) {
        std::cout << "friend size = " << response.friends_size() << std::endl;
        for (int i = 0; i < response.friends_size(); ++i) {
            std::cout << "index: " << (i+1) << response.friends(i) << std::endl;
        }
    } else {
        std::cout << "get friend list error: " << response.result().errmsg() << std::endl;
    }
    return 0;
}