#include "srcs/net/server/tcp_server.h"

#include <memory>

#include "srcs/net/channel/channel.h"
#include "srcs/net/connection/tcp_connection.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/logger/logger.h"
#include "srcs/net/connection/buffer/linked_buffer.h"

using namespace net;

template<typename BufferType>
TCPServer<BufferType>::TCPServer(EventLoopPtr& loop, size_t num_io_threads, SocketAddress host_addr)
  : loop_(loop),
    io_thread_pool_(num_io_threads),
    acceptor_(loop),
    host_addr_(host_addr),
    name_(host_addr.toIPPort()),
    createCallback_(net::defaultCreateCallback<BufferType>),
    readCallback_(net::defaultReadCallback<BufferType>),
    connections_(),
    next_conn_id_(0) {
  acceptor_.setReadCallback([this](){ this->acceptorReadCallback(); });
}

template<typename BufferType>
void TCPServer<BufferType>::acceptorReadCallback() {
  SocketAddress peer_addr;
  FDHandler accept_fd = acceptor_.accept(peer_addr);
  if (accept_fd < 0) {
    LOG(ERROR) << "accept failed, #active connections = "
               << connections_.size()
               << ", refuse connection";
    return;
  }

  EventLoopPtr& io_loop = io_thread_pool_.getNextLoop();

  char buff[64];
  snprintf(buff, sizeof buff, "%s->%s-#%d",
           peer_addr.toIPPort().c_str(), name_.c_str(), next_conn_id_);
  std::string name(buff);

  connections_[name] = std::make_shared<TCPConnection<BufferType>>(
    io_loop,
    std::move(accept_fd),
    host_addr_,
    peer_addr,
    name
  );
  ++ next_conn_id_;

  const TCPConnectionPtr<BufferType>& conn = connections_[name];

  conn->setCreateCallback([this](const TCPConnectionPtr<BufferType>& curr_conn) {
    this->createCallback_(curr_conn);
  });
  conn->setCloseCallback([this](const TCPConnectionPtr<BufferType>& curr_conn) {
    this->connectionCloseCallback(curr_conn);
  });
  conn->setReadCallback([this](BufferType& buffer, const TCPConnectionPtr<BufferType>& curr_conn) {
    this->readCallback_(buffer, curr_conn);
  });

  io_loop->runInLoop([&conn]() {
    conn->handleCreate();
  });
}

template<typename BufferType>
void TCPServer<BufferType>::connectionCloseCallback(const TCPConnectionPtr<BufferType> &conn) {
  loop_->runInLoop([this, conn](){
    LOG(DEBUG) << "connection " << conn->name() << " erased";
    connections_.erase(conn->name());
  });
}

// explict instantiations
template class net::TCPServer<LinkedBuffer>;
template class net::TCPServer<Buffer>;
