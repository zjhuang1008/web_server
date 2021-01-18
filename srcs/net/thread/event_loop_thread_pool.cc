#include "srcs/net/thread/event_loop_thread_pool.h"

#include <memory>

#include "srcs/net/thread/event_loop_thread.h"
#include "srcs/net/channel/channel.h"

using namespace net;

EventLoopThreadPool::EventLoopThreadPool(size_t num_threads) : 
  num_threads_(num_threads),
  next_thread_idx_(0) {
  for (size_t i = 0; i < num_threads_; ++ i) {
    auto event_loop_thread_ptr = std::make_shared<EventLoopThread>();
    threads_.push_back(event_loop_thread_ptr);
    event_loop_thread_ptr->startLoop();
  }
}

// EventLoopThreadPool::~EventLoopThreadPool() {
// }

EventLoopPtr& EventLoopThreadPool::getNextLoop() {
  return getNextThread()->loop();
}

EventLoopThreadPtr& EventLoopThreadPool::getNextThread() {
  EventLoopThreadPtr& thread = threads_[next_thread_idx_];
  next_thread_idx_ = (next_thread_idx_ + 1) % (threads_.size());

  return thread;
};
