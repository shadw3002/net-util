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

  static void nonblock(int sockfd);

  static void nonagle(int sockfd, int optval = 1);

  static std::tuple<std::string, uint16_t> show_sockaddr_in(const sockaddr_in& addr);

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

void NetUtil::nonblock(int sockfd)
{
	int ret = fcntl(sockfd, F_GETFL);
	ret = ret | O_NONBLOCK;
	if (fcntl(sockfd, F_SETFL, ret) == -1) {
    // TODO
  }
}

void NetUtil::nonagle(int sockfd, int optval)
{
  int ret = setsockopt(
    sockfd,
    IPPROTO_TCP,
    TCP_NODELAY,
    &optval,
    static_cast<socklen_t>(sizeof(optval))
  );

	if (ret != 0) {
    // TODO
  }
}

std::tuple<std::string, uint16_t>
NetUtil::show_sockaddr_in(const sockaddr_in& addr)
{
  char ip_c_str[INET_ADDRSTRLEN];

  inet_ntop( // TODO
    AF_INET,
    &addr.sin_addr.s_addr,
    ip_c_str,
    sizeof(ip_c_str)
  );

  uint16_t port = ntohs(addr.sin_port);

  return {std::string(ip_c_str), port};
}