#include "srcs/net/connection/buffer.h"

#include <string>

#include "srcs/net/fd_handler/fd_handler.h"
#include "srcs/net/sys_wrapper/sysw.h"

using namespace net;

static constexpr size_t kTmpBuffSize = 65536;  // 64KB

std::string Buffer::read(size_t len, bool &success) {
  size_t readable_sz = readableSize();
  if (readable_sz < len) {
    success = false;
    return std::string();
  } else if (readable_sz == len) {
    std::string str = std::string(readerIter(), len);
    reader_index_ = kPrependSize;
    writer_index_ = kPrependSize;
    return str;
  } else {
    std::string str = std::string(readerIter(), len);
    reader_index_ += len;
    return str;
  }
}

void Buffer::readAll() {
  reader_index_ = kPrependSize;
  writer_index_ = kPrependSize;
}

ssize_t Buffer::write(FDHandler fd, int &saved_errno) {
  char tmp_buf[kTmpBuffSize];

  size_t writable_sz = writableSize();
  struct iovec vec[2];
  vec[0].iov_base = writerIter();
  vec[0].iov_len = writable_sz;
  vec[1].iov_base = tmp_buf;
  vec[1].iov_len = kTmpBuffSize;
  
  const ssize_t n = sysw::readv(fd, vec, 2);
  if (n < 0) {
    saved_errno = errno;
  } else {
    size_t n_p = static_cast<size_t>(n);
    if (n_p <= writable_sz) {
      writer_index_ += n_p;
    } else {
      writer_index_ += writable_sz;
      append(tmp_buf, n_p - writable_sz);
    }
  }
  
  return n;
}

void Buffer::append(char *tmp_buf, size_t len) {
  size_t reuseable_sz = reader_index_ - kPrependSize;
  if (reuseable_sz >= len) {
    std::copy(readerIter(), writerIter(), prependIter());
    writer_index_ = kPrependSize + readableSize();
    reader_index_ = kPrependSize;
  } else {
    buffer_.resize(writer_index_ + len);
  }

  std::copy(tmp_buf, tmp_buf+len, writerIter()); 
  writer_index_ += len;
}

