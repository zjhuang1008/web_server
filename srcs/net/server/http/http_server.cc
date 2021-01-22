#include "srcs/net/server/http/http_server.h"

#include <boost/any.hpp>

#include "srcs/net/connection/buffer.h"
#include "srcs/net/connection/tcp_connection.h"
#include "srcs/net/server/http/http_request.h"
#include "srcs/net/server/http/http_context.h"
#include "srcs/net/server/http/http_response.h"
#include "srcs/logger/logger.h"

namespace net {

static void badRequestHttpCallback(const HTTPRequest&, HTTPResponse& resp) {
  resp.setStatusCode(HttpStatusCode::k400BadRequest);
  resp.setCloseConnection(true);
}

static void notFoundHttpCallback(const HTTPRequest&, HTTPResponse& resp) {
  resp.setStatusCode(HttpStatusCode::k404NotFound);
  resp.setCloseConnection(true);
}

}; // namespace net

using namespace net;

HTTPServer::HTTPServer(EventLoopPtr& loop, size_t num_io_threads, SocketAddress host_address)
  : server_(loop, num_io_threads, host_address) {
  server_.setConnectionCreateCallback([this](const TCPConnectionPtr& conn) {
    this->connectionOnCreate(conn);
  });
  server_.setConnectionReadCallback([this](Buffer& in_buffer, const TCPConnectionPtr& conn) {
    this->connectionOnRead(in_buffer, conn);
  });
}

void HTTPServer::connectionOnCreate(const TCPConnectionPtr& conn) {
  conn->setContext(HTTPContext());
}

void HTTPServer::connectionOnRead(Buffer& in_buffer, const TCPConnectionPtr& conn) {
  HTTPResponse response;
  HTTPContext* http_context = boost::any_cast<HTTPContext>(conn->getMutableContext());

  const HTTPRequest& request = http_context->request();

  bool to_send = false;
  if (!http_context->parseRequest(in_buffer)) {
    // bad request
    to_send = true;

    net::badRequestHttpCallback(request, response);
  }

  if (http_context->parseFinished()) {
    http_context->resetState();
    to_send = true;

    const std::string& path = request.getPath();
    LOG(DEBUG) << "connection " << conn->name() << " receive " << path;

    if (responseCallbacks_.count(path)) {
      // request can be handled
      responseCallbacks_[path](request, response);

      // decide whether to close the connection
      if (request.hasHeader("Connection")) {
        const std::string& conn_value = request.getHeader("Connection");
        bool to_close = conn_value == "close" ||
                        (request.getVersion() == HttpVersion::kHttp10 && conn_value == "Keep-Alive");
        response.setCloseConnection(to_close);
      }
    } else {
      // not found
      net::notFoundHttpCallback(request, response);
    }
  }

  if (to_send) {
    conn->send(response.toBuffer());

    if (response.getCloseConnection()) {
      conn->shutdown();
    }
  }
}