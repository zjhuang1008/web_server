#include "srcs/net/server/tcp_server.h"

#include <memory>

#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/channel/channel.h"
#include "srcs/net/server/tcp_connection.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/thread/event_loop.h"

using namespace net;

TCPServer::TCPServer(size_t num_io_threads, SocketAddress host_address)
  : io_thread_pool_(num_io_threads),
    acceptor_(std::move(host_address)) {
  acceptor_.setListenCallback(std::bind(&TCPServer::listenCallback, this));
}

void TCPServer::start() {
  acceptor_.listen();
}

void TCPServer::listenCallback() {
  EventLoopPtr io_loop = io_thread_pool_.getNextLoop();
  SocketAddress peer_address;
  FDHandler accept_fd = acceptor_.accept(peer_address);
  ChannelPtr accept_channel = std::make_shared<Channel>(io_loop, accept_fd);
  TCPConnectionPtr conn = std::make_shared<TCPConnection>(io_loop, accept_channel);

  // accept_channel->setReadCallback(connection_cb_.read);
  // accept_channel->setWriteCallback(connection_cb_.write);
  // accept_channel->setCloseCallback(connection_cb_.close);

  io_loop->runInLoop(std::bind(&TCPConnection::connectionEstablished, conn));
}
