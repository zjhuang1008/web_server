#ifndef BUFFER_H
#define BUFFER_H

#include "srcs/utils/copyable.h"
#include "srcs/net/types.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sys/types.h>

#include <string.h>

namespace net {

class FDHandler;

class Buffer : public std::iterator<std::random_access_iterator_tag, char> {
public:
  using iterator = char*;
  static constexpr size_t kPrependSize = 8;
  static constexpr size_t kInitialSize = 1024;

  explicit Buffer(size_t initial_size = kInitialSize)
    : buffer_(kPrependSize + kInitialSize),
      reader_index_(kPrependSize),
      writer_index_(kPrependSize) {}

  char* begin() { return &(*buffer_.begin()); }
  const char* begin() const { return &(*buffer_.begin()); }
  
  char* end() { return begin() + writer_index_; }
  const char* end() const { return begin() + writer_index_; }

  char* readerIter() { return begin() + reader_index_; }
  const char* readerIter() const { return begin() + reader_index_; }

  char* prependIter() { return begin() + kPrependSize; }
  const char* prependIter() const { return begin() + kPrependSize; }

  size_t readableSize() const { return writer_index_ - reader_index_; }
  size_t writableSize() const { return buffer_.size() - writer_index_; }

  // void writerForward(size_t len) { writer_index_ += len; }
  // void readerForward(size_t len) { reader_index_ += len; }
  
  bool read(size_t len);
  bool readUntil(const char* iter);
  bool readAll() {
    reader_index_ = kPrependSize;
    writer_index_ = kPrependSize;
    return true;
  }

  ssize_t writeFromFD(int fd);
  ssize_t readToFD(int fd);

  void append(const char *tmp_buf, size_t len);
  void append(const char *str) { append(str, strlen(str)); }
  void append(const std::string& str) { append(str.c_str(), str.size()); }
private:
  std::vector<char> buffer_;
  size_t reader_index_;
  size_t writer_index_;
};

}; // namespace

#endif // BUFFER_H
