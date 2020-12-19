#ifndef FD_HANDLER_H
#define FD_HANDLER_H

#include <memory>

namespace net {

class FDHandler {
public:
  using FDPtr = std::shared_ptr<int>;
  FDHandler(const int& fd);
  
  operator int() const { return *fd_ptr_; };
  int get();
private:
  FDPtr fd_ptr_;
};

} // namespace net

#endif // FD_HANDLER_H
