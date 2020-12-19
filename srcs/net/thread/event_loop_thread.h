#ifndef EVENT_LOOP_THREAD_H
#define EVENT_LOOP_THREAD_H

#include <vector>
#include <memory>
#include <thread>

#include "srcs/net/utils/uncopyable.h"

namespace net {

class Channel;
class EventLoop;

class EventLoopThread : private Uncopyable {
public:
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  using ChannelPtr = std::shared_ptr<Channel>;

  EventLoopThread();
  ~EventLoopThread();
  
  void startLoop();
  // EventLoopPtr getLoop() { return loop_; };

  void addChannel(ChannelPtr ch);
  void updateChannel(ChannelPtr ch);
private:
  EventLoopPtr loop_;
  std::thread thread_;
};

} // namespace net

#endif // EVENT_LOOP_THREAD_H
