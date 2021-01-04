#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <memory>
#include <unordered_map>
#include <string>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/address/socket_address.h"
#include "srcs/net/thread/event_loop_thread_pool.h"
#include "srcs/net/server/acceptor.h"
#include "srcs/net/connection/tcp_connection.h"
#include "srcs/net/types.h"

namespace net {

class TCPServer : private Uncopyable {
public:
  TCPServer(EventLoopPtr loop, size_t num_io_threads, SocketAddress host_address);
  ~TCPServer();

  void start();

  void setConnectionReadCallback(TCPConnection::ReadCallback cb) { readCallback_ = std::move(cb); }
  void setConnectionWriteCallback(Callback cb) {  }
private:
  EventLoopThreadPool io_thread_pool_;
  Acceptor acceptor_;
  TCPConnection::ReadCallback readCallback_;

  void listenCallback();
  void connectionCloseCallback(const TCPConnectionPtr &conn);
  
  std::unordered_map<std::string, TCPConnectionPtr> connections_;
};

} // namespace net

#endif // TCP_SERVER_H
