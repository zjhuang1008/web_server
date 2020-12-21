#ifndef EVENT_LOOP_THREAD_POOL_H
#define EVENT_LOOP_THREAD_POOL_H

#include <vector>
#include <memory>

#include "srcs/utils/types.h"
#include "srcs/utils/uncopyable.h"

namespace net {

class Channel;
class EventLoopThread;

class EventLoopThreadPool : private Uncopyable {
public:
  using EventLoopThreadPtr = std::shared_ptr<EventLoopThread>;
  using ChannelPtr = std::shared_ptr<Channel>;

  EventLoopThreadPool(int num_threads);
  ~EventLoopThreadPool() = default;

  // void start();

  void enqueue(ChannelPtr ch);
private:
  int num_threads_;
  std::vector<EventLoopThreadPtr> threads_;
  
  // use round-robin
  // todo: maybe a better way?
  size_t next_thread_idx_;
  EventLoopThreadPtr& getNextThread();
};

} // namespace net

#endif // EVENT_LOOP_THREAD_POOL_H
