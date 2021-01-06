#ifndef BUFFER_H
#define BUFFER_H

#include "srcs/utils/uncopyable.h"
#include "srcs/net/types.h"

#include <vector>
#include <string>
#include <algorithm>
#include <sys/types.h>

namespace net {

class FDHandler;

class Buffer : private Uncopyable {
public:
  static constexpr size_t kPrependSize = 8;
  static constexpr size_t kInitialSize = 1024;

  explicit Buffer(size_t initial_size = kInitialSize)
    : buffer_(kPrependSize + kInitialSize),
      reader_index_(kPrependSize),
      writer_index_(kPrependSize) {}

  char* beginIter() { return &(*buffer_.begin()); }
  const char* beginIter() const { return &(*buffer_.begin()); }

  char* readerIter() { return beginIter() + reader_index_; }
  const char* readerIter() const { return beginIter() + reader_index_; }
  
  char* writerIter() { return beginIter() + writer_index_; }
  const char* writerIter() const { return beginIter() + writer_index_; }

  char* prependIter() { return beginIter() + kPrependSize; }
  const char* prependIter() const { return beginIter() + kPrependSize; }

  size_t readableSize() { return writer_index_ - reader_index_; }
  size_t writableSize() { return buffer_.size() - writer_index_; }

  // void writerForward(size_t len) { writer_index_ += len; }
  // void readerForward(size_t len) { reader_index_ += len; }
  
  std::string read(size_t len, bool &success);
  void readAll() {
    reader_index_ = kPrependSize;
    writer_index_ = kPrependSize;
  }

  ssize_t write(const FDHandler& fd);

  void append(char *tmp_buf, size_t len);

  const char* findCRLF() {
    return std::search(readerIter(), writerIter(), kCRLF, kCRLF+2);
  }
private:
  std::vector<char> buffer_;
  size_t reader_index_;
  size_t writer_index_;

  static const char *kCRLF;
};

}; // namespace

#endif // BUFFER_H
