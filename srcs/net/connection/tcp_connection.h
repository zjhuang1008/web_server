#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <functional>
#include <string>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"
#include "srcs/net/connection/buffer.h"
#include "srcs/net/address/socket_address.h"

#include <boost/any.hpp>

namespace net {

class TCPConnection : private Uncopyable,
                      public std::enable_shared_from_this<TCPConnection> {
public:
  using CreateCallback = std::function<void(const TCPConnectionPtr&)>;
  using CloseCallback = std::function<void(const TCPConnectionPtr&)>;
  using ReadCallback = std::function<void(Buffer&, const TCPConnectionPtr&)>;

  TCPConnection(EventLoopPtr io_loop, 
                FDHandler accept_fd, 
                const SocketAddress& host_addr,
                const SocketAddress& peer_addr,
                std::string name);

  std::string name() { return name_; }

  const SocketAddress& host_addr() const { return host_addr_; }
  const SocketAddress& peer_addr() const { return peer_addr_; }

  void handleCreate();
  void handleClose();
  void handleError();
  void handleRead();
  
  void setCreateCallback(CreateCallback cb) { createCallback_ = std::move(cb); }
  void setCloseCallback(CloseCallback cb) { closeCallback_ = std::move(cb); }
  void setReadCallback(ReadCallback cb) { readCallback_ = std::move(cb); }

  void setContext(const boost::any& context) { context_ = context; }
  const boost::any& getContext() { return context_; }
  boost::any* getMutableContext() { return &context_; }
private:
  EventLoopPtr io_loop_;
  ChannelPtr channel_;
  SocketAddress host_addr_;
  SocketAddress peer_addr_;
  std::string name_;

  Buffer in_buffer_;
  Buffer out_buffer_;

  CreateCallback createCallback_;
  CloseCallback closeCallback_;
  ReadCallback readCallback_;

  boost::any context_;
};

} // namespace net

#endif // CONNECTION_H
