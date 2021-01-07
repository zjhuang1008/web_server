#include "srcs/net/server/http/http_context.h"

#include <algorithm>

#include "srcs/net/connection/buffer.h"
#include "srcs/logger/logger.h"

using namespace net;

bool HTTPContext::parseRequest(Buffer& buff) {  
  for (; state_ != HTTPRequestParseState::kGotAll;) {
    const char* crlf_iter = buff.findCRLF();
    if (crlf_iter != buff.writerIter()) {
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

bool HTTPContext::parseOneLine(const char* start, const char* end) {
  switch (state_) {
  case HTTPRequestParseState::kExpectRequestLine:
    if (!parseRequestLine(start, end)) 
      return false;
    state_ = HTTPRequestParseState::kExpectHeader;
    break;
  case HTTPRequestParseState::kExpectHeader:
    if (start == end) {
      // meet empty line
      state_ = HTTPRequestParseState::kExpectBody;
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
    break;
  default:
    // state_ = HTTPRequestParseState::kGotAll
    // LOG(WARN) << "enter state " << static_cast<int>(state_);
    return true;
    break;
  }

  return true;
}

bool HTTPContext::parseRequestLine(const char* start, const char* end) {
  // set method
  const char* split = std::find(start, end, ' ');
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

bool HTTPContext::parseHeader(const char* start, const char* end) {
  const char* colon = std::find(start, end, ':');
  if (colon == end) return false;

  return request_.setHeader(start, colon, end);
}

//bool HTTPContext::parseBody(const char* start, const char* end) {
//  return true;
//}