#include <memory>
#include <iostream>

#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/eventfd.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>

#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/net/channel/channel.h"
#include "srcs/net/thread/event_loop.h"
#include "srcs/net/thread/event_loop_thread_pool.h"
#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/logger/logger.h"
#include "srcs/net/types.h"

using namespace net;

void sendMessageToNextThread(EventLoopPtr& loop, net::EventLoopThreadPool& thread_pool) {
  int fd = sysw::open("test", O_RDWR | O_CREAT);
  
  auto io_loop = thread_pool.getNextLoop();
  ChannelPtr ch = std::make_shared<net::Channel>(io_loop, net::FDHandler(fd));
  ch->setReadCallback([ch](){
    char buf[50];
    sysw::read(ch->fd(), buf, sizeof(buf));

    std::cout << "read: " << buf << std::endl;
  });

  // thread_pool.enqueue(ch);
  
  char buf[] = "hello";
  sysw::write(ch->fd(), buf, sizeof(buf));
  // loop_in_thread->addPendingCallbacks();
}

// use eventfd to send message to thread pool
int main(int argc, char* argv[]) {
  int num_threads = 1;
  
  // net::EventLoop::EventLoopPtr loop = std::make_shared<net::EventLoop>();
  net::EventLoopThreadPool thread_pool(num_threads);  
  LOG(INFO) << "begin thread_pool"; 
  // sendMessageToNextThread(loop, thread_pool);
  int fd = sysw::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  
  auto io_loop = thread_pool.getNextLoop();

  ChannelPtr ch = std::make_shared<net::Channel>(io_loop, net::FDHandler(fd));
  ch->setReadCallback([ch](){
    uint64_t one;
    sysw::read(ch->fd(), &one, sizeof(one));

    LOG(DEBUG) << "read: " << one;
  });
  
  io_loop->runInLoop([ch, io_loop](){
    ch->enableReading();
    io_loop->addChannelInPoller(ch);
  });
  // thread_pool.enqueue(ch);
  
  LOG(INFO) << "main thread waked";

  uint64_t one = 1;
  sysw::write(ch->fd(), &one, sizeof(one));
  LOG(INFO) << "write: " << one;

  for (;;);
  // loop->loop();
}