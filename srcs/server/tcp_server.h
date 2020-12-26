#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <memory>

#include "srcs/utils/uncopyable.h"
#include "srcs/server/socket_address.h"
#include "srcs/net/thread/event_loop_thread_pool.h"
#include "srcs/server/acceptor.h"

namespace net {

class TCPServer : private Uncopyable {
public:
  TCPServer(size_t num_io_threads, SocketAddress socket_address);
  ~TCPServer();

  void start();
private:
  void socketReadCallback();

  EventLoopThreadPool io_thread_pool_;
  Acceptor acceptor_;
};

} // namespace net

#endif // TCP_SERVER_H
