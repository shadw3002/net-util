#pragma once

#include "EPoller.h"
#include <map>
#include "CurrentThread.h"

class EventLoop
{
public:
  EventLoop();

  ~EventLoop();

  void loop();

  void quit();

  bool is_in_loop_thread() const;

private:

  void process_active_events(std::vector<Channel*>& active_channels);

  bool m_looping;
  bool m_quit;

  const pid_t m_thread_id;

  std::map<int, Channel*> m_fd_to_channel_map;

  std::vector<Channel*> m_active_channels;

  EPoller m_epoller;
};

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