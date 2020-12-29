#include "srcs/net/thread/event_loop_thread.h"

#include <functional>
#include <thread>

#include "srcs/net/channel/channel.h"
#include "srcs/net/thread/event_loop.h"

using namespace net;

EventLoopThread::EventLoopThread() : loop_(std::make_shared<EventLoop>()) {
  loop_->setWakeupChannel();
}

// EventLoopThread::~EventLoopThread() {
// }

void EventLoopThread::startLoop() {
  thread_ = std::thread(std::bind(&EventLoop::startLoop, loop_));
}

// void EventLoopThread::addChannel(ChannelPtr ch) {
//   ch->setLoop(loop_); 
//   // send job to the thread
//   loop_->runInLoop(ch->registerCallback());
// }
