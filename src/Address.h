#pragma once

#include <string>
#include <netinet/in.h>

class Address
{
public:
  explicit Address(uint16_t port);

  Address(const std::string& ip, uint16_t port);

  const struct sockaddr_in& as_sockaddr_in() const;

private:
  struct sockaddr_in m_addr;
};

Address::Address(uint16_t port)
{
  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = ;
  addr.sin_port = ;
}