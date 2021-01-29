#include "srcs/net/connection/buffer/linked_buffer.h"
#include <iostream>
#include <algorithm>
#include <srcs/net/server/http/http_literal.h>

using namespace net;
using namespace std;

int main() {
  LinkedBuffer lb;
  lb.append("hll\r");
  lb.append("\n");
  lb.append("sssss");

  auto iter = std::search(lb.readerIter(), lb.end(), kCRLF, kCRLF+2);

  cout << (iter - lb.begin()) << endl;
  cout << (iter == lb.end()) << endl;

  cout << lb.readableSize() << endl;
  for (auto&& c : lb)
    cout << c << " ";
  cout << endl;

  lb.read(2);

  cout << lb.readableSize() << endl;
  for (auto&& c : lb)
    cout << c << " ";
  cout << endl;

  lb.read(2);

  cout << lb.readableSize() << endl;
  for (auto&& c : lb)
    cout << c << " ";
  cout << endl;

  return 0;
}