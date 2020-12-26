#include "srcs/net/thread/event_loop.h"

#include <memory>

#include <vector>
#include <mutex>
// #include <thread>
#include <sys/eventfd.h>

#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/channel/channel.h"
#include "srcs/net/poller/poller.h"
#include "srcs/net/fd_handler/fd_handler.h"

using namespace net;

static const int kPollTimeMs = -1;

EventLoop::EventLoop() : 
  thread_id_(std::this_thread::get_id()),
  wakeup_fd_(sysw::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
  poller_(Poller::newPoller()) {
}

// EventLoop::~EventLoop() {
// }

void EventLoop::setWakeupChannel() {
  wakeup_channel_ = std::make_shared<Channel>(wakeup_fd_);
  wakeup_channel_->setLoop(shared_from_this());
  wakeup_channel_->enableReading();
  wakeup_channel_->setReadCallback(std::bind(&EventLoop::wakeupCallback, this));
  this->addChannelInPoller(wakeup_channel_);
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  sysw::write(wakeup_fd_, &one, sizeof(one));
}

void EventLoop::wakeupCallback() {
  uint64_t buf;
  sysw::read(wakeup_fd_, &buf, sizeof(buf));
}

void EventLoop::addChannelInPoller(ChannelPtr ch) {
  poller_->addChannel(std::move(ch));
}

void EventLoop::updateChannelInPoller(const ChannelPtr& ch) {
  poller_->updateChannel(ch);
}

// void EventLoop::removeChannel(const ChannelPtr& ch) {
//   poller_->removeChannel(ch);
// }

void EventLoop::startLoop() {
  {
    std::lock_guard<std::mutex> lk(mutex_);
    looping_ = true;
    thread_id_ = std::this_thread::get_id();
  }
  loop();
}

void EventLoop::endLoop() {
  {
    std::lock_guard<std::mutex> lk(mutex_);
    looping_ = false;
  }
}

void EventLoop::loop() {
  while (looping_) {
    // int num_events = 
    poller_->poll(kPollTimeMs);
    
    for (auto& ch : poller_->active_channels()) {
      ch->handleEvent();
    }
    
    doPendingCallbacks();
  }
}

void EventLoop::doPendingCallbacks() {
  std::vector<Callback> excuting_callbacks;
  {
    std::lock_guard<std::mutex> lk(mutex_);
    excuting_callbacks.swap(pending_callbacks_);
  }

  for (const Callback& cb : excuting_callbacks) {
    cb();
  }
}

void EventLoop::runInLoop(Callback cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(std::move(cb));
  }
}

void EventLoop::queueInLoop(Callback cb) {
  {
    std::lock_guard<std::mutex> lk(mutex_);
    pending_callbacks_.push_back(std::move(cb));
  }
  
  if (!isInLoopThread()) {
    // TODO: wakeup only no events happening.
    wakeup();
  }
}
