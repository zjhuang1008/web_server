#include "srcs/net/sys_wrapper/sysw.h"

#include <unistd.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


void sysw::log_error_and_abort() {
  // LOGERR << "errno: " << errno << " " << strerror(errno) << std::endl;
  printf("errno: %s\n", strerror(errno));
  std::abort();
}

int sysw::open(const char* path, int flag) {
  int fd = ::open(path, flag);
  if (fd < 0) sysw::log_error_and_abort();
  return fd;
}

ssize_t sysw::write(int fd, const void* buf, size_t count) {
  ssize_t n = ::write(fd, buf, count);
  if (n < 0) sysw::log_error_and_abort();
  return n;
}

ssize_t sysw::read(int fd, void* buf, size_t count) {
  ssize_t n = ::read(fd, buf, count);
  if (n < 0) sysw::log_error_and_abort();
  return n;
}

int sysw::eventfd(unsigned int count, int flags) {
  int fd = ::eventfd(count, flags);
  if (fd < 0) sysw::log_error_and_abort();
  return fd;
}

int sysw::epoll_create1(int flags) {
  int epfd = ::epoll_create1(flags);
  if (epfd < 0) sysw::log_error_and_abort();
  return epfd;
}

int sysw::epoll_ctl(int epfd, int op, int fd, struct epoll_event *events) {
  int ok = ::epoll_ctl(epfd, op, fd, events);
  if (ok < 0) sysw::log_error_and_abort();
  return ok;
}

int sysw::epoll_wait(int epfd, struct epoll_event *events, int max_events, int timeout) {
  int num_events = ::epoll_wait(epfd, events, max_events, timeout);
  if (num_events < 0 && errno != EINTR) sysw::log_error_and_abort();
  return num_events;
}
