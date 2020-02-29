#pragma once

#include "NetUtil.h"
#include "TcpListener.h"
#include <string>
#include <functional>
#include "EventLoopThreadPool.h"

class TcpConnection;

class TcpServer
{
public:
  using Callback = std::function<int(Channel*)>;

  TcpServer(EventLoop* loop, const char* server_ip, const uint16_t port);

  ~TcpServer();



  void start();

  void set_read_callback(Callback callback);
  void set_write_callback(Callback callback);
  void set_error_callback(Callback callback);

private:

  int TcpServer::read_handle(Channel* channel);

  int TcpServer::write_handle(Channel* channel);

  void accept_connect_handle(int sockfd, const sockaddr_in& peer_addr);

  EventLoop* m_loop;

  TcpListener m_listener;

  EventLoopThreadPool m_threadpool;

  Callback m_read_callback;
  Callback m_write_callback;
  Callback m_error_callback;
};

TcpServer::TcpServer(EventLoop* loop, const char* server_ip, const uint16_t port)
  : m_loop(loop)
  , m_listener(m_loop, server_ip, port)
  , m_threadpool(20)
{
  using namespace std::placeholders;

  m_listener.set_callback(std::bind(
    &TcpServer::accept_connect_handle,
    this,
    _1,
    _2
  ));
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
  using namespace std::placeholders;
  m_threadpool.SetReadCallback(std::bind(&TcpServer::read_handle, this, _1));
  m_threadpool.SetWriteCallback(nullptr);
  m_threadpool.SetErrorCallback(nullptr);
  m_threadpool.Loop();
  m_listener.listen();
}

void TcpServer::accept_connect_handle(int sockfd, const sockaddr_in& peer_addr)
{


  NetUtil::nonblock(sockfd);
  NetUtil::nonagle(sockfd);

  auto [ip_addr, port] = NetUtil::show_sockaddr_in(peer_addr);

  printf("[ip:port] %s : %u", ip_addr.c_str(), port);

  if (m_threadpool.Empty()) {
    // TODO
  } else {
    m_threadpool.PushFd(sockfd);
  }

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

int TcpServer::read_handle(Channel* channel)
{
  auto buffer_in = channel->buffer_in();

  int save_errno = 0;

  auto n = buffer_in.readFd(channel->fd(), &save_errno);

  if (n > 0) {
    if (m_read_callback) m_read_callback(channel);
  } else if (n == 0) {

  } else {

  }
}

int TcpServer::write_handle(Channel* channel)
{
  // channel->is_writing()
  if (true) {
    ssize_t n = write(
      channel->fd(),
      channel->buffer_out().peek(),
      channel->buffer_out().readableBytes()
    );

    if (n > 0) {
      
    }
  } else {

  }
}