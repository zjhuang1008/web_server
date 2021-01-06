#include "srcs/net/server/http/http_context.h"

#include <algorithm>

#include "srcs/net/connection/buffer.h"
#include "srcs/logger/logger.h"

using namespace net;

bool HTTPContext::parseRequest(Buffer& buff) {  
  for (;;) {
    const char* crlf_iter = buff.findCRLF();
    if (crlf_iter != buff.writerIter()) {
      if (!parseOneLine(buff.readerIter(), crlf_iter))
        return false;
    } 
    break;
  }

  return true;
}

bool HTTPContext::parseOneLine(const char* start, const char* end) {
  switch (state_) {
  case HTTPRequestParseState::kExpectRequestLine:
    if (!parseRequestLine(start, end)) 
      return false;
    break;
  case HTTPRequestParseState::kExpectHeader:
    if (!parseHeader(start, end)) 
      return false;
    break;
  case HTTPRequestParseState::kExpectBody:
    if (!parseBody(start, end)) 
      return false;
    break;
  case HTTPRequestParseState::kGotAll:
    break;
  default:
    // LOG(WARN) << "enter state " << static_cast<int>(state_);
    break;
  }

  return true;
}

bool HTTPContext::parseRequestLine(const char* start, const char* end) {
  const char* space = std::find(start, end, ' ');
  
  if (space == end) return false;
  if (!request_.setMethod(start, space)) return false;

  return true;
}

bool HTTPContext::parseHeader(const char* start, const char* end) {
  return true;
}

bool HTTPContext::parseBody(const char* start, const char* end) {
  return true;
}