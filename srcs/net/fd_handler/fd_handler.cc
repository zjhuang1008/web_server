#include "srcs/net/fd_handler/fd_handler.h"

#include <unistd.h>

#include "srcs/logger/logger.h"

using namespace net;

struct FDDeleter {
  void operator()(int* fd_ptr) const {
    ::close(*fd_ptr);
    delete fd_ptr;
  }
};

FDHandler::FDHandler(const int& fd) : fd_ptr_(new int(fd), FDDeleter()) {

};

int FDHandler::get() {
  return *fd_ptr_;
}
