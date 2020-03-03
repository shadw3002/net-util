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

  const char* bytes() const;
private:
  std::map<std::string, std::string> m_headers;
  StatusCode m_state_code;
  std::string m_status_message;
  bool m_keep_alive;
  std::string m_body;
};