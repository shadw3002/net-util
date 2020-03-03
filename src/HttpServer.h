#pragma once

#include "TcpServer.h"
#include "boost/noncopyable.hpp"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <functional>


class HttpServer : boost::noncopyable
{
public:
  using HttpCallback = std::function<void(const HttpRequest&, HttpResponse&)>;

  HttpServer(const char* server_ip, uint16_t port);

  ~HttpServer();

  void start();
private:

  void message_handle(TcpConnection::Ptr, Buffer*);
  void write_handle(TcpConnection::Ptr);
  void connected_handle(TcpConnection::Ptr);
  void request_handle(TcpConnection::Ptr, const HttpRequest&);



  EventLoop* m_loop;

  TcpServer m_tcpserver;

  HttpCallback m_http_callback;
};

HttpServer::HttpServer(const char* server_ip, uint16_t port)
  : m_loop(new EventLoop())
  , m_tcpserver(m_loop, server_ip, port)
{
  using namespace std::placeholders;

  m_tcpserver.set_read_callback(std::bind(&HttpServer::message_handle, this, _1, _2));
  m_tcpserver.set_write_callback(std::bind(&HttpServer::write_handle, this, _1));
  m_tcpserver.set_connected_callback(std::bind(&HttpServer::connected_handle, this, _1));

}

HttpServer::~HttpServer()
{
  delete m_loop;
}

void HttpServer::start()
{
  m_tcpserver.start();
  m_loop->loop();
}

void HttpServer::message_handle(TcpConnection::Ptr conn, Buffer* buffer)
{
  HttpContext* context = std::any_cast<HttpContext>(conn->context());

  if (!context->parse(buffer)) {
    conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
    conn->shutdown();
  }

  if (context->ready()) {
    request_handle(conn, context->request());
    context->reset();
  }
}

void HttpServer::write_handle(TcpConnection::Ptr conn)
{

}

void HttpServer::connected_handle(TcpConnection::Ptr conn)
{
  conn->set_context(HttpContext());
}

void HttpServer::request_handle(TcpConnection::Ptr conn, const HttpRequest& request)
{
  HttpResponse response;
  m_http_callback(request, response);
  conn->send(response.bytes());
}