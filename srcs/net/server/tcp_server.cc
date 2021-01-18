#include "srcs/net/server/tcp_server.h"

#include <memory>

#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/channel/channel.h"
#include "srcs/net/connection/tcp_connection.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/logger/logger.h"

namespace net {

static void defaultCreateCallback(const TCPConnectionPtr& conn) {
  LOG(INFO) << conn->peer_addr().toIPPort() << " -> " 
            << conn->host_addr().toIPPort() << " established";
}

static void defaultReadCallback(Buffer& buffer, const TCPConnectionPtr& conn) {
  buffer.readAll();
}

} // namespace net

using namespace net;

TCPServer::TCPServer(const EventLoopPtr& loop, size_t num_io_threads, SocketAddress host_addr)
  : loop_(loop),
    io_thread_pool_(num_io_threads),
    acceptor_(loop),
    host_addr_(host_addr),
    createCallback_(net::defaultCreateCallback),
    readCallback_(net::defaultReadCallback),
    connections_(),
    next_conn_id_(0) {
  acceptor_.setReadCallback([this](){ this->acceptorReadCallback(); });
}

void TCPServer::start() {
  acceptor_.listen(host_addr_);
}

void TCPServer::acceptorReadCallback() {
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
           peer_addr.toIPPort().c_str(), host_addr_.toIPPort().c_str(), next_conn_id_);
  std::string name(buff);

  connections_[name] = std::make_shared<TCPConnection>(
    io_loop,
    std::move(accept_fd),
    host_addr_,
    peer_addr,
    name
  );
  ++ next_conn_id_;

  const TCPConnectionPtr& conn = connections_[name];

  conn->setCreateCallback([this](const TCPConnectionPtr& curr_conn) {
    this->createCallback_(curr_conn);
  });
  conn->setCloseCallback([this](const TCPConnectionPtr& curr_conn) {
    this->connectionCloseCallback(curr_conn);
  });
  conn->setReadCallback([this](Buffer& buffer, const TCPConnectionPtr& curr_conn) {
    this->readCallback_(buffer, curr_conn);
  });
  
  io_loop->runInLoop([&conn]() {
    conn->handleCreate();
  });
}

void TCPServer::connectionCloseCallback(const TCPConnectionPtr &conn) {
  loop_->runInLoop([this, conn](){
    LOG(DEBUG) << "connection " << conn->name() << " erased";
    connections_.erase(conn->name());
  });
}
