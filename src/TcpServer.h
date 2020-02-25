#pragma once

#include "NetUtil.h"
#include "TcpListener.h"
#include <string>
#include <functional>

class TcpConnection;

class TcpServer
{
public:
  using Callback = std::function<void(const TcpConnection&)>;

  TcpServer(EventLoop* loop, const char* server_ip, const uint16_t port);

  ~TcpServer();

  void accept_connect_handle(const TcpConnection* conn);

  void start();

  void set_read_callback(Callback callback);
  void set_write_callback(Callback callback);
  void set_error_callback(Callback callback);

private:
  EventLoop* m_loop;

  TcpListener m_listener;

  Callback m_read_callback;
  Callback m_write_callback;
  Callback m_error_callback;
};

TcpServer::TcpServer(EventLoop* loop, const char* server_ip, const uint16_t port)
  : m_loop(loop)
  , m_listener(m_loop, server_ip, port)
{

}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
  m_listener.listen();
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