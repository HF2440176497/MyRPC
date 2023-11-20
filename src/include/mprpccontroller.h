
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"

// RpcController 所有的成员函数都是纯虚函数，是抽象类
// MprpcController 作为派生类重载所有纯虚函数 才能实例化对象
class MprpcController: public google::protobuf::RpcController
{
public:
    MprpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    // 未实现具体功能
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);
private: 
    bool m_failed;
    std::string m_errText;
};
