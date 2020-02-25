#pragma once

#include <functional>

class EventLoop;

class Channel
{
public:
  using Callback = std::function<void()>;

  Channel(EventLoop* owner_loop, int fd);

  int fd() const;

  int events_watch() const;
  void set_events_watch(int events);

  int events_recv() const;
  void set_events_recv(int events);

  void enable_read();
  void enable_write();
  void disable_write();
  void disable_all();

  void set_read_callback(Callback callback);
  void set_write_callback(Callback callback);
  void set_error_callback(Callback callback);

  void handle_event();

private:

  void update();

  int m_fd;

  int m_events_watch;

  int m_events_recv;

  EventLoop* m_owner_loop;

  Callback m_read_callback;
  Callback m_write_callback;
  Callback m_error_callback;
};

#include "EventLoop.h"
#include <sys/epoll.h>

Channel::Channel(EventLoop* owner_loop, int fd)
  : m_owner_loop(owner_loop)
  , m_fd(0)
  , m_events_watch(0)
  , m_events_recv(0)
{
  m_owner_loop->add_channel(this);
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

void Channel::enable_read()
{
  m_events_watch |= (EPOLLIN | EPOLLPRI);
  update();
}

void Channel::enable_write()
{
  m_events_watch |= EPOLLOUT;
  update();
}

void Channel::disable_write()
{
  m_events_watch &= ~EPOLLOUT;
  update();
}

void Channel::disable_all()
{
  m_events_watch = 0;
  update();
}

void Channel::update()
{
  m_owner_loop->update_channel(this);
}