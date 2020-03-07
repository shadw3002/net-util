#pragma once

#include "EPoller.h"
#include <map>
#include <functional>
#include <mutex>
#include "Channel.h"
#include "Timer.h"
#include "Heap.h"

class EventLoop
{
public:
  using Functor = std::function<void()>;

  EventLoop();

  ~EventLoop();

  void loop();

  // 停止, 非即时
  void quit();

  bool is_in_loop_thread() const;

  void assert_in_loop_thread();

  // 仅供 Channel 构造函数调用
  void add_channel(Channel* channel);

  // 仅供 Channel 构造函数调用
  void update_channel(Channel* channel);

  void remove_channel(Channel* channel);

  void do_functor(const Functor& functor);

  void queue_functor(const Functor& functor);

  void add_timer(Timer* timer);

private:
  void wake_up();

  void process_active_time_events();

  void process_active_events();

  void do_pending_functors();

  bool has_channel(Channel* channel);

  bool m_looping;

  const pid_t m_thread_id;

  std::map<int, Channel*> m_fd2channel_map;

  std::vector<Channel*> m_active_channels;

  EPoller m_epoller;

  std::vector<Functor> m_pending_functors;

  std::mutex m_mtx;

  Channel m_wakeup_channel;

  Heap<Timer*> m_timerheap;
};
