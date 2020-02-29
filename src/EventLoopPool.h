#pragma

#include "EventLoop.h"
#include "ThreadPool.h"
#include <vector>

class EventLoopPool
{
public:
  EventLoopPool(EventLoop* owner_pool, size_t num_threads);

  ~EventLoopPool();

  void start();

  EventLoop* next_loop();

private:
  EventLoop* m_owner_loop;



  std::vector<EventLoop*> m_loops;

  size_t m_num_threads;

  size_t m_next;

  ThreadPool m_thread_pool;
};

#include <iostream>

EventLoopPool::EventLoopPool(EventLoop* owner_loop, size_t num_threads)
  : m_owner_loop(owner_loop)
  , m_num_threads(num_threads)
  , m_next(0)
  , m_thread_pool(m_num_threads)
{

}

EventLoopPool::~EventLoopPool()
{

}

void EventLoopPool::start()
{
  // should only call once

  for (int i = 0; i < m_num_threads; i++) {
    EventLoop* loop = new EventLoop;

    m_thread_pool.execute(std::bind(&EventLoop::loop, loop));
    m_loops.push_back(loop);
  }
}

EventLoop* EventLoopPool::next_loop()
{
  EventLoop* loop = m_owner_loop;

  if (!m_loops.empty()) {
    loop = m_loops[m_next];
    m_next++;
    if (m_next >= m_loops.size()) m_next = 0;
  }

  return loop;
}