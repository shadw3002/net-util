#include "EventLoop.h"
#include "TcpServer.h"

int main()
{
  EventLoop loop;
  TcpServer server(&loop, "127.0.0.1", 66366);

  server.set_connected_callback([](auto conn){

  });

  server.set_read_callback([](auto conn, auto buffer){
    auto str = buffer->retrieveAsString();
    conn->send(str);
  });

  server.start();
  loop.loop();
}