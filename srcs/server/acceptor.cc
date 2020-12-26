#include "srcs/server/acceptor.h"

#include <memory>

#include "srcs/net/thread/event_loop.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/server/socket_address.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/channel/channel.h"

using namespace net;

Acceptor::Acceptor(SocketAddress socket_address, int domain, int type) 
  : loop_(std::make_shared<EventLoop>()),
    sockfd_(sysw::socket(domain, type, 0)),
    sockch_(std::make_shared<Channel>(sockfd_)),
    addr_(std::move(socket_address)) {
  sysw::bind(sockfd_, addr_.sockaddr_ptr(), addr_.socklen());
  
  sockch_->setLoop(loop_);
}

void Acceptor::listen() {
  sysw::listen(sockfd_, SOMAXCONN);
  sockch_->enableReading();
  loop_->runInLoop(sockch_->registerCallback());
}

Acceptor::ChannelPtr Acceptor::accept() {
  struct sockaddr peer_addr;
  socklen_t peer_addr_len;
  int accept_fd = sysw::accept4(sockfd_, &peer_addr, &peer_addr_len);
  return std::make_shared<Channel>(FDHandler(accept_fd));
}
