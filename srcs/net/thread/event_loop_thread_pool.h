#ifndef EVENT_LOOP_THREAD_POOL_H
#define EVENT_LOOP_THREAD_POOL_H

#include <vector>
#include <memory>

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"

namespace net {

class Channel;
class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : private Uncopyable {
public:
  // using EventLoopThreadPtr = std::shared_ptr<EventLoopThread>;
  // using EventLoopPtr = std::shared_ptr<EventLoop>;
  // using ChannelPtr = std::shared_ptr<Channel>;

  EventLoopThreadPool(size_t num_threads);
  ~EventLoopThreadPool() = default;

  EventLoopPtr getNextLoop();

  // void start();

  // void enqueue(ChannelPtr ch);
private:
  size_t num_threads_;
  std::vector<EventLoopThreadPtr> threads_;
  
  // use round-robin
  // todo: maybe a better way?
  size_t next_thread_idx_;
  EventLoopThreadPtr& getNextThread();
};

} // namespace net

#endif // EVENT_LOOP_THREAD_POOL_H
