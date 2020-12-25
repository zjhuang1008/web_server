#ifndef SOCKET_H
#define SOCKET_H

#include <socket.h>

#include "srcs/utils/uncopyable.h"
#include "srcs/server/socket_address.h"

namespace net {

class FDHandler;

class Acceptor : private Uncopyable {
public:
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  Acceptor(SocketAddress socket_address, int domain=AF_INET, int type=SOCK_STREAM);
private:
  EventLoopPtr loop_;
  FDHandler sockfd_;
  SocketAddress addr_;
};

} // namespace net

#endif // SOCKET_H
