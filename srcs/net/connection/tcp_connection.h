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
  using ReadCallback = std::function<void(Buffer&)>;

  TCPConnection(EventLoopPtr io_loop, FDHandler accept_fd, std::string name);

  std::string name() { return name_; }

  void handleCreate();
  void handleClose();
  void handleError();
  void handleRead();
  
  void setCloseCallback(CloseCallback cb) { closeCallback_ = std::move(cb); }
  void setReadCallback(ReadCallback cb) { readCallback_ = std::move(cb); }
private:
  EventLoopPtr io_loop_;
  ChannelPtr channel_;
  std::string name_;

  Buffer in_buffer_;
  Buffer out_buffer_;

  CloseCallback closeCallback_;
  ReadCallback readCallback_;

};

} // namespace net

#endif // CONNECTION_H
