#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <memory>

namespace net {
  using std::size_t;

  class Channel;
  class EventLoop;
  class EventLoopThread;
  class Poller;
  class TCPConnection;
  class Buffer;
  class SocketAddress;
  
  class HTTPServer;
  class HTTPResponse;
  class HTTPRequest;

  using Callback = std::function<void()>;

  using ChannelPtr = std::shared_ptr<Channel>;
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  using EventLoopThreadPtr = std::shared_ptr<EventLoopThread>;
  using PollerPtr = std::shared_ptr<Poller>;
  using TCPConnectionPtr = std::shared_ptr<TCPConnection>;

} // namespace net

#endif // TYPES_H
