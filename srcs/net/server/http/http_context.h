#ifndef HTTP_CONTEXT_H
#define HTTP_CONTEXT_H

#include "srcs/net/types.h"
#include "srcs/utils/copyable.h"
#include "srcs/net/server/http/http_request.h"
#include "srcs/net/connection/buffer/linked_buffer.h"

#include <algorithm>

namespace net {

class HTTPContext : public Copyable {
public:
  enum class HTTPRequestParseState {
    kExpectRequestLine,
    kExpectHeader,
    kExpectBody,
    kGotAll,
  };
  HTTPContext() : state_(HTTPRequestParseState::kExpectRequestLine) {};

  void resetState() { state_ = HTTPRequestParseState::kExpectRequestLine; }
  bool parseFinished() { return state_ == HTTPRequestParseState::kGotAll; }
  const HTTPRequest& request() { return request_; }

  template<typename BufferType>
  bool parseRequest(BufferType& buff);

  template<typename _RandomAccessIterator>
  bool parseOneLine(_RandomAccessIterator start, _RandomAccessIterator end);

  template<typename _RandomAccessIterator>
  bool parseRequestLine(_RandomAccessIterator start, _RandomAccessIterator end);

  template<typename _RandomAccessIterator>
  bool parseHeader(_RandomAccessIterator start, _RandomAccessIterator end);
//  bool parseBody(const char* start, const char* end);
private:
  HTTPRequestParseState state_;
  HTTPRequest request_;
};

template<typename BufferType>
bool HTTPContext::parseRequest(BufferType& buff) {
  for (; state_ != HTTPRequestParseState::kGotAll;) {
    auto&& crlf_iter = std::search(buff.readerIter(), buff.end(), kCRLF, kCRLF+2);
    if (crlf_iter != buff.end()) {
      if (parseOneLine(buff.readerIter(), crlf_iter)) {
        buff.readUntil(crlf_iter + 2);
      } else {
        return false;
      }
    } else {
      // received data is not enough to parse
      break;
    }
  }

  return true;
}

template<typename _RandomAccessIterator>
bool HTTPContext::parseOneLine(_RandomAccessIterator start, _RandomAccessIterator end) {
  switch (state_) {
    case HTTPRequestParseState::kExpectRequestLine:
      if (!parseRequestLine(start, end))
        return false;
      state_ = HTTPRequestParseState::kExpectHeader;
      break;
    case HTTPRequestParseState::kExpectHeader:
      if (start == end) {
        // meet empty line
        state_ = request_.expectBody() ?
                 HTTPRequestParseState::kExpectBody :
                 HTTPRequestParseState::kGotAll;

        return true;
      }
      if (!parseHeader(start, end))
        return false;
      break;
    case HTTPRequestParseState::kExpectBody:
      // TODO: parse body
      state_ = HTTPRequestParseState::kGotAll;
      return true;
//    if (!parseBody(start, end))
//      return false;
//    break;
    case HTTPRequestParseState::kGotAll:
    default:
//    state_ = HTTPRequestParseState::kExpectRequestLine;
      // state_ = HTTPRequestParseState::kGotAll
      // LOG(WARN) << "enter state " << static_cast<int>(state_);
      return true;
  }

  return true;
}

template<typename _RandomAccessIterator>
bool HTTPContext::parseRequestLine(_RandomAccessIterator start, _RandomAccessIterator end) {
  // set method
  auto&& split = std::find(start, end, ' ');
  if (split == end || !request_.setMethod(start, split))
    return false;

  // set URI
  start = split + 1;
  split = std::find(start, end, '?');
  if (split != end) {
    // request has query
    if (!request_.setPath(start, split))
      return false;

    start = split + 1;
    split = std::find(start, end, ' ');
    if (!request_.setQuery(start, split))
      return false;
  } else {
    // request has no query
    split = std::find(start, end, ' ');
    if (!request_.setPath(start, split))
      return false;
  }

  // set version
  start = split + 1;

  return request_.setVersion(start, end);
}

template<typename _RandomAccessIterator>
bool HTTPContext::parseHeader(_RandomAccessIterator start, _RandomAccessIterator end) {
  _RandomAccessIterator colon = std::find(start, end, ':');
  if (colon == end) return false;

  return request_.setHeader(start, colon, end);
}

} // namespace net

#endif // HTTP_CONTEXT_H
