#pragma once

#include "EPoller.h"
#include <map>
#include <functional>
#include "CurrentThread.h"

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

  void add_or_update_channel(Channel* channel);

private:

  void process_active_events(std::vector<Channel*>& active_channels);

  bool has_channel(Channel* channel);

  bool m_looping;
  bool m_quit;

  const pid_t m_thread_id;

  std::map<int, Channel*> m_fd2channel_map;

  std::vector<Channel*> m_active_channels;

  EPoller m_epoller;

  std::vector<Functor> m_pending_functors;
};

#include <assert.h>

EventLoop::EventLoop()
  : m_looping(false)
  , m_quit(false)
  , m_thread_id(CurrentThread::tid())
{

}

void EventLoop::loop()
{
  m_looping = true;

  while (!m_quit) {
    m_active_channels.clear();
    m_epoller.poll(m_active_channels);
  }
}

void EventLoop::process_active_events(std::vector<Channel*>& active_channels)
{
  for (auto channel : active_channels) {
    channel->handle_event();
  }
}

void EventLoop::quit()
{
  m_quit = true;
}

bool EventLoop::is_in_loop_thread() const
{
  return m_thread_id == CurrentThread::tid();
}

void EventLoop::assert_in_loop_thread()
{
  if (!is_in_loop_thread()) assert(false);
}

void EventLoop::add_or_update_channel(Channel* channel)
{
  if (has_channel(channel)) {
    m_epoller.ctl(channel->fd(), EPOLL_CTL_MOD, channel->events_watch());
  } else {
    m_epoller.ctl(channel->fd(), EPOLL_CTL_ADD, channel->events_watch());
    m_fd2channel_map.insert({channel->fd(), channel});
  }
}

bool EventLoop::has_channel(Channel* channel)
{
  return m_fd2channel_map.find(channel->fd()) != m_fd2channel_map.end();
}