#include "srcs/net/server/acceptor.h"

#include <memory>

#include "srcs/net/thread/event_loop.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/address/socket_address.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/channel/channel.h"
#include "srcs/logger/logger.h"

using namespace net;

Acceptor::Acceptor(EventLoopPtr& loop, int domain, int type, int protocol)
  : loop_(loop),
    sockfd_(sysw::socket(domain, type, protocol)),
    sockch_(std::make_shared<Channel>(loop_, sockfd_)) {
  // TODO make it user-define
  setReuseAddress();
}

void Acceptor::setReadCallback(Callback cb) {
  sockch_->setReadCallback(std::move(cb));
}

void Acceptor::listen(const SocketAddress& host_addr) {
  sysw::bind(sockfd_, host_addr.sockaddr_ptr(), host_addr.socklen());  
  sysw::listen(sockfd_, SOMAXCONN);
  std::string host_name = host_addr.toIPPort();

  loop_->runInLoop([this, host_name](){
    sockch_->enableReading();
    loop_->addChannelInPoller(sockch_);
    LOG(INFO) << "acceptor start to listen " << host_name;
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
//  LOG(ERROR) << "accept_fd: " << accept_fd;
  if (accept_fd < 0) {
    return accept_fd;
  }

  peer_addr.set_sockaddr_in6(peer_sockaddr);

  return accept_fd;
}

void Acceptor::setReuseAddress() {
  int optval;
  sysw::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}
