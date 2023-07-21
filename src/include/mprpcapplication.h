#pragma once

#include "mprpcconfig.h"

// RPC 的初始化类：单例类
// 饿汉式实现 即程序执行，即保证静态实例已创建
class MprpcApplication {
private:
    MprpcApplication() {};
    ~MprpcApplication() {};
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;

public:
    static MprpcConfig m_config;

public:
    static void Init(int argc, char** argv);
    static MprpcApplication& GetInstance();
    MprpcConfig& GetConfig();
};

// 单例模式
// 
// 出发点：解决单例模式在多线程环境下的问题
// 
// 饿汉式：静态成员赋初值时直接赋予有效值，但是对于饿汉式单例对象不会被其他全局对象涉及
// 因为多个 cpp 文件的全局对象初始化顺序不确定

// 
// 避免多线程问题：
// 
// 内存释放问题：
// 懒汉式的实现，只需要在 GetInstance 函数中定义静态对象