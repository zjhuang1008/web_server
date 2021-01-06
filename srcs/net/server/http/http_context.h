#ifndef HTTP_CONTEXT_H
#define HTTP_CONTEXT_H

#include "srcs/net/types.h"
#include "srcs/utils/copyable.h"
#include "srcs/net/server/http/http_request.h"

namespace net {

class HTTPContext : public Copyable {
public:
  enum class HTTPRequestParseState {
    kExpectRequestLine,
    kExpectHeader,
    kExpectBody,
    kGotAll,
  };
  HTTPContext() : state_(HTTPRequestParseState::kExpectRequestLine) {};

  bool parseRequest(Buffer& buff);
  bool parseOneLine(const char* start, const char* end);
  
  bool parseRequestLine(const char* start, const char* end);
  bool parseHeader(const char* start, const char* end);
  bool parseBody(const char* start, const char* end);
private:
  HTTPRequestParseState state_;
  HTTPRequest request_;
};

} // namespace net

#endif // HTTP_CONTEXT_H
