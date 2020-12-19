#ifndef EPOLL_POLLER_H
#define EPOLL_POLLER_H

#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/poller/poller.h"

#include <mutex>
#include <unordered_map> 
#include <vector>
#include <sys/epoll.h>

namespace net {

class Channel;

class EpollPoller : public Poller {
public:
  using EventList = std::vector<struct epoll_event>;

  EpollPoller();
  ~EpollPoller() = default;

  int poll(int timeout_ms) override;
  
  void addChannel(ChannelPtr ch) override;
  void updateChannel(const ChannelPtr& ch) override;
  void removeChannel(const ChannelPtr& ch) override;

  void epollCtlWrapper(const ChannelPtr& ch, int op);
private:
  static constexpr int kInitNumberOfEvents = 16;
  FDHandler epoll_fd_;

  EventList events_;
};

} // namespace net

#endif // EPOLL_POLLER_H
