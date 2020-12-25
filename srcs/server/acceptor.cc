#include "srcs/server/acceptor.h"

#include <memory>

#include "srcs/net/thread/event_loop.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/server/socket_address.h"

using namespace net;

Acceptor::Acceptor(SocketAddress socket_address, int domain, int type) 
  : loop_(std::make_shared<EventLoop>()),
    sockfd_(sysw::socket(domain, type, 0)),
    addr_(std::move(socket_address)) {
  sysw::bind(sockfd_, addr_.sockaddr_ptr(), addr_.socklen());
  sysw::listen(sockfd_, SOMAXCONN);
  // add to epoll and start to loop
}