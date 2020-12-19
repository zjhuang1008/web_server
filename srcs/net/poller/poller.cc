#include "srcs/net/poller/poller.h"

#include "srcs/net/poller/epoll_poller.h"

using namespace net;

Poller::Poller() {

}

Poller::~Poller() {

}

Poller::PollerPtr Poller::newPoller() {
#ifdef USE_EPOLL
  return std::make_shared<EpollPoller>();
#endif
#ifndef USE_EPOLL
  static_assert(false, "poll is not implemented yet");
#endif
}
