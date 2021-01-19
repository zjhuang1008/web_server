#include "tcp_client.h"

#include "srcs/net/thread/event_loop.h"

using namespace net;

TCPClient::TCPClient(SocketAddress host_addr)
  : host_addr_(host_addr) {

}

