#pragma once

#include <functional>

class Channel
{
public:
  using Callback = std::function<void()>;

  Channel(int fd);

  int fd() const;

  int events_watch() const;
  void set_events_watch(int events);

  int events_recv() const;
  void set_events_recv(int events);

  void set_read_callback(Callback callback);
  void set_write_callback(Callback callback);
  void set_error_callback(Callback callback);

  void handle_event();

private:

  int m_fd;

  int m_events_watch;

  int m_events_recv;

  Callback m_read_callback;
  Callback m_write_callback;
  Callback m_error_callback;
};

#include "EventLoop.h"
#include <sys/epoll.h>

Channel::Channel(int fd)
  : m_fd(0)
  , m_events_watch(0)
  , m_events_recv(0)
{

}

int Channel::events_recv() const
{
  return m_events_recv;
}

void Channel::set_events_recv(int events)
{
  m_events_recv = events;
}

int Channel::events_watch() const
{
  return m_events_watch;
}

void Channel::set_events_watch(int events)
{
  m_events_watch = events;
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

void Channel::handle_event()
{
	if (m_events_recv & (EPOLLHUP | EPOLLERR))
	{
		if (m_error_callback) m_error_callback();
		// TODO
	}
	else
	{
		if (m_events_recv & (EPOLLIN | EPOLLRDHUP))
		{
			if (m_read_callback) m_read_callback();
		}
		if (m_events_recv & EPOLLOUT)
		{
			if (m_write_callback) m_write_callback();
		}
	}
}