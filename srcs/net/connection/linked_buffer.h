#ifndef LINKED_BUFFER_H
#define LINKED_BUFFER_H

#include <vector>
#include <deque>
#include <cassert>
#include <bits/types/struct_iovec.h>

#include "srcs/utils/copyable.h"
#include "srcs/net/types.h"

namespace net {

class LinkedBufferIter : public Copyable {
public:
  using CharContainer = std::vector<char>;
  using NodeContainer = std::deque<CharContainer>;

  explicit LinkedBufferIter(
    const NodeContainer& node_container)
    : node_iter_(),
      char_iter_(),
      node_container_(node_container) {
  }

  explicit LinkedBufferIter(
    NodeContainer::iterator node_iter,
    CharContainer::iterator char_iter,
    const NodeContainer& node_container)
    : node_iter_(node_iter),
      char_iter_(char_iter),
      node_container_(node_container) {
  }

  LinkedBufferIter(const LinkedBufferIter& rhs)
    : node_iter_(rhs.node_iter_),
      char_iter_(rhs.char_iter_),
      node_container_(rhs.node_container_) {
  }

  const NodeContainer::iterator& getNodeIter() const { return node_iter_; }
  const CharContainer::iterator& getCharIter() const { return char_iter_; }
  void setIter(const NodeContainer::iterator& node_iter, const CharContainer::iterator& char_iter) {
    node_iter_ = node_iter;
    char_iter_ = char_iter;
  }

  void toLastCharOfPrevNode();
  void toFirstCharOfNextNode();
  void toLastCharOfNextNode();

  LinkedBufferIter& operator+=(ptrdiff_t n);
  LinkedBufferIter& operator-=(ptrdiff_t n);
  const LinkedBufferIter operator+(ptrdiff_t n) const;
  const LinkedBufferIter operator-(ptrdiff_t n) const;
  LinkedBufferIter& operator++();
  const LinkedBufferIter operator++(int);
  LinkedBufferIter& operator--();
  const LinkedBufferIter operator--(int);

//  ptrdiff_t operator-(const LinkedBufferIter& rhs) const;

  char& operator*() const { return *char_iter_; }
  char* operator->() const { return &(operator*()); }

  bool operator==(const LinkedBufferIter& rhs) {
    return node_iter_ == rhs.node_iter_ && char_iter_ == rhs.char_iter_;
  }
  bool operator!=(const LinkedBufferIter& rhs) {
    return !(*this == rhs);
  }
  bool operator<(const LinkedBufferIter& rhs) {
    return node_iter_ == rhs.node_iter_ ? char_iter_ < rhs.char_iter_ : node_iter_ < rhs.node_iter_;
  }

//  ptrdiff_t nodeDistanceTo(const LinkedBufferIter& rhs) {
//    return rhs.node_iter_ - node_iter_;
//  }
  size_t charDistanceToEndOfNode() const {
    return static_cast<size_t>(node_iter_->end() - char_iter_);
  }
private:
  NodeContainer::iterator node_iter_;
  CharContainer::iterator char_iter_;
  const NodeContainer& node_container_;
};

//ptrdiff_t nodeDistance(const LinkedBufferIter& lhs, const LinkedBufferIter& rhs) {
//  return rhs.getNodeIter() - lhs.getNodeIter();
//}

class LinkedBuffer : private Copyable {
public:
  using Iterator = LinkedBufferIter;
  using CharContainer = std::vector<char>;
  using NodeContainer = std::deque<CharContainer>;

  explicit LinkedBuffer()
    : buffer_(),
      reader_iter_(buffer_) {
  }

  void append(CharContainer buf);

  ssize_t writeFromFD(int fd);
  ssize_t readToFD(int fd);

  std::vector<struct iovec> toIOVec();
  size_t numReadableNodes() { return static_cast<size_t>(buffer_.end() - reader_iter_.getNodeIter()); }

  void read(size_t n);
  void readUntil(const Iterator& iter);
private:
  // buffer contains a list of node, each node has a char array
  NodeContainer buffer_;

  Iterator reader_iter_;
  static const char *kCRLF;
};

} // namespace net

#endif // LINKED_BUFFER_H
