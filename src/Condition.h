#pragma once

#include "Mutex.h"

class Condition
{
public:
    Condition(const Condition&) = delete;
    Condition& operator=(const Condition&) = delete;

    explicit Condition(MutexLock& mutex)
        : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.get_pthread_mutex());
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notify_all()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};