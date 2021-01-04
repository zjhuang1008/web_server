#include "srcs/net/server/tcp_server.h"

#include <memory>

#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/channel/channel.h"
#include "srcs/net/connection/tcp_connection.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/thread/event_loop.h"

namespace net {

static void defaultBufferReadingFunction(Buffer& buffer) {
  buffer.readAll();
}

} // namespace net

using namespace net;

TCPServer::TCPServer(EventLoopPtr loop, size_t num_io_threads, SocketAddress host_address)
  : io_thread_pool_(num_io_threads),
    acceptor_(std::move(loop), std::move(host_address)),
    readCallback_(net::defaultBufferReadingFunction) {
  acceptor_.setListenCallback([this](){ this->listenCallback(); });
}

void TCPServer::start() {
  acceptor_.listen();
}

void TCPServer::listenCallback() {
  EventLoopPtr io_loop = io_thread_pool_.getNextLoop();
  
  SocketAddress peer_address;
  FDHandler accept_fd = acceptor_.accept(peer_address);
  std::string conn_name = peer_address.toIPPort();

  TCPConnectionPtr conn = std::make_shared<TCPConnection>(
    io_loop, 
    std::move(accept_fd),
    std::move(conn_name)
  );

  conn->setCloseCallback([this](const TCPConnectionPtr& curr_conn) {
    this->connectionCloseCallback(curr_conn);
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
