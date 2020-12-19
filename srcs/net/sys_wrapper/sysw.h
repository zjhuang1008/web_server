#ifndef SYS_H
#define SYS_H

#include <unistd.h>
#include <sys/epoll.h>


namespace sysw {

void log_error_and_abort();

int open(const char* path, int flag);
ssize_t write(int fd, const void* buf, size_t count);
ssize_t read(int fd, void* buf, size_t count);

int eventfd(unsigned int count, int flags);

int epoll_create1(int flags);
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *events);
int epoll_wait(int epfd, struct epoll_event *events, int max_events, int timeout);
}

#endif // SYS_H
