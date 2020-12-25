#include "srcs/server/tcp_server.h"

using namespace net;

TCPServer::TCPServer(size_t num_io_threads, SocketAddress socket_address)
  : io_thread_pool_(num_io_threads),
    acceptor_(std::move(socket_address)) {
  
}
