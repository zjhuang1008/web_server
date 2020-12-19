#include <memory>
#include <iostream>

#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/channel/channel.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/net/thread/event_loop_thread_pool.h"
#include "srcs/net/fd_handler/fd_handler.h"


void sendMessageToNextThread(net::EventLoop::EventLoopPtr& loop, net::EventLoopThreadPool& thread_pool) {
  int fd = sysw::open("test", O_RDWR | O_CREAT);
  
  net::Channel::ChannelPtr ch = std::make_shared<net::Channel>(net::FDHandler(fd));
  ch->setReadCallback([ch](){
    char buf[50];
    sysw::read(ch->fd(), buf, sizeof(buf));

    std::cout << "read: " << buf << std::endl;
  });

  thread_pool.enqueue(ch);
  
  char buf[] = "hello";
  sysw::write(ch->fd(), buf, sizeof(buf));
  // loop_in_thread->addPendingCallbacks();
}

// use eventfd to send message to thread pool
int main(int argc, char* argv[]) {
  int num_threads = 1;
  
  net::EventLoop::EventLoopPtr loop = std::make_shared<net::EventLoop>();
  net::EventLoopThreadPool thread_pool(num_threads);  
  
  // sendMessageToNextThread(loop, thread_pool);
  int fd = sysw::open("/home/devin/works/web_server/test", O_RDWR | O_CREAT);
  
  net::Channel::ChannelPtr ch = std::make_shared<net::Channel>(net::FDHandler(fd));
  ch->setReadCallback([ch](){
    char buf[50];
    sysw::read(ch->fd(), buf, sizeof(buf));

    std::cout << "read: " << buf << std::endl;
  });

  thread_pool.enqueue(ch);
  
  char buf[] = "hello";
  sysw::write(ch->fd(), buf, sizeof(buf));
  
  loop->loop();
}