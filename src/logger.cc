
#include "logger.h"

#include <time.h>

#include <iostream>
#include <thread>

Logger::Logger() : m_lckQue({}), m_loglevel(INFO) {
    std::thread writeLogTask([&]() {  // 按引用捕获，在类中相当于捕获 this
        for (;;) {
            // 获取当前日期
            time_t now    = time(nullptr);
            tm*    now_tm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", now_tm->tm_year + 1900, now_tm->tm_mon, now_tm->tm_mday);

            // 需要尝试取出消息队列中的消息，进行额外处理
            std::string msg;
            m_lckQue.Pop(msg);

            char time_buf[128]{};
            sprintf(time_buf, "%d:%d:%d => [%S]",
                    now_tm->tm_hour,
                    now_tm->tm_min,
                    now_tm->tm_sec,
                    LogLevel_tab[m_loglevel]);
            msg.insert(0, time_buf);
            msg.append("\n");

            // 写入相应日志文件
            FILE* pf = fopen(file_name, "a+");
            if (pf == nullptr) {
                std::cout << "logger file: " << file_name << "open error" << std::endl;
                exit(EXIT_FAILURE);
            }
            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    writeLogTask.detach();
}

void Logger::SetLogLevel(LogLevel level) {
    m_loglevel = level;
}

void Logger::Log(std::string msg) {
    m_lckQue.Push(msg);
}

Logger& Logger::GetInstance() {
    static Logger m_instance;
    return m_instance;
}
