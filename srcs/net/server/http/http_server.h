#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"
#include "srcs/net/server/tcp_server.h"

#include <unordered_map>
#include <string>
#include <functional>

namespace net {

class HTTPServer : private Uncopyable {
public:
  using HttpCallback = std::function<HTTPResponse(HTTPRequest)>;
  HTTPServer(EventLoopPtr loop, size_t num_io_threads, SocketAddress host_address);

  void setHttpCallback(HttpCallback );

private:
  TCPServer server_;
  
  void connectionOnCreate(const TCPConnectionPtr& conn);
  void connectionOnRead(Buffer& in_buffer, const TCPConnectionPtr& conn);
  // std::unordered_map<std::string, >
  
  // HTTPRequest request_;
  // HTTPResponse response_;
};

} // namespace net

#endif // HTTP_SERVER_H
