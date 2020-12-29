#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"

namespace net {

class TCPConnection : private Uncopyable,
                      public std::enable_shared_from_this<TCPConnection> {
public:
  // using ChannelPtr = std::shared_ptr<Channel>;
  TCPConnection(EventLoopPtr io_loop, ChannelPtr channel);

  void connectionEstablished();

private:
  EventLoopPtr io_loop_;
  ChannelPtr channel_;

  Callback readCallback_;

};

} // namespace net

#endif // CONNECTION_H
