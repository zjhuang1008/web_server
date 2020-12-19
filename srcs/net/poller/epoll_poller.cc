#include "srcs/net/poller/epoll_poller.h"

#include <mutex>
#include <sys/epoll.h>
#include <assert.h>

#include "srcs/net/channel/channel.h"
#include "srcs/net/sys_wrapper/sysw.h"

using namespace net;

EpollPoller::EpollPoller() : 
  epoll_fd_(sysw::epoll_create1(EPOLL_CLOEXEC)),
  events_(kInitNumberOfEvents) {
  
}

// EpollPoller::~EpollPoller() {

// }

int EpollPoller::poll(int timeout_ms) {
  active_channels_.clear();
  int num_events = sysw::epoll_wait(epoll_fd_, 
                                    &(*events_.begin()), 
                                    static_cast<int>(events_.size()), 
                                    timeout_ms);
  
  for (int i = 0; i < num_events; ++ i) {
    ChannelPtr ch = fd2channels_[events_[i].data.fd];
    ch->set_revents_type(static_cast<size_t>(events_[i].events));
    active_channels_.push_back(ch);
  }
  
  return num_events;
}

void EpollPoller::addChannel(ChannelPtr ch) {
  int fd = ch->fd();
#ifdef NDEBUG 
  assert(!fd2channels_.count(fd));
#endif
  epollCtlWrapper(ch, EPOLL_CTL_ADD);
  fd2channels_[fd] = std::move(ch);
}

void EpollPoller::updateChannel(const ChannelPtr& ch) {
#ifdef NDEBUG
  assert(fd2channels_.count(fd));
#endif
  epollCtlWrapper(ch, EPOLL_CTL_MOD);  
}

void EpollPoller::removeChannel(const ChannelPtr& ch) {
#ifdef NDEBUG
  assert(fd2channels_.count(fd));
#endif
  epollCtlWrapper(ch, EPOLL_CTL_DEL); 
  fd2channels_.erase(ch->fd());
}

void EpollPoller::epollCtlWrapper(const ChannelPtr& ch, int op) {
  int fd = ch->fd();
  struct epoll_event event;
  event.events = static_cast<uint32_t>(ch->events_type());
  event.data.fd = fd;
  sysw::epoll_ctl(epoll_fd_, op, fd, &event);
}
