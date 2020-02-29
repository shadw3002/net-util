#pragma once

#include "NetUtil.h"
#include "TcpListener.h"
#include <string>
#include <functional>
#include "EventLoopPool.h"
#include "TcpConnection.h"
#include <map>

class TcpConnection;

class TcpServer
{
public:
  TcpServer(EventLoop* loop, const char* server_ip, const uint16_t port);

  ~TcpServer();



  void start();

  void set_read_callback(TcpConnection::ReadCompleteCallback callback);
  void set_write_callback(TcpConnection::WriteCompleteCallback callback);
  void set_connected_callback(TcpConnection::ConnectedCallback callback);

private:
  void remove_connection(const TcpConnection::Ptr& conn);

  void remove_connection_worker(const TcpConnection::Ptr& conn);

  void accept_connect_handle(int sockfd, const sockaddr_in& peer_addr);

  EventLoop* m_loop;

  TcpListener m_listener;

  EventLoopPool m_looppool;

  int m_conn_cnt;

  std::map<std::string, TcpConnection::Ptr> m_conns;

  TcpConnection::ReadCompleteCallback m_read_callback;
  TcpConnection::WriteCompleteCallback m_write_callback;
  TcpConnection::ConnectedCallback m_connected_callback;
};

TcpServer::TcpServer(EventLoop* loop, const char* server_ip, const uint16_t port)
  : m_loop(loop)
  , m_listener(m_loop, server_ip, port)
  , m_looppool(m_loop, 20)
  , m_conn_cnt(0)
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

  puts("Tcpserver starting");

  m_looppool.start();
  m_listener.listen();
}

void TcpServer::accept_connect_handle(int sockfd, const sockaddr_in& peer_addr)
{
  using namespace std::placeholders;

  NetUtil::nonblock(sockfd);
  NetUtil::nonagle(sockfd);

  auto [ip_addr, port] = NetUtil::show_sockaddr_in(peer_addr);

  char buf[32]; {
    snprintf(buf, sizeof buf, "#%d", m_conn_cnt);
    m_conn_cnt++;
  }

  auto name = std::string(buf);
  auto ioloop = m_looppool.next_loop();

  TcpConnection::Ptr conn(std::make_shared<TcpConnection>(
    ioloop,
    name,
    sockfd
  ));
  m_conns[name] = conn;

  conn->set_read_complete_callback(m_read_callback);
  conn->set_write_complete_callback(m_write_callback);
  conn->set_connected_callback(m_connected_callback);
  conn->set_close_callback(std::bind(
    &TcpServer::remove_connection, this, _1
  ));

  ioloop->push_functor(std::bind(&TcpConnection::established, conn));
}

void TcpServer::remove_connection(const TcpConnection::Ptr& conn)
{
  m_loop->push_functor(std::bind(&TcpServer::remove_connection_worker, this, conn));
}

void TcpServer::remove_connection_worker(const TcpConnection::Ptr& conn)
{
  m_conns.erase(conn->name());
  conn->loop()->queue_functor(std::bind(&TcpConnection::destroy, conn));
}

void TcpServer::set_read_callback(TcpConnection::ReadCompleteCallback callback)
{
  m_read_callback = callback;
}

void TcpServer::set_write_callback(TcpConnection::WriteCompleteCallback callback)
{
  m_write_callback = callback;
}

void TcpServer::set_connected_callback(TcpConnection::ConnectedCallback callback)
{
  m_connected_callback = callback;
}