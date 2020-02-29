#pragma once

#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

// from https://www.zhihu.com/question/27908489
class ThreadPool
{
public:
  using Functor = std::function<void()>;

  explicit ThreadPool(size_t num_threads);

  ThreadPool() = delete;

  ThreadPool(ThreadPool&&) = delete;

  ~ThreadPool();

  template<class F>
  void execute(F&& task);

private:
  struct Data {
    std::mutex mtx;
    std::condition_variable cond;
    bool is_shutdown;
    std::queue<Functor> tasks;
  };

  std::shared_ptr<Data> m_data;
};

ThreadPool::ThreadPool(size_t thread_count)
  : m_data()
{
  for (size_t i = 0; i < thread_count; ++i) {
    std::thread([data = m_data] {
      std::unique_lock<std::mutex> lk(data->mtx);
      for (;;) {
        if (!data->tasks.empty()) {
          auto current = std::move(data->tasks.front());
          data->tasks.pop();
          lk.unlock();
          current();
          lk.lock();
        } else if (data->is_shutdown) {
          break;
        } else {
          data->cond.wait(lk);
        }
      }
    }).detach();
  }
}

ThreadPool::~ThreadPool()
{
  if ((bool)m_data) {
    {
      std::lock_guard<std::mutex> lk(m_data->mtx);
      m_data->is_shutdown = true;
    }
    m_data->cond.notify_all();
  }
}

template<class F>
void ThreadPool::execute(F&& task)
{
  {
    std::lock_guard<std::mutex> lk(m_data->mtx);
    m_data->tasks.emplace(std::forward<F>(task));
  }
  m_data->cond.notify_one();
}