#include "srcs/net/server/tcp_connection.h"

#include <utility>

#include "srcs/net/channel/channel.h"
#include "srcs/net/thread/event_loop.h"

using namespace net;

TCPConnection::TCPConnection(EventLoopPtr io_loop, ChannelPtr channel)
  : io_loop_(std::move(io_loop)),
    channel_(std::move(channel)) {
  
}

void TCPConnection::connectionEstablished() {
  channel_->enableReading();
  io_loop_->addChannelInPoller(channel_);
}