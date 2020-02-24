#pragma once

class Channel
{
public:
  using Callback = std::function<void(EventLoop&)>;

  Channel(int fd);

  int fd() const;

  int events() const;

  void set_events(int events);

  void set_read_callback(Callback callback);
  void set_write_callback(Callback callback);
  void set_error_callback(Callback callback);

private:

  int m_fd;

  int m_events;

  Callback m_read_callback;
  Callback m_write_callback;
  Callback m_error_callback;
};

#include "EventLoop.h"

Channel::Channel(int fd)
  : m_fd(0)
  , m_events(0)
{

}

int Channel::events() const
{
  return m_events;
}

void Channel::set_events(int events)
{
  m_events = events;
}

void Channel::set_read_callback(Callback callback)
{
  m_read_callback = callback;
}

void Channel::set_write_callback(Callback callback)
{
  m_write_callback = callback;
}

void Channel::set_error_callback(Callback callback)
{
  m_error_callback = callback;
}

int Channel::fd() const
{
  return m_fd;
}