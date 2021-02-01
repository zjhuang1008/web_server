#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <memory>
#include <cstddef>

namespace net {
  using std::size_t;
  using std::ptrdiff_t;

  class Channel;
  class EventLoop;
  class EventLoopThread;
  class Poller;
  class Buffer;
  class LinkedBuffer;
  class SocketAddress;

  template<typename BufferType>
  class HTTPServer;

  class HTTPResponse;
  class HTTPRequest;
  using HTTPResponsePtr = std::shared_ptr<HTTPResponse>;
  using HTTPRequestPtr = std::shared_ptr<HTTPRequest>;

  using Callback = std::function<void()>;

  using ChannelPtr = std::shared_ptr<Channel>;
  using EventLoopPtr = std::shared_ptr<EventLoop>;
  using EventLoopThreadPtr = std::shared_ptr<EventLoopThread>;
  using PollerPtr = std::shared_ptr<Poller>;

  template<typename BufferType>
  class TCPConnection;

  template<typename BufferType>
  using TCPConnectionPtr = std::shared_ptr<TCPConnection<BufferType>>;
} // namespace net

#endif // TYPES_H
