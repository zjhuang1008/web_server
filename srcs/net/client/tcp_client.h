#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"
#include "srcs/net/thread/event_loop_thread_pool.h"
#include "srcs/net/address/socket_address.h"

namespace net {

class TCPClient : private Uncopyable {
public:
  explicit TCPClient(SocketAddress host_addr);

  void setConnectionCreateCallback();
private:
  SocketAddress host_addr_;
};

} // namespace net

#endif // TCP_CLIENT_H
