#include "srcs/net/connection/linked_buffer.h"
#include "srcs/net/sys_wrapper/sysw.h"

#include <sys/ioctl.h>

using namespace net;

ssize_t LinkedBuffer::writeFromFD(int fd) {
  int sz_to_read;
  ioctl(fd, FIONREAD, &sz_to_read);
  std::vector<char> buf;

  const ssize_t n = sysw::read(fd, &(*buf.begin()), sz_to_read);
  if (n < 0) return n;

  buffer_.push_back(std::move(buf));
  writer_iter_.nextNode();

  return 0;
}

ssize_t LinkedBuffer::readToFD(int fd) {
  return 0;
}
