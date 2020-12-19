#include "srcs/net/fd_handler/fd_handler.h"

#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

using namespace std;

void foo(net::FDHandler fd_handler) {
  int count = 50;
  char buffer[count];
  strcpy(buffer, "a dummy test");

  net::FDHandler fd_tmp = fd_handler;
  cout << "In foo: " << fd_tmp.get() << endl;
  if (::write(fd_tmp.get(), buffer, count) < 0)
    cout << strerror(errno) << endl;
}

int main(int argc, char* argv[]) {
  int fd;
  
  {
    net::FDHandler fd_handler = net::FDHandler(::open("test", O_WRONLY | O_CREAT));
    foo(fd_handler);

    cout << "In closure: " << strerror(errno) << endl;
    fd = fd_handler.get();
    cout << fd << endl;
  }
  
  ::close(fd);
  cout << "After closure: " << strerror(errno) << endl;

  return 0;
}
