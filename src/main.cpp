#include "TcpServer.h"

int main()
{
  TcpServer server("127.0.0.1", 66366);
  server.start();
}