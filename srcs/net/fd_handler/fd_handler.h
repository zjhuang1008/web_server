#ifndef FD_HANDLER_H
#define FD_HANDLER_H

#include "srcs/utils/copyable.h"

#include <memory>

namespace net {

class FDHandler : private Copyable {
public:
  FDHandler(const int& fd);
  
  operator int() const { return *fd_ptr_; };
  int get();
private:
  std::shared_ptr<int> fd_ptr_;
};

} // namespace net

#endif // FD_HANDLER_H
