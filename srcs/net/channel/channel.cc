#include "srcs/net/channel/channel.h"

#include <functional>
#include <utility>
#include <sys/poll.h>
#include <unistd.h>

#include "srcs/net/thread/event_loop.h"
#include "srcs/logger/logger.h"

namespace net {

const size_t Channel::kReadEvent = POLLIN | POLLPRI;
const size_t Channel::kWriteEvent = POLLOUT;
const size_t Channel::kNoneEvent = 0;

} // namespace net

using namespace net;

Channel::Channel(EventLoopPtr loop, FDHandler fd_handler) : 
  loop_(std::move(loop)),
  fd_handler_(std::move(fd_handler)),
  events_type_(kNoneEvent),
  revents_type_(kNoneEvent) {

}

// Channel::~Channel() {
// }

//void Channel::update() {
//  loop_->updateChannelInPoller(shared_from_this());
//}

void Channel::handleEvent() {
  // TODO: whether needed
  // use guard to reserve a reference to this in this scope.
  auto guard = shared_from_this();

  if ((revents_type_ & POLLHUP) && !(revents_type_ & POLLIN)) {
    LOG(DEBUG) << "fd " << fd_handler_ << " receive POLLHUP";
    if (closeCallback_) closeCallback_();
  }

  if (revents_type_ & (POLLERR | POLLNVAL)) {
    LOG(DEBUG) << "fd " << fd_handler_ << " receive POLLERR | POLLNVAL";
    if (errorCallback_) errorCallback_();
  }

  if (revents_type_ & (POLLIN | POLLRDHUP | POLLPRI)) {
    if (readCallback_) readCallback_();
  }
  
  if (revents_type_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }
}
