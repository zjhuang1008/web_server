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
  TCPServer(EventLoopPtr& loop, size_t num_io_threads, SocketAddress host_addr);
//  ~TCPServer();

  void start();

  void setConnectionCreateCallback(TCPConnection::CreateCallback cb) { createCallback_ = std::move(cb); }
  void setConnectionReadCallback(TCPConnection::ReadCallback cb) { readCallback_ = std::move(cb); }
private:
  EventLoopPtr loop_;
  EventLoopThreadPool io_thread_pool_;
  Acceptor acceptor_;
  SocketAddress host_addr_;
  std::string name_;

  void acceptorReadCallback();

  TCPConnection::CreateCallback createCallback_;
  void connectionCloseCallback(const TCPConnectionPtr &conn);
  TCPConnection::ReadCallback readCallback_;
  
  std::unordered_map<std::string, TCPConnectionPtr> connections_;

  int next_conn_id_;
//  size_t maximum_conn_num_;
};

} // namespace net

#endif // TCP_SERVER_H
