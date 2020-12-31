#include "srcs/net/connection/tcp_connection.h"

#include <utility>

#include "srcs/net/channel/channel.h"
#include "srcs/net/thread/event_loop.h"

using namespace net;

TCPConnection::TCPConnection(EventLoopPtr io_loop, ChannelPtr channel)
  : io_loop_(std::move(io_loop)),
    channel_(std::move(channel)),
    buffer_() {
  channel_->setReadCallback(std::bind(&TCPConnection::handleRead, this));
  channel_->setCloseCallback(std::bind(&TCPConnection::handleClose, this));
  channel_->setErrorCallback(std::bind(&TCPConnection::handleError, this));
}

void TCPConnection::handleRead() {
  int saved_errno = 0;
  ssize_t n = buffer_.write(channel_->fd(), saved_errno);

  if (n > 0) {
    bufferReadingFunction_(buffer_);
  } else if (n == 0) {
    handleClose();
  } else {
    handleError();
  }
}

void TCPConnection::handleClose() {
  
}

void TCPConnection::handleError() {
  
}

void TCPConnection::connectionEstablished() {
  channel_->enableReading();
  io_loop_->addChannelInPoller(channel_);
}

// void TCPConnection::defaultBufferReadingFunction(Buffer& buffer) {
//   buffer.readAll();
// }