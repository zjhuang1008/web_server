#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"

namespace net {

class HTTPRequest : private Uncopyable {
public:
  HTTPRequest();
private:
  
};

} // namespace net

#endif // HTTP_REQUEST_H
