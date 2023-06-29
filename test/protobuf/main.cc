#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main() {
    {
        LoginResponse rsp;
        ResCode *rc = rsp.mutable_code();
        rc->set_errcode(1);
        rc->set_errmsg("failed");
    }

    {
        GetFriendListResponse list_rsp;
        ResCode *rc = list_rsp.mutable_code();  // 返回成员变量 code 自定义类型
        rc->set_errcode(0);
        
        User* user1 = list_rsp.add_friend_list();  // friend_list 是列表型成员
        user1->set_name("zhang san");
        user1->set_age(20);
        user1->set_sex(User::MAN);  

        std::cout << list_rsp.friend_list_size() << std::endl;
    }

    {




        
    }


    return 0;
}
