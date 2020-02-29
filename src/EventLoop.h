#pragma once

#include "EPoller.h"
#include <map>
#include <functional>
#include <mutex>
#include "Channel.h"

class EventLoop
{
public:
  typedef std::function<void()> Functor;

  EventLoop();

  ~EventLoop();

  void loop();

  void quit();

  bool is_in_loop_thread() const;

  void assert_in_loop_thread();

  void add_channel(Channel* channel);

  void update_channel(Channel* channel);

  void do_functor(const Functor& functor);

  void wake_up();

  void queue_functor(const Functor& functor);

  void remove_channel(Channel* channel);

private:


  void process_active_events();

  void do_pending_functors();

  bool has_channel(Channel* channel);

  bool m_looping;
  bool m_quit;

  const pid_t m_thread_id;

  std::map<int, Channel*> m_fd2channel_map;

  std::vector<Channel*> m_active_channels;

  EPoller m_epoller;

  std::vector<Functor> m_pending_functors;

  std::mutex m_mtx;

  Channel m_wakeup_channel;
};
