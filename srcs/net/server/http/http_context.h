#ifndef HTTP_CONTEXT_H
#define HTTP_CONTEXT_H

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"

namespace net {

class HTTPContext : private Uncopyable {
public:
  HTTPContext();
private:
};

} // namespace net

#endif // HTTP_CONTEXT_H
