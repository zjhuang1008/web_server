#include "srcs/net/poller/epoll_poller.h"

#include <mutex>
#include <sys/epoll.h>
#include <cassert>

#include "srcs/net/channel/channel.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/logger/logger.h"

static const std::unordered_map<int, std::string> epoll_ctl_op_names = {
  {EPOLL_CTL_ADD, "EPOLL_CTL_ADD"},
  {EPOLL_CTL_DEL, "EPOLL_CTL_DEL"},
  {EPOLL_CTL_MOD, "EPOLL_CTL_MOD"},
};

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
  
  if (num_events < 0) return num_events;

  if (static_cast<size_t>(num_events) == events_.size()) {
    events_.resize(events_.size() * 2);
  }
//  LOG(DEBUG) << Fmt("get %d events", num_events);

  for (size_t i = 0; i < static_cast<size_t>(num_events); ++ i) {
    ChannelPtr ch = fd2channels_[events_[i].data.fd];
//    LOG(DEBUG) << Fmt("get ch with fd=%d event", ch->fd());
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
  // TODO: use ptr to reduce a lookup in fd2channels_ when event happens
  int ok = sysw::epoll_ctl(epoll_fd_, op, fd, &event);
  if (ok < 0 || event.events == 0) {
    LOG(DEBUG) << "call epoll_ctl with "
               << "epfd=" << epoll_fd_ << ", "
               << "op=" << epoll_ctl_op_names.at(op) << ", "
               << "fd=" << fd << ", "
               << "events=" << Fmt("%08X", event.events);
  }
}
