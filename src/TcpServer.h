#pragma once

#include "NetUtil.h"
#include "Channel.h"
#include "EventLoop.h"
#include <string>
#include <functional>

class TcpConnection;

class TcpServer
{
public:
  using Callback = std::function<void(const TcpConnection&)>;

  TcpServer(const char* server_ip, const uint16_t port);

  ~TcpServer();

  void accept_connect_handle(const TcpConnection* conn);

  void start();

  void set_read_callback(Callback callback);
  void set_write_callback(Callback callback);
  void set_error_callback(Callback callback);

private:
  int m_sockfd;

  Channel m_channel;

  EventLoop m_eventloop;

  Callback m_read_callback;
  Callback m_write_callback;
  Callback m_error_callback;
};

TcpServer::TcpServer(const char* server_ip, const uint16_t port)
  : m_sockfd(NetUtil::listen(server_ip, port))
  , m_channel(m_sockfd)
{
  m_channel.set_read_callback(std::bind(
    &TcpServer::accept_connect_handle,
    std::placeholders::_1
  ));

  m_channel.set_events_watch(EPOLLIN);
  m_eventloop.add_or_update_channel(&m_channel);
}

TcpServer::~TcpServer()
{
  close(m_sockfd);
}

void TcpServer::start()
{

  m_eventloop.loop();
}

void TcpServer::accept_connect_handle(const TcpConnection* conn)
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