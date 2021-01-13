#include <utility>

#include "srcs/net/server/http/http_server.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/net/address/socket_address.h"
#include "srcs/net/server/http/http_request.h"
#include "srcs/net/server/http/http_response.h"
#include "srcs/logger/logger.h"

using namespace net;

int main(int argc, char* argv[]) {
  size_t num_io_threads = 4;
  uint16_t port = 45678;

  EventLoopPtr loop = std::make_shared<EventLoop>();
  SocketAddress address(port);

  HTTPServer server(loop, num_io_threads, address);

  server.setResponseCallback("/hello", [](const HTTPRequest& req, HTTPResponse& resp) {
    resp.setStatusCode(HttpStatusCode::k200Ok);
    resp.setHeader("Content-type", "text/plain");
    resp.setBody("Hello World\r\n");
  });

  server.start();

  loop->startLoop();
}
