#ifndef CHANNEL_H
#define CHANNEL_H

#include <memory>
#include <functional>
#include <utility>

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"
#include "srcs/net/fd_handler/fd_handler.h"

namespace net {

// will be shared by a poller and a holder.
class Channel : public std::enable_shared_from_this<Channel>, private Uncopyable {
public:
  Channel(EventLoopPtr loop, FDHandler fd_handler);
  ~Channel() = default;
    
  void enableReading() { events_type_ |= kReadEvent; }
  void disableReading() { events_type_ &= ~kReadEvent; }
  void enableWriting() { events_type_ |= kWriteEvent; }
  void disableWriting() { events_type_ &= ~kWriteEvent; }
  void disableAll() { events_type_ = kNoneEvent; }

  bool isWriting() const { return events_type_ & kWriteEvent; }
  bool isReading() const { return events_type_ & kReadEvent; }

//  void update();

  void handleEvent();
  
  // called when read is ready on fd
  void setReadCallback(Callback cb) { readCallback_ = std::move(cb); }
  // called when write is ready on fd
  void setWriteCallback(Callback cb) { writeCallback_ = std::move(cb); }
  // called when close
  void setCloseCallback(Callback cb) { closeCallback_ = std::move(cb); }
  // called when error happened
  void setErrorCallback(Callback cb) { errorCallback_ = std::move(cb); }

  int fd() { return fd_handler_; }
  size_t events_type() { return events_type_; }
  size_t revents_type() { return revents_type_; }
  void set_revents_type(size_t revents_type) { revents_type_ = revents_type; }

private:
  EventLoopPtr loop_;
  FDHandler fd_handler_;
  size_t events_type_;  // epoll events
  size_t revents_type_;  // epoll revents

  // Callback registerCallback_;
  Callback readCallback_;
  Callback writeCallback_;
  Callback closeCallback_;
  Callback errorCallback_;

  static const size_t kReadEvent;
  static const size_t kWriteEvent;
  static const size_t kNoneEvent;
};

} // namespace net

#endif // CHANNEL_H
