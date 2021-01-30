#include "linked_buffer.h"
#include "srcs/net/sys_wrapper/sysw.h"
#include "srcs/logger/logger.h"

#include <vector>
#include <sys/ioctl.h>
#include <cstring>
#include <algorithm>

using namespace net;

//const char* LinkedBuffer::kCRLF = "\r\n";

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

ssize_t LinkedBuffer::writeFromFD(int fd) {
  int sz_to_read;
  ioctl(fd, FIONREAD, &sz_to_read);
  char buf[sz_to_read];

  const ssize_t n = sysw::read(fd, buf, sz_to_read);
  if (n <= 0) return n;

  append(buf);
  return 0;
}

ssize_t LinkedBuffer::readToFD(int fd) {
  std::vector<struct iovec> vec = toIOVec();
  ssize_t n = sysw::writev(fd, &(*vec.begin()), static_cast<int>(vec.size()));
  if (n < 0) return n;

  read(static_cast<size_t>(n));
  return 0;
}

void LinkedBuffer::append(const char *buf, size_t len) {
  buffer_.emplace_back(buf, buf+len);
  if (buffer_.size() == 1) {
    reader_iter_.set(buffer_.begin(), buffer_.begin()->begin(), &buffer_);
  }
}

std::vector<struct iovec> LinkedBuffer::toIOVec() const {
  size_t node_d = numReadableNodes();
  std::vector<struct iovec> vec;
  vec.reserve(node_d);

  for (iterator iter(reader_iter_); iter.getNodeIter() != buffer_.end(); iter.toFirstCharOfNextNode()) {
    struct iovec v;
    v.iov_base = &(*iter);
    v.iov_len = iter.charDistanceToEndOfNode();
    vec.push_back(v);
  }
  return vec;
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
