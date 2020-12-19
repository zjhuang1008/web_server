#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <vector>
#include <memory>
#include <mutex>

#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/utils/types.h"
#include "srcs/net/utils/uncopyable.h"

namespace net {

class Channel;
class Poller;

class EventLoop : public std::enable_shared_from_this<EventLoop>, private Uncopyable {
public:
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  using PollerPtr = std::shared_ptr<Poller>;
  using ChannelPtr = std::shared_ptr<Channel>;

  EventLoop();
  ~EventLoop() = default;

  /// used by main thread
  void setWakeupChannel();
  void wakeup();
  void addPendingCallbacks(Callback cb);

  /// used by this thread
  void wakeupCallback();

  void addChannelInPoller(ChannelPtr ch);
  void updateChannelInPoller(const ChannelPtr& ch);
  // void removeChannel(const ChannelPtr& ch);

  void loop();

  void doPendingCallbacks();
private:
  mutable std::mutex mutex_;
  bool quit_;

  ChannelPtr wakeup_channel_;
  FDHandler wakeup_fd_;  
  // used by this thread
  PollerPtr poller_;
  
  std::vector<Callback> pending_callbacks_;

};

} // namespace net

#endif // EVENT_LOOP_H
