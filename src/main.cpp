#include "EventLoop.h"
#include "HttpServer.h"
#include "HttpResponse.h"
#include "HttpRequest.h"

int main()
{
  HttpServer server("127.0.0.1", 55555);

  server.set_http_callback([](auto requesst, auto response){
    response->set_status_code(HttpResponse::StatusCode::Code_404);
    response->set_status_message("Not Found");
  });

  server.start();
}