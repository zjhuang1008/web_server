#include "buffer.h"

#include <string>

#include "srcs/net/sys_wrapper/sysw.h"

using namespace net;

static constexpr size_t kTmpBuffSize = 65536;  // 64KB

bool Buffer::read(size_t len) {
  size_t readable_sz = readableSize();
  if (readable_sz < len) {
    return false;
  } else if (readable_sz == len) {
    return readAll();
  } else {
    reader_index_ += len;
    return true;
  }
}

bool Buffer::readUntil(const char *iter) {
  if (iter >= readerIter() && iter <= end()) {
    return read(static_cast<size_t>(iter - readerIter()));
  } else {
    return false;
  }
}


ssize_t Buffer::writeFromFD(int fd) {
  char tmp_buf[kTmpBuffSize];

  size_t writable_sz = writableSize();
  struct iovec vec[2];
  vec[0].iov_base = end();
  vec[0].iov_len = writable_sz;
  vec[1].iov_base = tmp_buf;
  vec[1].iov_len = kTmpBuffSize;
  
  const ssize_t n = sysw::readv(fd, vec, 2);
  if (n <= 0) {
    return n;
  }

  auto n_p = static_cast<size_t>(n);
  if (n_p <= writable_sz) {
    writer_index_ += n_p;
  } else {
    writer_index_ += writable_sz;
    append(tmp_buf, n_p - writable_sz);
  }

  return n;
}


ssize_t Buffer::readToFD(int fd) {
  const ssize_t n = sysw::write(fd, readerIter(), readableSize());
  if (n < 0) {
    return n;
  }

  reader_index_ += static_cast<size_t>(n);

  return n;
}

void Buffer::append(const char *tmp_buf, size_t len) {
  size_t reuseable_sz = reader_index_ - kPrependSize;
  if (reuseable_sz >= len) {
    // copy forward to reserve space to write
    std::copy(readerIter(), end(), prependIter());
    writer_index_ = kPrependSize + readableSize();
    reader_index_ = kPrependSize;
  } else {
    // resize buffer
    buffer_.resize(writer_index_ + len);
  }

  // do append
  std::copy(tmp_buf, tmp_buf+len, end());
  writer_index_ += len;
}

