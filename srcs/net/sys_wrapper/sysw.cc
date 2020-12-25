#include "srcs/net/sys_wrapper/sysw.h"

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "srcs/logger/logger.h"

using namespace net;

void sysw::log_error_and_abort(const char* which) {
  // LOG(FATAL) << "errno: " << errno << " " << strerror(errno);
  LOGSYS(FATAL) << which << " error.";
  std::abort();
}

int sysw::open(const char* path, int flag) {
  int fd = ::open(path, flag);
  if (fd < 0) sysw::log_error_and_abort("open");
  return fd;
}

ssize_t sysw::write(int fd, const void* buf, size_t count) {
  ssize_t n = ::write(fd, buf, count);
  if (n < 0) sysw::log_error_and_abort("write");
  return n;
}

ssize_t sysw::read(int fd, void* buf, size_t count) {
  ssize_t n = ::read(fd, buf, count);
  if (n < 0) sysw::log_error_and_abort("read");
  return n;
}

int sysw::eventfd(unsigned int count, int flags) {
  int fd = ::eventfd(count, flags);
  if (fd < 0) sysw::log_error_and_abort("eventfd");
  return fd;
}

int sysw::epoll_create1(int flags) {
  int epfd = ::epoll_create1(flags);
  if (epfd < 0) sysw::log_error_and_abort("epoll_create1");
  return epfd;
}

int sysw::epoll_ctl(int epfd, int op, int fd, struct epoll_event *events) {
  int ok = ::epoll_ctl(epfd, op, fd, events);
  if (ok < 0) sysw::log_error_and_abort("epoll_ctl");
  return ok;
}

int sysw::epoll_wait(int epfd, struct epoll_event *events, int max_events, int timeout) {
  int num_events = ::epoll_wait(epfd, events, max_events, timeout);
  if (num_events < 0 && errno != EINTR) sysw::log_error_and_abort("epoll_wait");
  return num_events;
}

int sysw::socket(int domain, int type, int protocol) {
  int fd = ::socket(domain, type, protocol);
  if (fd < 0) sysw::log_error_and_abort("socket");
  return fd;
}

int sysw::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  int ok = ::bind(sockfd, addr, addrlen);
  if (ok < 0) sysw::log_error_and_abort("bind");
  return ok;
}

int sysw::listen(int sockfd, int backlog) {
  int ok = ::listen(sockfd, backlog);
  if (ok < 0) sysw::log_error_and_abort("listen");
  return ok;
}

int sysw::accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
  int fd = ::accept4(sockfd, addr, addrlen, flags);
  if (fd < 0) sysw::log_error_and_abort("accept");
  return fd;
}

int sysw::connect(int clientfd, const struct sockaddr *addr, socklen_t addrlen) {
  int ok = ::connect(clientfd, addr, addrlen);
  if (ok < 0) sysw::log_error_and_abort("connect");
  return ok;
}
