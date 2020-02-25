#pragma once

#include "EPoller.h"
#include <map>
#include <functional>


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

private:

  void process_active_events();

  bool has_channel(Channel* channel);

  bool m_looping;
  bool m_quit;

  const pid_t m_thread_id;

  std::map<int, Channel*> m_fd2channel_map;

  std::vector<Channel*> m_active_channels;

  EPoller m_epoller;

  std::vector<Functor> m_pending_functors;
};
