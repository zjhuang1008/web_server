#include "srcs/net/address/socket_address.h"

#include <string>

#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "srcs/logger/logger.h"

static constexpr size_t g_buf_size = 64;
static_assert(g_buf_size >= INET6_ADDRSTRLEN);
static_assert(g_buf_size >= INET_ADDRSTRLEN);

using namespace net;

static void writeIPToBuffer(char* buf, const struct sockaddr* addr) {
  if (addr->sa_family == AF_INET6) {
    
    const struct sockaddr_in6* addr6 = reinterpret_cast<const struct sockaddr_in6*>(addr);
    inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(g_buf_size));
  } else if (addr->sa_family == AF_INET) {
    
    const struct sockaddr_in* addr4 = reinterpret_cast<const struct sockaddr_in*>(addr);
    inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(g_buf_size));
  } else {
    LOG(ERROR) << "got unsupported sa_familiy: " << addr->sa_family;
  }
}

SocketAddress::SocketAddress(uint16_t port, bool loopbackOnly, bool ipv6) {
  static_assert(offsetof(SocketAddress, addr6_) == 0, "addr6_ offset 0");
  static_assert(offsetof(SocketAddress, addr_) == 0, "addr_ offset 0");
  
  if (ipv6) {
    memset(&addr6_, 0, sizeof addr6_);
    addr6_.sin6_family = AF_INET6;

    in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
    addr6_.sin6_addr = ip;

    addr6_.sin6_port = htons(port);
  } else {
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    
    in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
    addr_.sin_addr.s_addr = htonl(ip);

    addr_.sin_port = htons(port);
  }
}

std::string SocketAddress::toIP() {
  char buf[g_buf_size];

  writeIPToBuffer(buf, sockaddr_ptr());
  return std::string(buf);
}

uint16_t SocketAddress::toPort() {
  return ntohs(addr_.sin_port);
}

std::string SocketAddress::toIPPort() {
  char buf[g_buf_size];

  writeIPToBuffer(buf, sockaddr_ptr());
  size_t end = ::strlen(buf);

  uint16_t port = toPort();
  snprintf(buf+end, g_buf_size - end, ":%u", port);
  return std::string(buf);
}
