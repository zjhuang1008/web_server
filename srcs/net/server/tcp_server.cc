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

TCPServer::TCPServer(EventLoopPtr loop, size_t num_io_threads, SocketAddress host_addr)
  : io_thread_pool_(num_io_threads),
    acceptor_(std::move(loop)),
    host_addr_(std::move(host_addr)),
    createCallback_(net::defaultCreateCallback),
    readCallback_(net::defaultReadCallback) {
  acceptor_.setReadCallback([this](){ this->acceptorReadCallback(); });
}

void TCPServer::start() {
  acceptor_.listen(host_addr_);
}

void TCPServer::acceptorReadCallback() {
  EventLoopPtr io_loop = io_thread_pool_.getNextLoop();
  
  SocketAddress peer_addr;
  FDHandler accept_fd = acceptor_.accept(peer_addr);

  char buff[64];
  snprintf(buff, sizeof buff, "%s-#%d", host_addr_.toIPPort().c_str(), nextConnID_);
  ++ nextConnID_;

  TCPConnectionPtr conn = std::make_shared<TCPConnection>(
    io_loop, 
    std::move(accept_fd),
    host_addr_,
    peer_addr,
    std::string(buff)
  );

  conn->setCreateCallback([this](const TCPConnectionPtr& curr_conn) {
    this->createCallback_(curr_conn);
  });
  conn->setCloseCallback([this](const TCPConnectionPtr& curr_conn) {
    this->connectionCloseCallback(curr_conn);
  });
  conn->setReadCallback([this](Buffer& buffer, const TCPConnectionPtr& curr_conn) {
    this->readCallback_(buffer, curr_conn);
  });
  
  io_loop->runInLoop(
    std::bind(
      [this](TCPConnectionPtr curr_conn) {
        curr_conn->handleCreate();
        connections_[curr_conn->name()] = std::move(curr_conn);
      }, 
      std::move(conn)
    )
  );
}

void TCPServer::connectionCloseCallback(const TCPConnectionPtr &conn) {
  connections_.erase(conn->name());
}
