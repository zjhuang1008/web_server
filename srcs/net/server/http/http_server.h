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
  using ResponseCallback = std::function<void(const HTTPRequest&, HTTPResponse&)>;
  HTTPServer(EventLoopPtr loop, size_t num_io_threads, SocketAddress host_address);

//  void setResponseCallback(ResponseCallback cb) { responseCallback_ = std::move(cb); };
  void setResponseCallback(const std::string& path, ResponseCallback cb) {
    responseCallbacks_[path] = std::move(cb);
  }

  void start() { server_.start(); }
private:
  TCPServer server_;

//  ResponseCallback responseCallback_;
  std::unordered_map<std::string, ResponseCallback> responseCallbacks_;

  void connectionOnCreate(const TCPConnectionPtr& conn);
  void connectionOnRead(Buffer& in_buffer, const TCPConnectionPtr& conn);
};

} // namespace net

#endif // HTTP_SERVER_H
