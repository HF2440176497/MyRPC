#include "lockqueue.h"

// template<typename T>
// void LockQueue<T>::Push(T new_value) {
//     std::lock_guard<std::mutex> lock(m_mutex);
//     m_queue.push(new_value);
//     m_cond.notify_one();
// }

// 目前只有一个线程读取消息队列
// template<typename T>
// T& LockQueue<T>::Pop() {
//     std::unique_lock<std::mutex> lock(m_mutex);
//     m_cond.wait(lock, [this]{ return !m_queue.empty(); });
    
//     if (m_queue.empty()) {
//         throw "Pop: False arousal and m_queue empty";
//     }
//     T& value = std::move(m_queue.front());  // 延长至引用的生命周期
//     m_queue.pop();
//     return value;
// }