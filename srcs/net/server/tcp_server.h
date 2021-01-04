#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <memory>
#include <unordered_map>
#include <string>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/address/socket_address.h"
#include "srcs/net/thread/event_loop_thread_pool.h"
#include "srcs/net/server/acceptor.h"
#include "srcs/net/types.h"

namespace net {

class TCPServer : private Uncopyable {
public:
  TCPServer(EventLoopPtr loop, size_t num_io_threads, SocketAddress host_address);
  ~TCPServer();

  void start();

  void setConnectionReadCallback(Callback cb) { connection_cb_.read = cb; }
  void setConnectionWriteCallback(Callback cb) { connection_cb_.write = cb; }
private:
  void listenCallback();

  void connectionCloseCallback(const TCPConnectionPtr &conn);

  struct connectionCallbacks {
    Callback read;
    Callback write;
  } connection_cb_;

  EventLoopThreadPool io_thread_pool_;
  Acceptor acceptor_;

  std::unordered_map<std::string, TCPConnectionPtr> connections_;
  
  BufferReadingFunction bufferReadingFunction_;
};

} // namespace net

#endif // TCP_SERVER_H
