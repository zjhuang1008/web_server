#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H

#include "srcs/utils/copyable.h"

#include <netinet/in.h>

namespace net {

class SocketAddress : private Copyable {
public:
  SocketAddress(uint16_t port, bool loopbackOnly);

  struct sockaddr* sockaddr_ptr() { return reinterpret_cast<struct sockaddr*>(&addr_); }
  socklen_t socklen() { return sizeof(addr_); }
private:
  struct sockaddr_in addr_;
};

} // namespace net

#endif // SOCKET_ADDRESS_H
