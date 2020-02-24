#pragma once

#include "EventLoop.h"
#include "Channel.h"
#include <functional>

class Address;

class Listener
{
public:
  typedef
  std::function<void(int fd,const Address& addr)>
  Callback;

  Listener(EventLoop* loop, const struct sockaddr_in& addr);

  void set_callback(const Callback& callback);

  void listen();

private:

  EventLoop* m_loop_;

  int m_sockfd;

  Channel m_channel;
};

#include <sys/socket.h>

int create_socket()
{
  return ::socket(
    AF_INET,
    SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
    IPPROTO_TCP
  );
}

Listener::Listener(EventLoop* loop, const struct sockaddr_in& addr)
  : m_loop_(loop)
  , m_sockfd(create_socket())
  , m_channel(loop, m_sockfd)
{
  int ret = ::bind(m_sockfd, sockaddr_cast(&addr), sizeof(addr));
}

