#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <memory>

namespace net {
  class Channel;
  class EventLoop;
  class EventLoopThread;
  class Poller;
  class TCPConnection;
  class Buffer;

  using Callback = std::function<void()>;
  using BufferReadingFunction = std::function<void(Buffer&)>;

  using ChannelPtr = std::shared_ptr<Channel>;
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  using EventLoopThreadPtr = std::shared_ptr<EventLoopThread>;
  
  using PollerPtr = std::shared_ptr<Poller>;

  using TCPConnectionPtr = std::shared_ptr<TCPConnection>;

} // namespace net

#endif // TYPES_H
