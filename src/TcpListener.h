#pragma once

#include <boost/noncopyable.hpp>
#include <functional>

#include "EventLoop.h"
#include "Channel.h"
#include "NetUtil.h"

class TcpListener : boost::noncopyable
{
public:
  using Callback = std::function<void(int sockfd, const sockaddr_in& peer_addr)>;

  TcpListener(EventLoop* loop, const char* ip, int16_t port);

  ~TcpListener();

  void set_callback(const Callback& callback);

  void listen();

private:

  void accept_connection();

  EventLoop* m_loop;

  int m_sockfd;

  Channel m_channel;

  Callback m_callback;
};

#include <sys/socket.h>

TcpListener::TcpListener(EventLoop* loop, const char* ip, int16_t port)
  : m_loop(loop)
  , m_sockfd(NetUtil::listen(ip, port))
  , m_channel(m_loop, m_sockfd)
  , m_callback(nullptr)
{
  m_channel.set_read_callback(std::bind(&TcpListener::accept_connection, this));
}

TcpListener::~TcpListener()
{
  close(m_sockfd);
}

void TcpListener::listen()
{
  // SOMAXCONN 为设置半连接队列长度
  int ret = ::listen(m_sockfd, SOMAXCONN);
  if (ret < 0) {
    // TODO
  }

  m_channel.enable_read();
}

void TcpListener::set_callback(const Callback& callback)
{
  m_callback = callback;
}

#include <iostream>

void TcpListener::accept_connection()
{
  auto [conn_fd, peer_addr] = NetUtil::accept(m_sockfd);

  if (conn_fd >= 0) {
    if (m_callback) m_callback(conn_fd, peer_addr);
    else close(m_sockfd); // TODO
  }
}
