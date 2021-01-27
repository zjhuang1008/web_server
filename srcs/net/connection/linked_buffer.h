#ifndef LINKED_BUFFER_H
#define LINKED_BUFFER_H

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"

#include <vector>

namespace net {

class LinkedBufferIter : private Uncopyable {
public:
  explicit LinkedBufferIter();
  void nextNode() { ++ node_iter; }
  LinkedBufferIter& operator++() {
    if (char_iter + 1 != node_iter->end()) {
      ++ char_iter;
    } else {
      ++ node_iter;
      char_iter = node_iter->begin();
    }
  }
private:
  std::vector<std::vector<char>>::iterator node_iter;
  std::vector<char>::iterator char_iter;
};

class LinkedBuffer : private Uncopyable {
public:
  using Iter = LinkedBufferIter;

  explicit LinkedBuffer();

  ssize_t writeFromFD(int fd);
  ssize_t readToFD(int fd);
private:
  // buffer contains a list of node, each node has a char array
  std::vector<std::vector<char>> buffer_;

  Iter reader_iter_;
  Iter writer_iter_;
  static const char *kCRLF;
};

} // namespace net

#endif // LINKED_BUFFER_H
