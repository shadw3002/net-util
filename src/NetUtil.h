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

class NetUtil
{
public:
  static int listen(const char* server_ip, const uint16_t port);

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