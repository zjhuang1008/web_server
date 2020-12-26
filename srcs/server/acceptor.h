#ifndef SOCKET_H
#define SOCKET_H

#include <memory>
#include <sys/socket.h>

#include "srcs/utils/uncopyable.h"
#include "srcs/utils/types.h"
#include "srcs/server/socket_address.h"
#include "srcs/net/fd_handler/fd_handler.h"

namespace net {

class Channel;
class EventLoop;

class Acceptor : private Uncopyable {
public:
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  using ChannelPtr = std::shared_ptr<Channel>;

  Acceptor(SocketAddress socket_address, int domain=AF_INET, int type=SOCK_STREAM);
  void setSocketReadCallback(Callback cb) {
    sockch_->setReadCallback(std::move(cb));
  }

  void listen();
  ChannelPtr accept();

  // int sockfd() { return sockfd_; }
private:
  EventLoopPtr loop_;
  FDHandler sockfd_;
  ChannelPtr sockch_;
  SocketAddress addr_;

  
};

} // namespace net

#endif // SOCKET_H
