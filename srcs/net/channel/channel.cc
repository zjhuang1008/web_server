#include "srcs/net/channel/channel.h"

#include <functional>
#include <sys/poll.h>
#include <unistd.h>

#include "srcs/net/thread/event_loop.h"


using namespace net;

Channel::Channel(FDHandler fd_handler) : 
  fd_handler_(fd_handler),
  registerCallback_(std::bind(&Channel::defaultRegisterCallback, this)) {

}

// Channel::~Channel() {
// }

void Channel::defaultRegisterCallback() {
  loop_->addChannelInPoller(shared_from_this());
  printf("register callback finished.\n");
}

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
}
