#pragma once

#include <boost/noncopyable.hpp>
#include "EventLoop.h"
#include "string.h"
#include "Channel.h"
#include <memory>
#include <functional>
#include "Buffer.h"

class TcpConnection : boost::noncopyable
                    , public std::enable_shared_from_this<TcpConnection>
{
public:
  using Ptr = std::shared_ptr<TcpConnection>;

  using ReadCompleteCallback = std::function<void(const Ptr&, Buffer*)>;

  using WriteCompleteCallback = std::function<void(const Ptr&)>;

  using CloseCallback = std::function<void(const Ptr&)>;

  using ConnectedCallback = std::function<void(const Ptr&)>;

  TcpConnection(EventLoop* loop,
                const std::string& name,
                int sockfd
                );

  ~TcpConnection();

  EventLoop* loop() const;

  const std::string& name() const;

  bool connected() const;

  void send(const std::string& message);

  void shutdown();

  void established();

  void destroy();

  void set_read_complete_callback(ReadCompleteCallback cb);

  void set_write_complete_callback(WriteCompleteCallback cb);

  void set_close_callback(CloseCallback cb);

  void set_connected_callback(ConnectedCallback cb);

private:

  void send_worker(const std::string& message);

  void handle_read();

  void handle_write();

  void handle_close();

  void handle_error();



  EventLoop* m_loop;

  std::string m_name;

  Channel* m_channel;

  Buffer m_buffer_in;

  Buffer m_buffer_out;

  ReadCompleteCallback m_read_complete_cb;

  WriteCompleteCallback m_write_complete_cb;

  CloseCallback m_close_cb;

  ConnectedCallback m_conn_cb;
};

#include <iostream>

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd
                             )
  : m_loop(loop)
  , m_name(name)
  , m_channel(new Channel(m_loop, sockfd))
{
  using namespace std::placeholders;

  m_channel->set_read_callback(std::bind(
    &TcpConnection::handle_read, this
  ));

  m_channel->set_write_callback(std::bind(
    &TcpConnection::handle_write, this
  ));

  m_channel->set_error_callback(std::bind(
    &TcpConnection::handle_error, this
  ));

  m_channel->set_close_callback(std::bind(
    &TcpConnection::handle_close, this
  ));
}

TcpConnection::~TcpConnection()
{

}

const std::string& TcpConnection::name() const
{
  return m_name;
}

EventLoop* TcpConnection::loop() const
{
  return m_loop;
}

void TcpConnection::destroy()
{
  m_channel->disable_all();

  m_loop->remove_channel(m_channel);
}

void TcpConnection::established()
{
  m_channel->enable_read();
  if (m_conn_cb) m_conn_cb(shared_from_this());
}

void TcpConnection::handle_read()
{
  int ret_errno = 0;
  ssize_t n = m_buffer_in.readFd(m_channel->fd(), &ret_errno);

  if (n > 0) {
    m_read_complete_cb(shared_from_this(), &m_buffer_in);
  } else if (n == 0) {
    handle_close();
  } else {
    errno = ret_errno;
    handle_error();
  }
}

void TcpConnection::handle_write()
{
  if (true) {
    ssize_t n = ::write(m_channel->fd(),
                        m_buffer_out.peek(),
                        m_buffer_out.readableBytes());
    if (n > 0) {
      m_buffer_out.retrieve(n);
      if (m_buffer_out.readableBytes() == 0) {
        m_channel->disable_write();
        if (m_write_complete_cb) {
          m_loop->queue_functor(
              std::bind(m_write_complete_cb, shared_from_this()));
        }
        // TODO
      } else {
        // "I am going to write more data"
      }
    } else {
      // "TcpConnection::handleWrite"
    }
  } else {
    // "Connection is down, no more writing"
  }
}

void TcpConnection::handle_close()
{
  m_channel->disable_all();

  if (m_close_cb) m_close_cb(shared_from_this());
}

void TcpConnection::handle_error()
{
  // TODO
}

void TcpConnection::send(const std::string& message)
{
  m_loop->do_functor(std::bind(&TcpConnection::send_worker, this, message));
}

void TcpConnection::send_worker(const std::string& message)
{
  ssize_t nwrote = 0;
  // if no thing in output queue, try writing directly
  if (!m_channel->is_writing() && m_buffer_out.readableBytes() == 0) {
    nwrote = ::write(m_channel->fd(), message.data(), message.size());
    if (nwrote >= 0) {
      if ((size_t)(nwrote) < message.size()) {
        // TODO
      } else if (m_write_complete_cb) {
        m_loop->queue_functor(
            std::bind(m_write_complete_cb, shared_from_this()));
      }
    } else {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        // TODO
      }
    }
  }

  assert(nwrote >= 0);
  if ((size_t)(nwrote) < message.size()) {
    m_buffer_out.append(message.data()+nwrote, message.size()-nwrote);
    if (!m_channel->is_writing()) {
      m_channel->enable_write();
    }
  }
}

void TcpConnection::set_read_complete_callback(ReadCompleteCallback cb)
{
  m_read_complete_cb = cb;
}

void TcpConnection::set_write_complete_callback(WriteCompleteCallback cb)
{
  m_write_complete_cb = cb;
}

void TcpConnection::set_close_callback(CloseCallback cb)
{
  m_close_cb = cb;
}

void TcpConnection::set_connected_callback(ConnectedCallback cb)
{
  m_conn_cb = cb;
}