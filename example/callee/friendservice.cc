#include <iostream>
#include <string>
#include <vector>

#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "friend.pb.h"
#include "logger.h"

Logger& g_logger = Logger::GetInstance();

// 首先生成一个单例对象，作为全局变量

class FriendService : public fixbug::FriendServiceRpc {
public:
    std::vector<std::string> GetFriendsList(uint32_t friendid) {
        std::vector<std::string> vec{};
        vec.push_back("wanghf");
        vec.push_back("lianmj");
        return vec;
    }

    // 与 userservice 的 Login 类似
    // request 已经反序列化完成，response 作为结果需要序列化，交给 CallMethod 提供的回调函数，也就是这里的参数 done
    // 例如：RpcProvider::SendRpcResponse
    void GetFriendsList(google::protobuf::RpcController*       controller,
                        const ::fixbug::GetFriendsListRequest* request,
                        ::fixbug::GetFriendsListResponse*      response,
                        ::google::protobuf::Closure*           done) {
        uint32_t userid = request->userid();
        auto ret = GetFriendsList(userid);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");   

        for (auto& name: ret) {
            std::string* p = response->add_friends();
            *p = name;
        }
        done->Run();
    }
};


int main(int argc, char** argv) {
    MprpcApplication::Init(argc, argv);

    LOG_INFO("First log msg");
    LOG_ERROR("%s: %s: %d", __FILE__, __FUNCTION__, __LINE__);

    RpcProvider provider;
    provider.NotifyService(new FriendService());
    provider.Run();
    return 0;
}