#pragma once

#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "CurrentThread.h"

class MutexLock
{
public:

    MutexLock()
        : holder_(0)
    {
        // TODO 锁类型可能得更改
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock()
    {
        assert(holder_ == 0);
        // TODO 没有检查返回值
        pthread_mutex_destroy(&mutex_);
    }

    bool is_locked_by_this_thread()
    {
        // 为避免每次都陷入系统调用，在第一次时缓存
        return holder_ == CurrentThread::tid();
    }

    void lock() // 仅供 MutexLockGuard 调用
    {
        // TODO 没有检查返回值
        pthread_mutex_lock(&mutex_);
        holder_ = gettid();
    }

    void unlock() // 仅供 MutexLockGuard 调用
    {
        holder_ = 0;
        // TODO 没有检查返回值
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* get_pthread_mutex() // 仅供 Codition 调用
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
    pid_t holder_; // 记录加锁的线程
};

class MutexLockGuard
{
public:
    explicit MutexLockGuard(MutexLock& mutex)
        : mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};

// 避免直接写 MutexLockGuard(mutex); 创建临时对象
#define MutexLockGuard(x) static_assert(false, "missing mutex guard var name")
