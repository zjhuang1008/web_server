#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <vector>
#include <memory>
#include <mutex>
#include <thread>

#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"

namespace net {

class Channel;
class Poller;

class EventLoop : public std::enable_shared_from_this<EventLoop>, private Uncopyable {
public:
  EventLoop();
  ~EventLoop() = default;

  /// used by main thread
  void setWakeupChannel();
  void wakeup();

  void runInLoop(Callback cb);
  void queueInLoop(Callback cb);

  /// used by this thread
  void wakeupCallback();

  void addChannelInPoller(ChannelPtr ch);
  void updateChannelInPoller(const ChannelPtr& ch);
  // void removeChannel(const ChannelPtr& ch);

  bool isInLoopThread() { return thread_id_ == std::this_thread::get_id(); }

  void startLoop();
  void endLoop();

private:
  void loop();
  void doPendingCallbacks();

  mutable std::mutex mutex_;
  std::thread::id thread_id_;

  bool looping_;
  // bool doing_pending_callbacks_;

  ChannelPtr wakeup_channel_;
  FDHandler wakeup_fd_;  
  // used by this thread
  PollerPtr poller_;
  
  std::vector<Callback> pending_callbacks_;

};

} // namespace net

#endif // EVENT_LOOP_H
