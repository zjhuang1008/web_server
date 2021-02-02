#include "srcs/net/connection/tcp_connection.h"

#include <utility>
#include <cassert>
#include <sys/socket.h>

#include "srcs/net/channel/channel.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/logger/logger.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/connection/buffer/buffer_factory.h"

using namespace net;

template<typename BufferType>
TCPConnection<BufferType>::TCPConnection(EventLoopPtr& io_loop,
                             FDHandler socket_fd,
                             const SocketAddress& host_addr,
                             const SocketAddress& peer_addr,
                             std::string name)
  : io_loop_(io_loop),
    channel_(std::make_shared<Channel>(io_loop, std::move(socket_fd))),
    host_addr_(host_addr),
    peer_addr_(peer_addr),
    name_(std::move(name)),
    status_(Status::kConnecting) {
  channel_->setCloseCallback([this]() { this->handleClose(); });
  channel_->setErrorCallback([this]() { this->handleError(); });
  channel_->setReadCallback([this]() { this->handleRead(); });
  channel_->setWriteCallback([this]() { this->handleWrite(); });
  LOG(DEBUG) << "connection " << name_ << " receive socket fd: " << channel_->fd();
}

template<typename BufferType>
void TCPConnection<BufferType>::handleCreate() {
  assert(status_ == Status::kConnecting);
//  LOG(DEBUG) << "connection " << name_ << " created";
  channel_->enableReading();
  io_loop_->addChannelInPoller(channel_);
  status_ = Status::kConnected;

  createCallback_(this->shared_from_this());
}

template<typename BufferType>
void TCPConnection<BufferType>::handleClose() {
  if (status_ != Status::kDisconnected) {
    LOG(DEBUG) << "connection " << name_ << " closed";
    io_loop_->removeChannelInPoller(channel_);
    // because closeCallback_ will let another thread to do the close task
    // set status_ to kDisconnected to prevent this function being level-triggered again
    status_ = Status::kDisconnected;

    closeCallback_(this->shared_from_this());
  }
}

template<typename BufferType>
void TCPConnection<BufferType>::handleError() {
  handleClose();
}

template<typename BufferType>
void TCPConnection<BufferType>::handleRead() {
  if (status_ == Status::kConnected || status_ == Status::kDisconnecting) {
    // when status_ is Status::kDisconnecting, EOF will read from the socket fd.
    LOG(DEBUG) << "connection " << name_ << " read socket fd: " << channel_->fd();
    ssize_t n = in_buffer_.writeFromFD(channel_->fd());

    if (n > 0) {
      readCallback_(in_buffer_, this->shared_from_this());
    } else if (n == 0) {
      // read EOF (it means another end calls shutdown)
      handleClose();
    } else {
      LOGSYS(ERROR) << "error happened when reading from connection " << name_;
      handleError();
    }
  }
}

template<typename BufferType>
void TCPConnection<BufferType>::handleWrite() {
  assert(status_ == Status::kConnected);

  LOG(DEBUG) << "connection " << name_ << " write socket fd: " << channel_->fd();
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

template<typename BufferType>
void TCPConnection<BufferType>::send(SendCallback cb) {
  assertInLoop();

  // SendCallback will append the response message to the out_buffer_
  cb(out_buffer_);

  if (!channel_->isWriting()) {
    // send buffer immediately if the connfd is not listened by epoll
    // if the message can be sent in one call, then we don't need to let epoll to listen on this connfd
    ssize_t n = out_buffer_.readToFD(channel_->fd());
    if (n < 0) {
      LOGSYS(ERROR) << "error happened when writing to connection " << name_;
      handleError();
      return;
    }

    if (out_buffer_.readableSize() > 0) {
      // if there is any content in the out_buffer_, then add this connfd to epoll
      channel_->enableWriting();
      io_loop_->updateChannelInPoller(channel_);
    }
  }
}

template<typename BufferType>
void TCPConnection<BufferType>::shutdown() {
  assertInLoop();
  if (status_ == Status::kDisconnected)
    return;

  status_ = Status::kDisconnecting;

  if (!channel_->isWriting()) {
    LOG(DEBUG) << "connection " << name_ << " shutdown";
    sysw::shutdown(channel_->fd(), SHUT_WR);
  }
}

template<typename BufferType>
void TCPConnection<BufferType>::assertInLoop() {
  assert(io_loop_->isInLoopThread());
}


// explict instantiations
template class net::TCPConnection<LinkedBuffer>;
template class net::TCPConnection<Buffer>;
