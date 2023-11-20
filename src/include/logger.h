#pragma once

#include <stdio.h>
#include <string>
#include "lockqueue.h"

enum LogLevel {
    INFO,
    ERROR,
};

// 能够按照日志等级查找等级字符串，便于扩展
static std::string LogLevel_tab[] = {
    "INFO",
    "ERROR"
};


// 设置成单例类 饿汉式实现
class Logger {
public:
    void SetLogLevel(LogLevel);
    void Log(std::string);

public:
    static Logger& GetInstance();

private:
    int                    m_loglevel;
    LockQueue<std::string> m_lckQue;

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&)      = delete;
};

#define LOG_INFO(logmsgformat, ...)                      \
    do {                                                 \
        Logger& logger = Logger::GetInstance();          \
        logger.SetLogLevel(INFO);                        \
        char str[1024];                                  \
        snprintf(str, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(str);                                 \
    } while (0);


#define LOG_ERROR(logmsgformat, ...)                      \
    do {                                                 \
        Logger& logger = Logger::GetInstance();          \
        logger.SetLogLevel(ERROR);                        \
        char str[1024];                                  \
        snprintf(str, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(str);                                 \
    } while (0);

// ##__VA_ARGS__ 前面有逗号，## 是用于连接两个语言符号
// 无可变参数时，可以去掉前面逗号