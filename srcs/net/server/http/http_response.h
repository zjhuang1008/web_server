#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"

namespace net {

class HTTPResponse : private Uncopyable {
public:
  HTTPResponse();
private:
};

} // namespace net

#endif // HTTP_RESPONSE_H
