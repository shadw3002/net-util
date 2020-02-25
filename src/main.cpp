#include "EventLoop.h"
#include "TcpServer.h"

int main()
{
  EventLoop loop;
  TcpServer server(&loop, "127.0.0.1", 66366);
  server.start();
  loop.loop();
}