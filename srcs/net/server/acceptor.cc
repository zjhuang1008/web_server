#include "srcs/net/server/acceptor.h"

#include <memory>

#include "srcs/net/thread/event_loop.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/address/socket_address.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/channel/channel.h"

using namespace net;

Acceptor::Acceptor(SocketAddress host_address, int domain, int type, int protocol) 
  : loop_(std::make_shared<EventLoop>()),
    sockfd_(sysw::socket(domain, type, protocol)),
    sockch_(std::make_shared<Channel>(loop_, sockfd_)),
    host_addr_(std::move(host_address)) {
  sysw::bind(sockfd_, host_addr_.sockaddr_ptr(), host_addr_.socklen());  
}

void Acceptor::setListenCallback(Callback cb) {
  sockch_->setReadCallback(std::move(cb));
}

void Acceptor::listen() {
  sysw::listen(sockfd_, SOMAXCONN);

  loop_->runInLoop([this](){
    sockch_->enableReading();
    loop_->addChannelInPoller(sockch_);
  });
}

FDHandler Acceptor::accept(SocketAddress& peer_addr) {
  struct sockaddr_in6 peer_sockaddr;
  socklen_t peer_sockaddr_len = static_cast<socklen_t>(sizeof(peer_sockaddr));
  FDHandler accept_fd = FDHandler(sysw::accept4(sockfd_, 
                                  reinterpret_cast<struct sockaddr*>(&peer_sockaddr), 
                                  &peer_sockaddr_len, 
                                  SOCK_NONBLOCK | SOCK_CLOEXEC));
  peer_addr.set_sockaddr_in6(peer_sockaddr);

  return accept_fd;
}
