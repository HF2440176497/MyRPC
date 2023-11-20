#pragma once

#include <queue>
#include <thread>
#include <condition_variable>

// 设计线程安全的 queue

// 我们考虑实际使用：
// 写日志线程需要应从消息队列中拿出消息，当队列为空，不应当
// 
// C++并发编程实战：wait_and_pop 当队列为空时不尝试加锁，个人认为符合我们的需求

template<typename T>
class LockQueue
{
public:
    void Push(T new_value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(new_value);
        m_cond.notify_one();
    }
    void Pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this]{ return !m_queue.empty(); });
        
        if (m_queue.empty()) {
            throw "Pop: False arousal and m_queue empty";
        }
        value = std::move(m_queue.front());  // 延长至引用的生命周期，必须是常引用
        m_queue.pop();
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};
