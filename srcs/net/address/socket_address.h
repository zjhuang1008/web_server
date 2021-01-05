#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H

#include "srcs/utils/copyable.h"

#include <netinet/in.h>
#include <string>

namespace net {

class SocketAddress : private Copyable {
public:
  SocketAddress() = default;
  SocketAddress(uint16_t port, bool loopbackOnly=false, bool ipv6=false);

  void set_sockaddr_in6(const struct sockaddr_in6& addr6) { addr6_ = addr6; }

  std::string toIP();
  uint16_t toPort();
  std::string toIPPort();

  struct sockaddr* sockaddr_ptr() { return reinterpret_cast<struct sockaddr*>(&addr6_); }
  socklen_t socklen() { return sizeof(addr6_); }
private:
  union {
    struct sockaddr_in addr_;
    struct sockaddr_in6 addr6_;
  };
};

} // namespace net

#endif // SOCKET_ADDRESS_H
