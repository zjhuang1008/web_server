#ifndef EVENT_LOOP_THREAD_H
#define EVENT_LOOP_THREAD_H

#include <vector>
#include <memory>
#include <thread>

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"

namespace net {

class EventLoopThread : private Uncopyable {
public:
  EventLoopThread();
  ~EventLoopThread() = default;
  
  void startLoop();
  EventLoopPtr loop() { return loop_; };

  // void addChannel(ChannelPtr ch);
private:
  EventLoopPtr loop_;
  std::thread thread_;
};

} // namespace net

#endif // EVENT_LOOP_THREAD_H
