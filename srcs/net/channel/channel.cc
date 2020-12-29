#include "srcs/net/channel/channel.h"

#include <functional>
#include <sys/poll.h>
#include <unistd.h>

#include "srcs/net/thread/event_loop.h"
#include "srcs/logger/logger.h"

using namespace net;

Channel::Channel(EventLoopPtr loop, FDHandler fd_handler) : 
  loop_(std::move(loop)),
  fd_handler_(fd_handler) {

}

// Channel::~Channel() {
// }

// void Channel::defaultRegisterCallback() {
//   loop_->addChannelInPoller(shared_from_this());
//   LOG(DEBUG) << "register callback finished.";
// }

void Channel::updateToPoller() {
  loop_->updateChannelInPoller(shared_from_this());
}

void Channel::handleEvent() {
  if (revents_type_ & (POLLIN | POLLRDHUP | POLLPRI)) {
    if (readCallback_) readCallback_();
  }
  
  if (revents_type_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }

  if ((revents_type_ & POLLHUP) && !(revents_type_ & POLLIN)) {
    if (closeCallback_) closeCallback_();
  }

  if (revents_type_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_();
  }
}
