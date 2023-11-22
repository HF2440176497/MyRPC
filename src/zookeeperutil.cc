
#include "zookeeperutil.h"

#include "logger.h"
#include "mprpcapplication.h"  // 单例类读取配置文件

ZkClient::ZkClient() : m_zhandle(nullptr) {}

ZkClient::~ZkClient() {
    if (m_zhandle != nullptr)
        zookeeper_close(m_zhandle);
}

/**
 * @brief watcher function 回调函数，回调线程执行
 * @param zh
 * @param type
 * @param state
 * @param path
 * @param watcherCtx
 */
void global_watcher(zhandle_t *zh, int type,
                    int state, const char *path, void *watcherCtx) {
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_CONNECTED_STATE) {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

// 客户端连接 zkserver
void ZkClient::Start() {
    std::string host    = MprpcApplication::GetInstance().GetConfig().GetItem("zookeeperip");
    std::string port    = MprpcApplication::GetInstance().GetConfig().GetItem("zookeeperport");
    std::string connstr = host + ":" + port;

    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (nullptr == m_zhandle) {
        LOG_ERROR("zookeeper_init error!");
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    LOG_INFO("zookeeper_init");
}


/**
 * @brief 
 * @param path znode 树形目录路径
 * @param data 
 * @param datalen 
 * @param state 
 */
void ZkClient::Create(const char *path, const char *data, int datalen, int state) {
    char path_buffer[128];
    int  bufferlen = sizeof(path_buffer);
    int  flag;
    // 先判断 path 表示的znode节点是否存在
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if (ZNONODE == flag)  // 表示path的znode节点不存在
    {
        // 创建指定path的znode节点 data: 
        flag = zoo_create(m_zhandle, path, data, datalen,
                          &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if (flag == ZOK) {
            std::cout << "znode create success... path:" << path << std::endl;
        } else {
            std::cout << "flag:" << flag << std::endl;
            std::cout << "znode create error... path:" << path << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

// 根据指定的path，获取znode节点的值
std::string ZkClient::GetData(const char *path) {
    char buffer[64];
    int  bufferlen = sizeof(buffer);
    int  flag      = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if (flag != ZOK) {
        std::cout << "get znode error... path:" << path << std::endl;
        return "";
    } else {
        return buffer;
    }
}