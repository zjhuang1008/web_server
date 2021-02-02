#include "linked_buffer.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/logger/logger.h"

#include <vector>
#include <sys/ioctl.h>
#include <cstring>
#include <algorithm>

using namespace net;

static constexpr size_t kTmpBuffSize = 65536;  // 64KB
static constexpr size_t kMinNodeSize = 1024;

void LinkedBufferIter::toLastCharOfPrevNode() {
  assert(node_iter_ != node_container_->begin());
  -- node_iter_;
  char_iter_ = std::prev(node_iter_->end());
}

void LinkedBufferIter::toFirstCharOfNextNode() {
  assert(node_iter_ != node_container_->end());

  ++ node_iter_;
  if (node_iter_ != node_container_->end()) {
    char_iter_ = node_iter_->begin();
  } else {
    char_iter_ = CharContainer::iterator();
  }
}

//void LinkedBufferIter::toLastCharOfNextNode() {
//  assert(node_iter_ != node_container_->end());
//  ++ node_iter_;
//  char_iter_ = std::prev(node_iter_->end());
//}

LinkedBufferIter& LinkedBufferIter::operator+=(ptrdiff_t n) {
  ptrdiff_t curr_node_diff;
  if (n >= 0) {
    do {
      curr_node_diff = charDistanceToEndOfNode();
      if (curr_node_diff > n) {
        std::advance(char_iter_, n);
        n = 0;
      } else {
        toFirstCharOfNextNode();
        n -= curr_node_diff;
      }
    } while (n > 0);
  } else {
    do {
      curr_node_diff = charDistanceToStartOfNode();
      if (curr_node_diff > n) {
        std::advance(char_iter_, -n);
        n = 0;
      } else {
        toLastCharOfPrevNode();
        n -= curr_node_diff;
      }
    } while (n > 0);
  }

  return *this;
}

LinkedBufferIter& LinkedBufferIter::operator-=(ptrdiff_t n) {
  return *this += -n;
}

const LinkedBufferIter LinkedBufferIter::operator+(ptrdiff_t n) const {
  LinkedBufferIter tmp = *this;
  return tmp += n;
}

const LinkedBufferIter LinkedBufferIter::operator-(ptrdiff_t n) const {
  LinkedBufferIter tmp = *this;
  return tmp -= n;
}

LinkedBufferIter &LinkedBufferIter::operator++() {
  ++ char_iter_;
  if (char_iter_ == node_iter_->end())
    toFirstCharOfNextNode();

  return *this;
}

const LinkedBufferIter LinkedBufferIter::operator++(int) {
  LinkedBufferIter tmp = *this;
  ++ (*this);
  return tmp;
}

LinkedBufferIter& LinkedBufferIter::operator--() {
  if (char_iter_ != node_iter_->begin()) {
    -- char_iter_;
  } else {
    toLastCharOfPrevNode();
  }
  return *this;
}

const LinkedBufferIter LinkedBufferIter::operator--(int) {
  LinkedBufferIter tmp = *this;
  -- (*this);
  return tmp;
}

ptrdiff_t LinkedBufferIter::operator-(const LinkedBufferIter &rhs) const {
  ptrdiff_t diff = 0;
  if (*this < rhs) {
    LinkedBufferIter iter(*this);
    for (; iter.getNodeIter() != rhs.getNodeIter(); iter.toFirstCharOfNextNode()) {
      diff -= iter.charDistanceToStartOfNode();
    }
    diff -= std::distance(iter.getCharIter(), rhs.getCharIter());
  } else {
    LinkedBufferIter iter(rhs);
    for (; iter.getNodeIter() != this->getNodeIter(); iter.toFirstCharOfNextNode()) {
      diff += iter.charDistanceToEndOfNode();
    }
    diff += std::distance(iter.getCharIter(), this->getCharIter());
  }

  return diff;
}

//ptrdiff_t LinkedBufferIter::operator-(const LinkedBufferIter &rhs) const {
//  ptrdiff_t diff = 0;
//  for (LinkedBufferIter iter(rhs); iter ; ) {
//
//  }
//  return -diff;
//}

// with ioctl
//ssize_t LinkedBuffer::writeFromFD(int fd) {
//  int sz_to_read;
//  ioctl(fd, FIONREAD, &sz_to_read);
//  std::vector<char> buf(static_cast<size_t>(sz_to_read));
//
//  const ssize_t n = sysw::read(fd, &(*buf.begin()), sz_to_read);
//  if (n <= 0) return n;
//
//  append(std::move(buf));
//  return n;
//}

// without ioctl
ssize_t LinkedBuffer::writeFromFD(int fd) {
  char buf[kTmpBuffSize];

  const ssize_t n = sysw::read(fd, buf, kTmpBuffSize);
  if (n <= 0) return n;

  append(buf, static_cast<size_t>(n));
  return n;
}

ssize_t LinkedBuffer::readToFD(int fd) {
  size_t node_d = numReadableNodes();
  struct iovec vec[node_d];

  toIOVec(vec, node_d);
  ssize_t n = sysw::writev(fd, vec, static_cast<int>(node_d));
  if (n < 0) return n;

  read(static_cast<size_t>(n));
  return n;
}

void LinkedBuffer::append(const char *buf, size_t len) {
  if (buffer_.size() == 0) {
    addNewNode(buf, len);
    reader_iter_.set(buffer_.begin(), buffer_.begin()->begin(), &buffer_);
  } else {
    auto&& last_node = std::prev(buffer_.end());
    if (last_node->size() + len < kMinNodeSize) {
      last_node->insert(last_node->end(), buf, buf+len);
    } else {
      addNewNode(buf, len);
    }
  }
}

void LinkedBuffer::addNewNode(const char *buf, size_t len) {
  CharContainer vec;
  // ensure that the capacity of the vector is at least kMinNodeSize
  // so that the insert in the future will not need to reallocate the vector.
  vec.reserve(std::max(kMinNodeSize, len));
  vec.assign(buf, buf + len);
  buffer_.push_back(std::move(vec));
}

//void LinkedBuffer::append(CharContainer&& vec) {
//  if (buffer_.size() == 0) {
//    buffer_.push_back(std::move(vec));
//    reader_iter_.set(buffer_.begin(), buffer_.begin()->begin(), &buffer_);
//  } else {
//    auto&& last_node = std::prev(buffer_.end());
//    if (last_node->size() < kMinNodeSize) {
//      last_node->insert(last_node->end(), vec.begin(), vec.end());
//    } else {
//      buffer_.push_back(std::move(vec));
//    }
//  }
//}

void LinkedBuffer::toIOVec(struct iovec* vec, size_t num) const {
  size_t i = 0;
  for (iterator iter(reader_iter_); iter.getNodeIter() != buffer_.end(); iter.toFirstCharOfNextNode()) {
    vec[i].iov_base = &(*iter);
    vec[i].iov_len = iter.charDistanceToEndOfNode();
    ++ i;
  }
}

bool LinkedBuffer::read(size_t n) {
  if (readableSize() < n) return false;

  reader_iter_ += static_cast<ptrdiff_t>(n);
  freeNodesBefore(reader_iter_.getNodeIter());
  return true;
}

bool LinkedBuffer::readUntil(const LinkedBuffer::iterator &iter) {
  if (iter < readerIter()) return false;

  reader_iter_ = iter;
  freeNodesBefore(reader_iter_.getNodeIter());
  return true;
}

void LinkedBuffer::freeNodesBefore(const NodeContainer::iterator& node_iter) {
  for (; buffer_.begin() != node_iter; ) {
    buffer_.pop_front();
  }
}

size_t LinkedBuffer::readableSize() const {
  size_t sz = 0;
  for (iterator iter(reader_iter_); iter.getNodeIter() != buffer_.end(); iter.toFirstCharOfNextNode()) {
    sz += static_cast<unsigned long>(iter.charDistanceToEndOfNode());
  }
  return sz;
}
