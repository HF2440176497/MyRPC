
#pragma once

#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"

#include "rpcheader.pb.h"

class MprpcChannel: public google::protobuf::RpcChannel 
{
public:
    // 通过 Stub 代理对象调用的方法，会走到这里
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done);


};