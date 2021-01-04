#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <functional>
#include <string>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"
#include "srcs/net/connection/buffer.h"

namespace net {

class TCPConnection : private Uncopyable,
                      public std::enable_shared_from_this<TCPConnection> {
public:
  using CloseCallback = std::function<void(const TCPConnectionPtr&)>;

  TCPConnection(EventLoopPtr io_loop, FDHandler accept_fd, std::string name);

  std::string name() { return name_; }

  void handleCreate();
  void handleClose();
  void handleRead();
  void handleError();
  
  void setCloseCallback(CloseCallback cb) { closeCallback_ = std::move(cb); }

  void setBufferReadingFunction_(BufferReadingFunction fn) { 
    bufferReadingFunction_ = std::move(fn); 
  }
private:
  EventLoopPtr io_loop_;
  ChannelPtr channel_;
  Buffer buffer_;
  std::string name_;

  BufferReadingFunction bufferReadingFunction_;

  CloseCallback closeCallback_;
};

} // namespace net

#endif // CONNECTION_H
