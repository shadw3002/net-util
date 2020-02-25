#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

class Channel;

class EPoller
{
public:
  EPoller();

  ~EPoller();

  void add_channel(Channel* channel);

  void mod_channel(Channel* channel);

  void del_channel(Channel* channel);

  void poll(std::vector<Channel*>& active_channels);

private:
  int m_epfd;

  void ctl(int op, Channel* channel);

  std::vector<struct epoll_event> m_events;
};

