#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <functional>
#include <string>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"
#include "srcs/net/connection/buffer/buffer_factory.h"
#include "srcs/net/address/socket_address.h"

#include <boost/any.hpp>
#include <srcs/net/fd_handler/fd_handler.h>

namespace net {

template<typename BufferType>
class TCPConnection : private Uncopyable,
                      public std::enable_shared_from_this<TCPConnection<BufferType>> {
public:
  enum class Status {
    kConnecting, kConnected, kDisconnecting, kDisconnected
  };

  using CreateCallback = std::function<void(const TCPConnectionPtr<BufferType>&)>;
  using CloseCallback = std::function<void(const TCPConnectionPtr<BufferType>&)>;
  using ReadCallback = std::function<void(BufferType&, const TCPConnectionPtr<BufferType>&)>;
  using SendCallback = std::function<void(BufferType&)>;

  TCPConnection(EventLoopPtr& io_loop,
                FDHandler socket_fd,
                const SocketAddress& host_addr,
                const SocketAddress& peer_addr,
                std::string name);

  const std::string& name() { return name_; }

  const SocketAddress& host_addr() const { return host_addr_; }
  const SocketAddress& peer_addr() const { return peer_addr_; }

  void handleCreate();
  void handleClose();
  void handleError();
  void handleRead();
  void handleWrite();
  
  void setCreateCallback(CreateCallback cb) { createCallback_ = std::move(cb); }
  void setCloseCallback(CloseCallback cb) { closeCallback_ = std::move(cb); }
  void setReadCallback(ReadCallback cb) { readCallback_ = std::move(cb); }

  void setContext(const boost::any& context) { context_ = context; }
  const boost::any& getContext() { return context_; }
  boost::any* getMutableContext() { return &context_; }

  void send(SendCallback cb);
//  void sendInLoop(Buffer buffer);

  void shutdown();
private:
  EventLoopPtr& io_loop_;
  ChannelPtr channel_;
  SocketAddress host_addr_;
  SocketAddress peer_addr_;
  std::string name_;
  Status status_;

  BufferType in_buffer_;
  BufferType out_buffer_;

  CreateCallback createCallback_;
  CloseCallback closeCallback_;
  ReadCallback readCallback_;

//  Callback writeCompleteCallback_;

  boost::any context_;

  inline void assertInLoop();
};

} // namespace net

#endif // CONNECTION_H
