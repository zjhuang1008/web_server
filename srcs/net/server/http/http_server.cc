#include "srcs/net/server/http/http_server.h"

#include <boost/any.hpp>

#include "srcs/net/connection/buffer.h"
#include "srcs/net/server/http/http_request.h"
#include "srcs/net/server/http/http_context.h"
#include "srcs/net/server/http/http_response.h"

using namespace net;

HTTPServer::HTTPServer(EventLoopPtr loop, size_t num_io_threads, SocketAddress host_address) 
  : server_(std::move(loop), num_io_threads, host_address) {
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
  HTTPContext* http_context = boost::any_cast<HTTPContext>(conn->getMutableContext());
  
  if (!http_context->parseRequest(in_buffer)) {
    // send
  }

  if (http_context->parseFinished()) {
    HTTPResponse response;
    responseCallback_(http_context->request(), response);

    // send
  }
}