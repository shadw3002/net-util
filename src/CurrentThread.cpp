#include "CurrentThread.h"


__thread int CurrentThread::t_cachedTid = 0;

void CurrentThread::cache_tid() {
  if (t_cachedTid == 0) {
    t_cachedTid = gettid();
  }
}

