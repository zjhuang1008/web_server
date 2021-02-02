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
#include "srcs/net/connection/buffer/linked_buffer.h"
#include "srcs/logger/logger.h"

namespace net {

template<typename BufferType>
static void defaultCreateCallback(const TCPConnectionPtr<BufferType>& conn) {
  LOG(INFO) << conn->peer_addr().toIPPort() << " -> "
            << conn->host_addr().toIPPort() << " established";
}

template<typename BufferType>
static void defaultReadCallback(BufferType& buffer, const TCPConnectionPtr<BufferType>& conn) {
  buffer.readAll();
}

template<typename BufferType>
class TCPServer : private Uncopyable {
public:
  TCPServer(EventLoopPtr& loop, size_t num_io_threads, SocketAddress host_addr);
//  ~TCPServer();

  void start() { acceptor_.listen(host_addr_); }

  void setConnectionCreateCallback(typename TCPConnection<BufferType>::CreateCallback cb) {
    createCallback_ = std::move(cb);
  }
  void setConnectionReadCallback(typename TCPConnection<BufferType>::ReadCallback cb) {
    readCallback_ = std::move(cb);
  }
private:
  EventLoopPtr loop_;
  EventLoopThreadPool io_thread_pool_;
  Acceptor acceptor_;
  SocketAddress host_addr_;
  std::string name_;

  typename TCPConnection<BufferType>::CreateCallback createCallback_;
  typename TCPConnection<BufferType>::ReadCallback readCallback_;

  void acceptorReadCallback();
  void connectionCloseCallback(const TCPConnectionPtr<BufferType> &conn);

  std::unordered_map<std::string, TCPConnectionPtr<BufferType>> connections_;

  int next_conn_id_;
//  size_t maximum_conn_num_;
};

} // namespace net

#endif // TCP_SERVER_H
