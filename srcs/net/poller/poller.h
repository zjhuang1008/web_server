#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <unordered_map>
#include <memory>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"

namespace net {

class Poller : private Uncopyable {
public:
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
