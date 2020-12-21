#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <unordered_map>
#include <memory>

#include "srcs/utils/uncopyable.h"

namespace net {

class Channel;

class Poller : private Uncopyable {
public:
  using PollerPtr = std::shared_ptr<Poller>;
  using ChannelPtr = std::shared_ptr<Channel>;
  using ChannelPtrList = std::vector<ChannelPtr>;

  Poller();
  virtual ~Poller();

  static PollerPtr newPoller();

  virtual int poll(int timeout_ms) = 0;
  
  std::vector<ChannelPtr> active_channels() { return active_channels_; };

  virtual void addChannel(ChannelPtr ch) = 0;
  virtual void updateChannel(const ChannelPtr& ch) = 0;
  virtual void removeChannel(const ChannelPtr& ch) = 0;
protected:
  std::unordered_map<int, ChannelPtr> fd2channels_;
  std::vector<ChannelPtr> active_channels_;
};

} // namespace net

#endif // POLLER_H
