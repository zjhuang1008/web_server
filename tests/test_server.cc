#include <utility>
#include <unistd.h>

#include "srcs/net/server/http/http_server.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/net/address/socket_address.h"
#include "srcs/net/server/http/http_request.h"
#include "srcs/net/server/http/http_response.h"
#include "srcs/net/connection/buffer/buffer_factory.h"

using namespace net;

int main(int argc, char* argv[]) {
  size_t num_io_threads = 4;
  uint16_t port = 45678;

  EventLoopPtr loop = std::make_shared<EventLoop>();
  SocketAddress address(port);

  HTTPServer<Buffer> server(loop, num_io_threads, address);

  server.setResponseCallback("/hello", [](const HTTPRequestPtr& req, HTTPResponsePtr& resp) {
    resp->setStatusCode(HttpStatusCode::k200Ok);
    resp->setHeader("Content-type", "text/plain");
    resp->setBody("Hello World\r\n");
  });
  server.setResponseCallback("/long", [](const HTTPRequestPtr& req, HTTPResponsePtr& resp) {
    resp->setStatusCode(HttpStatusCode::k200Ok);
    resp->setHeader("Content-type", "text/plain");
    resp->setBody(std::string(5000, 'a'));

    // sleep 20ms to simulate the delay of I/O or compute task.
    // usleep(20000);
  });
  server.setResponseCallback("/", [](const HTTPRequestPtr& req, HTTPResponsePtr& resp) {
    resp->setStatusCode(HttpStatusCode::k200Ok);
    resp->setHeader("Content-type", "text/plain");
    resp->setBody("Home\r\n");
  });

  server.start();

  loop->startLoop();
}
