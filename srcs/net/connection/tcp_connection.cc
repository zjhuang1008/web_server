#include "srcs/net/connection/tcp_connection.h"

#include <utility>

#include "srcs/net/channel/channel.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/logger/logger.h"

using namespace net;

TCPConnection::TCPConnection(EventLoopPtr io_loop, 
                             FDHandler accept_fd, 
                             const SocketAddress& host_addr,
                             const SocketAddress& peer_addr,
                             std::string name)
  : io_loop_(io_loop),
    channel_(std::make_shared<Channel>(io_loop, std::move(accept_fd))),
    host_addr_(std::move(host_addr)),
    peer_addr_(std::move(peer_addr)),
    name_(std::move(name)) {
  channel_->setReadCallback([this]() { this->handleRead(); });
  channel_->setCloseCallback([this]() { this->handleClose(); });
  channel_->setErrorCallback([this]() { this->handleError(); });
}

void TCPConnection::handleCreate() {
  channel_->enableReading();
  io_loop_->addChannelInPoller(channel_);

  createCallback_(shared_from_this());
}

void TCPConnection::handleClose() {
  LOG(DEBUG) << "connection " << name_ << "closed";
  io_loop_->removeChannelInPoller(channel_);

  closeCallback_(shared_from_this());
}

void TCPConnection::handleError() {
  handleClose();
}

void TCPConnection::handleRead() {
  ssize_t n = in_buffer_.write(channel_->fd());

  if (n > 0) {
    readCallback_(in_buffer_, shared_from_this());
  } else if (n == 0) {
    handleClose();
  } else {
    LOGSYS(ERROR) << "error happened in connection " << name_;
    handleError();
  }
}