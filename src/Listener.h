#pragma once

#include <boost/noncopyable.hpp>
#include <functional>

#include "EventLoop.h"
#include "Channel.h"
#include "NetUtil.h"

class Listener : boost::noncopyable
{
public:
  using Callback = std::function<void(int sockfd, const sockaddr_in& peer_addr)>;

  Listener(EventLoop* loop, const char* ip, int16_t port);

  void set_callback(const Callback& callback);

  void listen();

private:

  EventLoop* m_loop_;

  int m_sockfd;

  Callback m_callback;

  Channel m_channel;
};

#include <sys/socket.h>

Listener::Listener(EventLoop* loop, const char* ip, int16_t port)
  : m_loop_(loop)
  , m_sockfd(NetUtil::listen(ip, port))
  , m_channel(m_sockfd)
  , m_callback(nullptr)
{

}

void Listener::set_callback(const Callback& callback)
{
  m_callback = callback;
}
