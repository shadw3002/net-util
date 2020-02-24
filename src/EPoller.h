#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include "Channel.h"


class EPoller
{
public:
  EPoller();

  ~EPoller();

  void ctl(int fd, int op, uint32_t events);

  void poll(std::vector<Channel*>& active_channels);

private:
  int m_epfd;

  std::vector<struct epoll_event> m_events;
};

EPoller::EPoller()
{
  m_epfd = epoll_create1(0);
}

EPoller::~EPoller()
{
  close(m_epfd);
}

void EPoller::ctl(int op, int fd, uint32_t events)
{
  struct epoll_event event; {
    event.events = events;
  }

  epoll_ctl(m_epfd, op, fd, &event);
}

void EPoller::poll(std::vector<Channel*>& active_channels)
{
  int num_events = epoll_wait(
    m_epfd,
    m_events.data(),
    m_events.size(),
    -1
  );

  if (num_events > 0) {
    for (auto& event: m_events) {
      Channel* channel = static_cast<Channel*>(event.data.ptr);
      channel->set_events_recv(event.events);
      active_channels.push_back(channel);
    }
  }
  else if (num_events == 0) {
    // nothing to do
  }
  else {
    // error
  }
}