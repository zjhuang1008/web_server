#include "srcs/net/server/http/http_request.h"

#include <algorithm>

using namespace net;

const std::vector<std::string> HTTPRequest::kMethods = {"GET", "POST", "HEAD", "PUT", "DELETE"};

bool HTTPRequest::setMethod(const char* start, const char* end) {
  std::string method_str(start, end);
  auto iter = std::find(kMethods.begin(), kMethods.end(), method_str);
  if (iter != kMethods.end()) {
    method_ = Method(iter-kMethods.begin());
    return true;
  } 

  return false;
}

