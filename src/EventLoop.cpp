#include "EventLoop.h"
#include "CurrentThread.h"
#include "Channel.h"
#include <assert.h>

EventLoop::EventLoop()
  : m_looping(false)
  , m_quit(false)
  , m_thread_id(CurrentThread::tid())
{

}

EventLoop::~EventLoop()
{

}

#include <iostream>

void EventLoop::loop()
{
  m_looping = true;

  while (!m_quit) {
    m_active_channels.clear();
    m_epoller.poll(m_active_channels);

    puts("Poll done");

    process_active_events();
  }
}

void EventLoop::process_active_events()
{
  for (auto channel : m_active_channels) {
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

void EventLoop::add_channel(Channel* channel)
{
  if (has_channel(channel)) {
    // TODO
  } else {
    m_epoller.add_channel(channel);
    m_fd2channel_map.insert({channel->fd(), channel});
  }
}

void EventLoop::update_channel(Channel* channel)
{
  if (has_channel(channel)) {
    m_epoller.mod_channel(channel);
  } else {
    // TODO
  }
}

bool EventLoop::has_channel(Channel* channel)
{
  return m_fd2channel_map.find(channel->fd()) != m_fd2channel_map.end();
}