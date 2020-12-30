#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"
#include "srcs/net/connection/buffer.h"

namespace net {

class TCPConnection : private Uncopyable,
                      public std::enable_shared_from_this<TCPConnection> {
public:
  TCPConnection(EventLoopPtr io_loop, ChannelPtr channel);

  void handleRead();
  void handleClose();

  void connectionEstablished();
private:
  EventLoopPtr io_loop_;
  ChannelPtr channel_;
  Buffer buffer_;
};

} // namespace net

#endif // CONNECTION_H
