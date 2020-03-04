#pragma once

#include <map>
#include <string>


class HttpResponse
{
public:
  enum StatusCode {
    Unknown,
    Code_200 = 200,
    Code_301 = 301,
    Code_400 = 400,
    Code_404 = 404
  };

  HttpResponse();

  void set_status_code(StatusCode code);

  StatusCode status_code() const;

  void set_status_message(const std::string& message);

  void set_keep_alive(bool close);

  bool is_close_connection() const;

  void set_content_type(const std::string& content_type);

  void add_headers(const std::string& key, const std::string& value);

  void set_body(const std::string& body);

  void append_to_buffer(Buffer* buffer) const;

  std::string bytes() const;
private:
  std::map<std::string, std::string> m_headers;

  StatusCode m_status_code;

  std::string m_status_message;

  bool m_keep_alive;

  std::string m_body;
};

#include "Buffer.h"

HttpResponse::HttpResponse()
{

}

void HttpResponse::set_status_code(StatusCode code)
{
  m_status_code = code;
}

HttpResponse::StatusCode HttpResponse::status_code() const
{
  return m_status_code;
}

void HttpResponse::set_status_message(const std::string& message)
{
  m_status_message = message;
}

void HttpResponse::set_keep_alive(bool keep)
{
  m_keep_alive = keep;
}

bool HttpResponse::is_close_connection() const
{
  return !m_keep_alive;
}

void HttpResponse::set_content_type(const std::string& content_type)
{
  add_headers("Content-Type", content_type);
}

void HttpResponse::add_headers(const std::string& key, const std::string& value)
{
  m_headers[key] = value;
}

void HttpResponse::set_body(const std::string& body)
{
  m_body = body;
}

void HttpResponse::append_to_buffer(Buffer* buffer) const
{
  char buf[32];

  snprintf(buf, sizeof buf, "HTTP/1.1 %d ", m_status_code);
  buffer->append(buf);
  buffer->append(m_status_message);
  buffer->append("\r\n");

  if (!m_keep_alive) {
    buffer->append("Connection: close\r\n");
  } else {
    snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", m_body.size());
    buffer->append(buf);
    buffer->append("Connection: Keep-Alive\r\n");
  }

  for (auto& it : m_headers) {
    buffer->append(it.first);
    buffer->append(":");
    buffer->append(it.second);
    buffer->append("\r\n");
  }

  buffer->append("\r\n");
  buffer->append(m_body);
}

std::string HttpResponse::bytes() const
{
  std::string bytes;

  char buf[32];
  snprintf(buf, sizeof buf, "HTTP/1.1 %d ", m_status_code);

  bytes += buf + m_status_message + "\r\n";

  if (!m_keep_alive) {
    bytes += "Connection: close\r\n";
  } else {
    snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", m_body.size());
    bytes += std::string(buf) + "Connection: Keep-Alive\r\n";
  }

  for (auto& it : m_headers) {
    bytes += it.first + ":" + it.second + "\r\n";
  }

  bytes += "\r\n" + m_body;

  return bytes;
}