#include "srcs/net/connection/linked_buffer.h"
#include "srcs/net/sys_wrapper/sysw.h"

#include <sys/ioctl.h>

using namespace net;

#include <sys/ioctl.h>
#include <vector>
#include <cstring>

#include "srcs/net/sys_wrapper/sysw.h"


using namespace net;

void LinkedBufferIter::toLastCharOfPrevNode() {
  assert(node_iter_ != node_container_.begin());
  -- node_iter_;
  char_iter_ = std::prev(node_iter_->end());
}

void LinkedBufferIter::toFirstCharOfNextNode() {
  assert(node_iter_ != node_container_.end());
  ++ node_iter_;
  char_iter_ = node_iter_->begin();
}

void LinkedBufferIter::toLastCharOfNextNode() {
  assert(node_iter_ != node_container_.end());
  ++ node_iter_;
  char_iter_ = std::prev(node_iter_->end());
}

LinkedBufferIter& LinkedBufferIter::operator+=(ptrdiff_t n) {
  ptrdiff_t curr_node_diff;
  if (n >= 0) {
    do {
      curr_node_diff = std::distance(char_iter_, node_iter_->end());
      if (curr_node_diff >= n) {
        std::advance(char_iter_, n);
        n = 0;
      } else {
        toFirstCharOfNextNode();
        n -= curr_node_diff;
      }
    } while (n > 0);
  } else {
    do {
      curr_node_diff = std::distance(node_iter_->begin(), char_iter_) + 1;
      if (curr_node_diff >= n) {
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
  CharContainer buf;

  const ssize_t n = sysw::read(fd, &(*buf.begin()), sz_to_read);
  if (n <= 0) return n;

  append(std::move(buf));
  return 0;
}

ssize_t LinkedBuffer::readToFD(int fd) {
  std::vector<struct iovec> vec = toIOVec();
  ssize_t n = sysw::writev(fd, &(*vec.begin()), static_cast<int>(vec.size()));
  if (n <= 0) return n;

  read(static_cast<size_t>(n));
  return 0;
}

void LinkedBuffer::append(LinkedBuffer::CharContainer buf) {
  buffer_.push_back(std::move(buf));
  if (buffer_.size() == 1) {
    reader_iter_.setIter(buffer_.begin(), buffer_.begin()->begin());
  }
}

std::vector<struct iovec> LinkedBuffer::toIOVec() {
  size_t node_d = numReadableNodes();
  std::vector<struct iovec> vec(node_d);

  size_t i = 0;
  for (Iterator iter(reader_iter_); iter.getNodeIter() != buffer_.end(); iter.toFirstCharOfNextNode(), ++ i) {
    vec[i].iov_base = &(*iter);
    vec[i].iov_len = iter.charDistanceToEndOfNode();
  }
  return vec;
}

void LinkedBuffer::read(size_t n) {
  auto prev_node_iter = reader_iter_.getNodeIter();
  reader_iter_ += static_cast<ptrdiff_t>(n);
  auto curr_node_iter = reader_iter_.getNodeIter();

  // release node
}

void LinkedBuffer::readUntil(const LinkedBuffer::Iterator &iter) {

}
