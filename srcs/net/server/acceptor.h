#ifndef SOCKET_H
#define SOCKET_H

#include <memory>
#include <sys/socket.h>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"
#include "srcs/net/address/socket_address.h"
#include "srcs/net/fd_handler/fd_handler.h"

namespace net {

class Channel;
class EventLoop;

class Acceptor : private Uncopyable {
public:
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  using ChannelPtr = std::shared_ptr<Channel>;

  Acceptor(SocketAddress socket_address, 
           int domain=AF_INET, 
           int type=SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
           int protocol=IPPROTO_TCP);
  void setListenCallback(Callback cb);

  void listen();
  FDHandler accept(SocketAddress& peer_addr);

  // int sockfd() { return sockfd_; }
private:
  EventLoopPtr loop_;
  FDHandler sockfd_;
  ChannelPtr sockch_;
  SocketAddress host_addr_;

  
};

} // namespace net

#endif // SOCKET_H
