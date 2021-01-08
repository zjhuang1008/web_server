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

  // TODO: use sockaddr_storage
  void set_sockaddr_in6(const struct sockaddr_in6& addr6) { addr6_ = addr6; }

  std::string toIP() const;
  uint16_t toPort() const;
  std::string toIPPort() const;

  const struct sockaddr* sockaddr_ptr() const { return reinterpret_cast<const struct sockaddr*>(&addr6_); }
  socklen_t socklen() const { return sizeof(addr6_); }
private:
  union {
    struct sockaddr_in addr_;
    struct sockaddr_in6 addr6_;
  };
};

} // namespace net

#endif // SOCKET_ADDRESS_H
