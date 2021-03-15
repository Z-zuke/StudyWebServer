#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <exception>
#include <list>
#include <cstdio>
#include "locker.h"

/* 线程池类，定义为模板类，模板参数 T 为任务类  */
template<typename T>
class threadpool {
public:
    // thread_number 是线程池中线程的数量
    // max_requests 是请求队列中最多允许的、等待处理的请求数量
    threadpool(int thread_number = 8, int max_requests = 10000);
    ~threadpool();
    bool append(T* request);

private:
    // 工作线程运行的函数，它不断从工作队列中取出任务并执行之
    static void* worker(void* arg);
    void run();

private:
    // 线程的数量
    int m_thread_number;

    // 描述线程池的数组，大小为 m_thread_number
    pthread_t *m_threads;

    // 请求队列中最多允许的、等待处理的请求数量
    int m_max_requests;

    // 请求队列
    std::list<T*> m_workQueue;

    // 保护请求队列的互斥锁
    locker m_queueLocker;

    // 是否有任务需要处理
    sem m_queueStat;

    // 是否结束线程
    bool m_stop;
};

template<typename T>
threadpool< T >::threadpool(int thread_number, int max_requests) :
        m_thread_number(thread_number), m_max_requests(max_requests),
        m_stop(false), m_threads(NULL) {
            
    if ((thread_number <= 0) || (max_requests <= 0)) {
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];
    if (!m_threads) {
        throw std::exception();
    }

    // 创建 thread_number 个线程，并将其设置为脱离线程
    for (int i = 0; i < thread_number; ++i) {
        printf("create the %dth thread\n", i);
        if (pthread_create(m_threads + i, NULL, worker, this) != 0) {
            delete [] m_threads;
            throw std::exception();
        }

        if (pthread_detach(m_threads[i])) {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template< typename T>
threadpool< T >::~threadpool() {
    delete [] m_threads;
    m_stop = true;
}

template< typename T>
bool threadpool< T >::append(T* request) {
    // 操作工作队列时，一定要加锁，因为它被所有线程共享
    m_queueLocker.lock();
    if (m_workQueue.size() > m_max_requests) {
        m_queueLocker.unlock();
        return false;
    }

    m_workQueue.push_back(request);
    m_queueLocker.unlock();
    m_queueStat.post();
    return true;
}

template< typename T >
void* threadpool< T >::worker(void* arg) {
    threadpool* pool = (threadpool*)arg;
    pool->run();
    return pool;
}

template< typename T >
void threadpool< T >::run() {
    while(!m_stop) {
        m_queueStat.wait();
        m_queueLocker.lock();
        if (m_workQueue.empty()) {
            m_queueLocker.unlock();
            continue;
        }
        T* request = m_workQueue.front();
        m_workQueue.pop_front();
        m_queueLocker.unlock();
        if (!request) {
            continue;
        }
        request->process();
    }
}


#endif