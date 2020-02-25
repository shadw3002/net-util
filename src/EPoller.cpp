#include "Channel.h"
#include "EPoller.h"

#include "string.h"

#include <iostream>

EPoller::EPoller()
  : m_epfd(epoll_create1(EPOLL_CLOEXEC))
  , m_events(64)
{
  if (m_epfd < 0) {
    puts("create epfd error");
  }
}

EPoller::~EPoller()
{
  close(m_epfd);
}

void EPoller::ctl(int op, Channel* channel)
{
  struct epoll_event event; {
    bzero(&event, sizeof(event));
    event.events = channel->events_watch();
    event.data.ptr = channel;
  }

  int fd = channel->fd();

  if (epoll_ctl(m_epfd, op, fd, &event) < 0) {
    // TODO
  }
}

#include <iostream>

void EPoller::poll(std::vector<Channel*>& active_channels)
{
  std::cout << "poll size: " << m_events.size() << std::endl;

  int num_events = epoll_wait(
    m_epfd,
    m_events.data(),
    m_events.size(),
    10000
  );

  std::cout << "new events: " << num_events << std::endl;

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

void EPoller::add_channel(Channel* channel)
{
  puts("epoll ctl add");
  ctl(EPOLL_CTL_ADD, channel);
}

void EPoller::mod_channel(Channel* channel)
{
  puts("epoll ctl mod");
  ctl(EPOLL_CTL_MOD, channel);
}

void EPoller::del_channel(Channel* channel)
{
  puts("epoll ctl del");
  ctl(EPOLL_CTL_DEL, channel);
}
