#include "srcs/server/socket_address.h"

#include <string.h>
#include <stddef.h>
#include <netinet/in.h>

using namespace net;

SocketAddress::SocketAddress(uint16_t port, bool loopbackOnly) {
  static_assert(offsetof(SocketAddress, addr_) == 0, "addr_ offset 0");
  
  memset(&addr_, 0, sizeof addr_);
  addr_.sin_family = AF_INET;
  
  in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
  addr_.sin_addr.s_addr = htonl(ip);

  addr_.sin_port = htons(port);
}
