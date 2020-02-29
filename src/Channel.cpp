#include "Channel.h"
#include "EventLoop.h"
#include <sys/epoll.h>

Channel::Channel(EventLoop* owner_loop, int fd)
  : m_owner_loop(owner_loop)
  , m_fd(fd)
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

  update();
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

void Channel::set_close_callback(Callback callback)
{
  m_close_callback = callback;
}

int Channel::fd() const
{
  return m_fd;
}

void Channel::handle_event()
{


	if (m_events_recv & (EPOLLHUP | EPOLLERR))
	{
    puts("handle error");
		if (m_error_callback) m_error_callback(this);
		// TODO
	}
	else
	{
		if (m_events_recv & (EPOLLIN | EPOLLRDHUP))
		{
      puts("handle read");
			if (m_read_callback) m_read_callback(this);
		}
		if (m_events_recv & EPOLLOUT)
		{
      puts("handle write");
			if (m_write_callback) m_write_callback(this);
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

bool Channel::is_writing() const
{
  return m_events_watch & EPOLLOUT;
}