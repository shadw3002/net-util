#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string>
#include <tuple>
#include <bits/socket.h>

class NetUtil
{
public:
  static int listen(const char* server_ip, const uint16_t port);

  static std::tuple<int, sockaddr_in> accept(int server_sockfd);

private:
  static struct sockaddr_in get_sockaddr_in(const char* server_ip, const uint16_t port);

};

struct sockaddr_in NetUtil::get_sockaddr_in(const char* server_ip, const uint16_t port)
{
  struct sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(port); // 主机字节序到网络字节序
  if (inet_pton(AF_INET, server_ip, &sockaddr.sin_addr) != 1) {
    // TODO
  }

  return sockaddr;
}

int NetUtil::listen(const char* server_ip, const uint16_t port)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    // TODO
  }

  // TODONOW

  struct sockaddr_in server_addr = get_sockaddr_in(server_ip, port);

  int ret = connect(
    fd,
    reinterpret_cast<struct sockaddr*>(&server_addr),
    static_cast<socklen_t>(sizeof(server_addr))
  );
  if (ret == -1) {
    // TODO
  }

  return fd;
}

std::tuple<int, sockaddr_in>
NetUtil::accept(int server_sockfd)
{
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  socklen_t addrlen = sizeof(addr);

  int conn_fd = accept4(
    server_sockfd,
    (struct sockaddr*)&addr,
    &addrlen,
    SOCK_NONBLOCK | SOCK_CLOEXEC
  );

  if (conn_fd < 0) {
    // TODO
  }

  return {conn_fd, addr};
}