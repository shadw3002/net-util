#pragma once

#include <unistd.h>

namespace CurrentThread {
    // 相当于每个线程的全局变量
    __thread int t_cachedTid;

    void cache_tid();

    inline int tid() {
        if (__builtin_expect(t_cachedTid == 0, 0)) {
            cache_tid();
        }
        return t_cachedTid;
    }

    void CurrentThread::cache_tid() {
        if (t_cachedTid == 0) {
            t_cachedTid = gettid();
        }
    }
}