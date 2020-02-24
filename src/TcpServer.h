#pragma once

#include "NetUtil.h"
#include "Channel.h"
#include "EventLoop.h"
#include <string>
#include <functional>

class TcpServer
{
public:
  using Callback = std::function<void()>;

  TcpServer(const char* server_ip, const uint16_t port);

  ~TcpServer();

  void accept_connect_handle(EventLoop& eventLoop);

  void start();

  void set_read_callback(Callback callback);
  void set_write_callback(Callback callback);
  void set_error_callback(Callback callback);

private:
  int m_sockfd;

  Channel m_channel;

  EventLoop m_loop;

  Callback m_read_callback;
  Callback m_write_callback;
  Callback m_error_callback;


};

TcpServer::TcpServer(const char* server_ip, const uint16_t port)
  : m_sockfd(NetUtil::listen(server_ip, port))
  , m_channel(m_sockfd)
{
  m_channel.set_read_callback(std::bind(
    TcpServer::accept_connect_handle,
    this,
    std::placeholders::_1
  ));

  m_channel.set_events(EPOLLIN);
  m_loop.add_channel(&m_channel);
}

void TcpServer::accept_connect_handle(EventLoop & eventLoop)
{

}

void TcpServer::set_read_callback(Callback callback)
{
  m_read_callback = callback;
}

void TcpServer::set_write_callback(Callback callback)
{
  m_write_callback = callback;
}

void TcpServer::set_error_callback(Callback callback)
{
  m_error_callback = callback;
}