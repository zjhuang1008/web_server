#include "srcs/net/server/acceptor.h"

#include <memory>

#include "srcs/net/thread/event_loop.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/address/socket_address.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/channel/channel.h"

using namespace net;

Acceptor::Acceptor(EventLoopPtr loop, int domain, int type, int protocol) 
  : loop_(std::move(loop)),
    sockfd_(sysw::socket(domain, type, protocol)),
    sockch_(std::make_shared<Channel>(loop_, sockfd_)) {
  
}

void Acceptor::setReadCallback(Callback cb) {
  sockch_->setReadCallback(std::move(cb));
}

void Acceptor::listen(const SocketAddress& host_addr) {
  sysw::bind(sockfd_, host_addr.sockaddr_ptr(), host_addr.socklen());  
  sysw::listen(sockfd_, SOMAXCONN);

  loop_->runInLoop([this](){
    sockch_->enableReading();
    loop_->addChannelInPoller(sockch_);
  });
}

FDHandler Acceptor::accept(SocketAddress& peer_addr) {
  struct sockaddr_in6 peer_sockaddr;
  socklen_t peer_sockaddr_len = static_cast<socklen_t>(sizeof(peer_sockaddr));
  FDHandler accept_fd = FDHandler(
  sysw::accept4(
        sockfd_,
        reinterpret_cast<struct sockaddr*>(&peer_sockaddr),
        &peer_sockaddr_len,
        SOCK_NONBLOCK | SOCK_CLOEXEC
      )
  );
  peer_addr.set_sockaddr_in6(peer_sockaddr);

  return accept_fd;
}
