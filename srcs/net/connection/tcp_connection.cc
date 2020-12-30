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
}

void TCPConnection::handleRead() {
  int saved_errno = 0;
  buffer_.write(channel_->fd(), saved_errno);
}

void TCPConnection::handleClose() {
  
}

void TCPConnection::connectionEstablished() {
  channel_->enableReading();
  io_loop_->addChannelInPoller(channel_);
}