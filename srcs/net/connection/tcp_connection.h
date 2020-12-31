#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <functional>

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
  void handleError();

  void connectionEstablished();

  void setBufferReadingFunction_(BufferReadingFunction fn) { 
    bufferReadingFunction_ = std::move(fn); 
  }

private:
  EventLoopPtr io_loop_;
  ChannelPtr channel_;
  Buffer buffer_;

  BufferReadingFunction bufferReadingFunction_;
  // static void defaultBufferReadingFunction(Buffer& buffer);
};

} // namespace net

#endif // CONNECTION_H
