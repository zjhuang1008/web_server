#ifndef SYSW_H
#define SYSW_H

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>


namespace sysw {

inline void log_error(const char* which);

int open(const char* path, int flag);
ssize_t write(int fd, const void* buf, size_t count);
ssize_t read(int fd, void* buf, size_t count);

int eventfd(unsigned int count, int flags);

int epoll_create1(int flags);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *events);
int epoll_wait(int epfd, struct epoll_event *events, int max_events, int timeout);

int socket(int domain, int type, int protocol);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags = 0);
int connect(int clientfd, const struct sockaddr *addr, socklen_t addrlen);
int shutdown(int fd, int how);

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);

}

#endif // SYSW_H
