#include "srcs/net/connection/tcp_connection.h"

#include <utility>
#include <cassert>
#include <sys/socket.h>

#include "srcs/net/channel/channel.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/logger/logger.h"
#include "srcs/net/sys_wrapper/sysw.h"

using namespace net;

TCPConnection::TCPConnection(const EventLoopPtr& io_loop,
                             FDHandler accept_fd, 
                             const SocketAddress& host_addr,
                             const SocketAddress& peer_addr,
                             std::string name)
  : io_loop_(io_loop),
    channel_(std::make_shared<Channel>(io_loop, std::move(accept_fd))),
    host_addr_(host_addr),
    peer_addr_(peer_addr),
    name_(std::move(name)),
    status_(Status::kConnecting) {
  channel_->setCloseCallback([this]() { this->handleClose(); });
  channel_->setErrorCallback([this]() { this->handleError(); });
  channel_->setReadCallback([this]() { this->handleRead(); });
  channel_->setWriteCallback([this]() { this->handleWrite(); });
  LOG(DEBUG) << "connection " << name_ << " receive accept fd: " << channel_->fd();
}

void TCPConnection::handleCreate() {
  LOG(DEBUG) << "connection " << name_ << " created";
  channel_->enableReading();
  io_loop_->addChannelInPoller(channel_);
  status_ = Status::kConnected;

  createCallback_(shared_from_this());
}

void TCPConnection::handleClose() {
  LOG(DEBUG) << "connection " << name_ << " closed";
  io_loop_->removeChannelInPoller(channel_);
  status_ = Status::kDisconnected;

  closeCallback_(shared_from_this());
}

void TCPConnection::handleError() {
  handleClose();
}

void TCPConnection::handleRead() {
  LOG(DEBUG) << "connection " << name_ << " read accept fd: " << channel_->fd();
  ssize_t n = in_buffer_.writeFromFD(channel_->fd());

  if (n > 0) {
    readCallback_(in_buffer_, shared_from_this());
  } else if (n == 0) {
    handleClose();
  } else {
    LOGSYS(ERROR) << "error happened when reading from connection " << name_;
    handleError();
  }
}

void TCPConnection::handleWrite() {
  LOG(DEBUG) << "connection " << name_ << " write accept fd: " << channel_->fd();
  ssize_t n = out_buffer_.readToFD(channel_->fd());

  if (n < 0) {
    LOGSYS(ERROR) << "error happened when writing to connection " << name_;
    handleError();
    return;
  }

  if (out_buffer_.readableSize() == 0) {
    // disable writing to avoid busy loop
    channel_->disableWriting();
    io_loop_->updateChannelInPoller(channel_);

    if (status_ == Status::kDisconnecting) {
      shutdown();
    }

    // TODO: writeCompleteCallback
//    if (writeCompleteCallback_)
//      writeCompleteCallback_();
  }
}

void TCPConnection::send(const Buffer& buffer) {
  assertInLoop();

  out_buffer_.append(buffer.readerIter(), buffer.readableSize());

  // TODO: highWaterMarkCallback

  if (!channel_->isWriting()) {
    channel_->enableWriting();
    io_loop_->updateChannelInPoller(channel_);

    // perform write immediately when no writes before
    handleWrite();
  }
}

void TCPConnection::shutdown() {
  assertInLoop();
  status_ = Status::kDisconnecting;

  if (!channel_->isWriting()) {
    LOG(DEBUG) << "connection " << name_ << " shutdown";
    sysw::shutdown(channel_->fd(), SHUT_WR);
  }
}

void TCPConnection::assertInLoop() {
  assert(io_loop_->isInLoopThread());
}
