#ifndef LINKED_BUFFER_H
#define LINKED_BUFFER_H

#include <vector>
#include <deque>
#include <cassert>
#include <iterator>
#include <bits/types/struct_iovec.h>
#include <cstring>

#include "srcs/utils/copyable.h"
#include "srcs/net/types.h"

namespace net {

using CharContainer = std::vector<char>;
using NodeContainer = std::deque<CharContainer>;

class LinkedBufferIter : public std::iterator<std::random_access_iterator_tag, char> {
public:
  explicit LinkedBufferIter()
    : node_iter_(), char_iter_(), node_container_(nullptr) {
  }

  LinkedBufferIter(
    const NodeContainer::iterator& node_iter,
    const CharContainer::iterator& char_iter,
    const NodeContainer* node_container)
    : node_iter_(node_iter), char_iter_(char_iter), node_container_(node_container) {
  }

  LinkedBufferIter(const LinkedBufferIter& rhs)
    : node_iter_(rhs.node_iter_),
      char_iter_(rhs.char_iter_),
      node_container_(rhs.node_container_) {
  }

  LinkedBufferIter& operator=(const LinkedBufferIter& rhs) {
    LinkedBufferIter tmp(rhs);
    swap(tmp);
    return *this;
  }

  void swap(LinkedBufferIter& rhs) {
    std::swap(node_iter_, rhs.node_iter_);
    std::swap(char_iter_, rhs.char_iter_);
    std::swap(node_container_, rhs.node_container_);
  }

  NodeContainer::iterator getNodeIter() const { return node_iter_; }
  CharContainer::iterator getCharIter() const { return char_iter_; }
  void set(const NodeContainer::iterator& node_iter,
           const CharContainer::iterator& char_iter,
           const NodeContainer* node_container) {
    node_iter_ = node_iter;
    char_iter_ = char_iter;
    node_container_ = node_container;
  }

  void toLastCharOfPrevNode();
  void toFirstCharOfNextNode();
//  void toLastCharOfNextNode();

  LinkedBufferIter& operator+=(ptrdiff_t n);
  LinkedBufferIter& operator-=(ptrdiff_t n);
  const LinkedBufferIter operator+(ptrdiff_t n) const;
  const LinkedBufferIter operator-(ptrdiff_t n) const;
  LinkedBufferIter& operator++();
  const LinkedBufferIter operator++(int);
  LinkedBufferIter& operator--();
  const LinkedBufferIter operator--(int);

  ptrdiff_t operator-(const LinkedBufferIter& rhs) const;

  char& operator*() const { return *char_iter_; }
  char* operator->() const { return &(operator*()); }

  bool operator==(const LinkedBufferIter& rhs) const {
    return node_iter_ == rhs.node_iter_ && char_iter_ == rhs.char_iter_;
  }
  bool operator!=(const LinkedBufferIter& rhs) const {
    return !(*this == rhs);
  }
  bool operator<(const LinkedBufferIter& rhs) const {
    return node_iter_ == rhs.node_iter_ ? char_iter_ < rhs.char_iter_ : node_iter_ < rhs.node_iter_;
  }
  bool operator<=(const LinkedBufferIter& rhs) const {
    return *this < rhs || *this == rhs;
  }
  bool operator>(const LinkedBufferIter& rhs) const {
    return node_iter_ == rhs.node_iter_ ? char_iter_ > rhs.char_iter_ : node_iter_ > rhs.node_iter_;
  }
  bool operator>=(const LinkedBufferIter& rhs) const {
    return *this > rhs || *this == rhs;
  }

//  ptrdiff_t nodeDistanceTo(const LinkedBufferIter& rhs) {
//    return rhs.node_iter_ - node_iter_;
//  }
  ptrdiff_t charDistanceToEndOfNode() const {
    return std::distance(char_iter_, node_iter_->end());
  }
  ptrdiff_t charDistanceToStartOfNode() const {
    return std::distance(node_iter_->begin(), char_iter_) + 1;
  }
private:
  NodeContainer::iterator node_iter_;
  CharContainer::iterator char_iter_;
  const NodeContainer* node_container_;
};

//void swap(LinkedBufferIter& lhs, LinkedBufferIter& rhs) {
//  lhs.swap(rhs);
//}

//ptrdiff_t nodeDistance(const LinkedBufferIter& lhs, const LinkedBufferIter& rhs) {
//  return rhs.getNodeIter() - lhs.getNodeIter();
//}

class LinkedBuffer : private Copyable {
public:
  using iterator = LinkedBufferIter;

  explicit LinkedBuffer()
    : buffer_(), reader_iter_() {
  }

  iterator begin() {
    return iterator(buffer_.begin(), buffer_.begin()->begin(), &buffer_);
  }
  iterator end() {
    return iterator(buffer_.end(), CharContainer::iterator(), &buffer_);
  }
  iterator readerIter() const {
    return reader_iter_;
  }

  void append(const char *buf, size_t len);
  void append(const char *str) { append(str, strlen(str)); }
  void append(const std::string& str) { append(str.c_str(), str.size()); }

  ssize_t writeFromFD(int fd);
  ssize_t readToFD(int fd);

  bool read(size_t n);
  bool readUntil(const iterator& iter);
  bool readAll() {
    reader_iter_ = iterator();
    buffer_.clear();
    return true;
  }
  void freeNodesBefore(const NodeContainer::iterator& node_iter);

  size_t readableSize() const;

//  iterator findCRLF();
private:
  // buffer contains a list of node, each node has a char array
  NodeContainer buffer_;

//  iterator begin_iter_;
//  iterator end_iter_;
  iterator reader_iter_;

  std::vector<struct iovec> toIOVec() const;
  size_t numReadableNodes() const { return static_cast<size_t>(buffer_.end() - reader_iter_.getNodeIter()); }
};

} // namespace net

#endif // LINKED_BUFFER_H
