#pragma once

#include "Buffer.h"
#include "HttpRequest.h"

class HttpContext
{
public:

  enum State
  {
    ExpectRequestLine,
    ExpectHeaders,
    ExpectBody,
    Done
  };

  bool parse(Buffer* buffer);

  bool parse_request_line(const char* begin, const char* end);

  bool ready() const;

  void reset();

  const HttpRequest& request() const;

  HttpRequest& request();

private:
  State m_state;

  HttpRequest m_request;
};

#include <string.h>

bool HttpContext::parse(Buffer* buffer)
{
  const char CRLF[] = {'\r', '\n'};

  bool loop_control;
  do {
    loop_control = false;

    printf("context state: %d\n", m_state);

    switch (m_state){
      case ExpectRequestLine: {
        const char* crlf = (char*)memmem(
          buffer->peek(),
          buffer->readableBytes(),
          CRLF,
          sizeof(CRLF) / sizeof(char)
        );

        if (crlf) {
          auto ret = parse_request_line(buffer->peek(), crlf);
          if (ret) {
            buffer->retrieveUntil(crlf + 2);
            m_state = ExpectHeaders;
            loop_control = true;
          } else {
            return false;
          }
        }

        break;
      }
      case ExpectHeaders: {
        auto crlf = (const char*)memmem(
          buffer->peek(),
          buffer->readableBytes(),
          CRLF,
          sizeof(CRLF) / sizeof(char)
        );

        if (crlf) {
          auto colon = std::find(buffer->peek(), crlf, ':');
          if (colon != crlf) {
            auto start = buffer->peek(), end = crlf;
            std::string field(start, colon);

            colon++;
            while (colon < end && isspace(*colon)) ++colon;
            while (end - colon && isspace(*(end-1))) --end;

            std::string value(colon, end);

            m_request.add_header(field, value);

            loop_control = true;
          } else { // 空行
            m_state = Done;
          }

          buffer->retrieveUntil(crlf + 2);
        } else {

        }

        break;
      }
      case ExpectBody: {
        // TODO
        break;
      }
      case Done: {
        break;
      }
      default: {
        break;
      }
    }
  } while (loop_control);

  return true;
}

HttpRequest::Method method_from_str(const char* begin, const char* end)
{
  std::string method(begin, end);

  if (method == "GET") return HttpRequest::Method::Get;
  else if (method == "POST") return HttpRequest::Method::Post;
  else if (method == "HEAD") return HttpRequest::Method::Head;
  else if (method == "PUT") return HttpRequest::Method::Put;
  else if (method == "DELETE") return HttpRequest::Method::Delete;
  else return HttpRequest::Method::Invalid;
}

bool HttpContext::parse_request_line(const char* begin, const char* end)
{
  // 方法 URI 协议版本

  auto space = std::find(begin, end, ' ');
  if (space == end) return false;

  // Method
  auto method = method_from_str(begin, space);
  if (method == HttpRequest::Method::Invalid) return false;
  m_request.set_method(method);

  // URI
  begin = space + 1;
  space = std::find(begin, end, ' ');
  if (space == end) return false;
  auto question_mark = std::find(begin, space, '?');
  if (question_mark != space) {
    m_request.set_path(begin, question_mark);
    m_request.set_query(question_mark, space);
  } else {
    m_request.set_path(begin, space);
  }

  // Version
  begin = space + 1;
  if (end - begin != 8 || !std::equal(begin, end - 1, "HTTP/1.")) return false;

  char ch_end = *(end-1);
  if (ch_end == '1') m_request.set_version(HttpRequest::Version::Http11);
  else if (ch_end == '0') m_request.set_version(HttpRequest::Version::Http10);
  else return false;

  return true;
}

bool HttpContext::ready() const
{
  return m_state == State::Done;
}

void HttpContext::reset()
{
  m_state = State::ExpectRequestLine;
  HttpRequest dummy;
  m_request.swap(dummy);
}

const HttpRequest& HttpContext::request() const
{
  return m_request;
}

HttpRequest& HttpContext::request()
{
  return m_request;
}