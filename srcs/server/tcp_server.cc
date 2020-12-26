#include "srcs/server/tcp_server.h"

#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/channel/channel.h"

using namespace net;

TCPServer::TCPServer(size_t num_io_threads, SocketAddress socket_address)
  : io_thread_pool_(num_io_threads),
    acceptor_(std::move(socket_address)) {
  acceptor_.setSocketReadCallback(std::bind(&TCPServer::socketReadCallback, this));
}

void TCPServer::start() {
  acceptor_.listen();
}

void TCPServer::socketReadCallback() {
  std::shared_ptr<Channel> channel = acceptor_.accept();
  io_thread_pool_.enqueue(channel);
}