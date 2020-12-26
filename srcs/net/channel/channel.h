#include <memory>
#include <functional>
#include <utility>

#include <sys/poll.h>

#include "srcs/utils/types.h"
#include "srcs/utils/uncopyable.h"
#include "srcs/net/fd_handler/fd_handler.h"

namespace net {

class EventLoop;

// will be shared by a poller and a holder.
class Channel : public std::enable_shared_from_this<Channel>, private Uncopyable {
public:
  using ChannelPtr = std::shared_ptr<Channel>;
  using EventLoopPtr = std::shared_ptr<EventLoop>;

  Channel(FDHandler fd_handler);
  ~Channel() = default;
  
  void setLoop(EventLoopPtr loop) { loop_ = std::move(loop); };
  
  void enableReading() { events_type_ |= POLLIN; };
  void disableReading() { events_type_ &= ~POLLIN; };
  void enableWriting() { events_type_ |= POLLOUT; };
  void disableWriting() { events_type_ &= ~POLLOUT; };

  void updateToPoller();

  void handleEvent();

  // called when channel are first register in loop
  Callback registerCallback() { return registerCallback_; }
  void setRegisterCallback(Callback cb) { registerCallback_ = std::move(cb); };
  void defaultRegisterCallback();

  // called when read is ready on fd
  void setReadCallback(Callback cb) { readCallback_ = std::move(cb); };
  // called when write is ready on fd
  void setWriteCallback(Callback cb) { writeCallback_ = std::move(cb); };
  // called when close
  void setCloseCallback(Callback cb) { closeCallback_ = std::move(cb); };

  int fd() { return fd_handler_; }
  size_t events_type() { return events_type_; }
  size_t revents_type() { return revents_type_; }
  void set_revents_type(size_t revents_type) { revents_type_ = revents_type; }

private:
  EventLoopPtr loop_;
  FDHandler fd_handler_;
  size_t events_type_;  // epoll events
  size_t revents_type_;  // epoll revents

  Callback registerCallback_;
  Callback readCallback_;
  Callback writeCallback_;
  Callback closeCallback_;
};

} // namespace net