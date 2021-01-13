#include "srcs/net/sys_wrapper/sysw.h"

// #include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/eventfd.h>
// #include <sys/socket.h>
// #include <sys/epoll.h>
// #include <sys/types.h>

#include "srcs/logger/logger.h"

using namespace net;

void sysw::log_error(const char* which) {
  // LOG(FATAL) << "errno: " << errno << " " << strerror(errno);
  LOGSYS(ERROR) << which << " error.";
  // std::abort();
}

int sysw::open(const char* path, int flag) {
  int fd = ::open(path, flag);
  if (fd < 0) sysw::log_error("open");
  return fd;
}

ssize_t sysw::write(int fd, const void* buf, size_t count) {
  ssize_t n = ::write(fd, buf, count);
  if (n < 0) sysw::log_error("write");
  return n;
}

ssize_t sysw::read(int fd, void* buf, size_t count) {
  ssize_t n = ::read(fd, buf, count);
  if (n < 0) sysw::log_error("read");
  return n;
}

int sysw::eventfd(unsigned int count, int flags) {
  int fd = ::eventfd(count, flags);
  if (fd < 0) sysw::log_error("eventfd");
  return fd;
}

int sysw::epoll_create1(int flags) {
  int epfd = ::epoll_create1(flags);
  if (epfd < 0) sysw::log_error("epoll_create1");
  return epfd;
}

int sysw::epoll_ctl(int epfd, int op, int fd, struct epoll_event *events) {
  int ok = ::epoll_ctl(epfd, op, fd, events);
  if (ok < 0) sysw::log_error("epoll_ctl");
  return ok;
}

int sysw::epoll_wait(int epfd, struct epoll_event *events, int max_events, int timeout) {
  int num_events = ::epoll_wait(epfd, events, max_events, timeout);
  if (num_events < 0 && errno != EINTR) sysw::log_error("epoll_wait");
  return num_events;
}

int sysw::socket(int domain, int type, int protocol) {
  int fd = ::socket(domain, type, protocol);
  if (fd < 0) sysw::log_error("socket");
  return fd;
}

int sysw::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  int ok = ::bind(sockfd, addr, addrlen);
  if (ok < 0) sysw::log_error("bind");
  return ok;
}

int sysw::listen(int sockfd, int backlog) {
  int ok = ::listen(sockfd, backlog);
  if (ok < 0) sysw::log_error("listen");
  return ok;
}

int sysw::accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
  int fd = ::accept4(sockfd, addr, addrlen, flags);
  if (fd < 0) sysw::log_error("accept");
  return fd;
}

int sysw::connect(int clientfd, const struct sockaddr *addr, socklen_t addrlen) {
  int ok = ::connect(clientfd, addr, addrlen);
  if (ok < 0) sysw::log_error("connect");
  return ok;
}

ssize_t sysw::readv(int fd, const struct iovec *iov, int iovcnt) {
  ssize_t n = ::readv(fd, iov, iovcnt);
  if (n < 0) sysw::log_error("readv");
  return n;
}

int sysw::shutdown(int fd, int how) {
  int ok = ::shutdown(fd, how);
  if (ok < 0 && errno != 107) sysw::log_error("shutdown");
  return ok;
}

int sysw::setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen) {
  int ok = ::setsockopt(sockfd, level, optname, optval, optlen);
  if (ok < 0) sysw::log_error("setsockopt");
  return ok;
}
