#include "srcs/net/server/http/http_request.h"

#include <algorithm>

using namespace net;

const std::vector<std::string> HTTPRequest::kMethods({"GET", "POST", "HEAD", "PUT", "DELETE"});
const std::string HTTPRequest::kHTTPPrefix("HTTP/");
const std::vector<std::string> HTTPRequest::kHTTPVersions({"1.0", "1.1"});

bool HTTPRequest::setMethod(const char* start, const char* end) {
  std::string method_str(start, end);
  auto iter = std::find(kMethods.begin(), kMethods.end(), method_str);
  if (iter != kMethods.end()) {
    method_ = Method(iter-kMethods.begin());
    return true;
  }

  return false;
}

bool HTTPRequest::setVersion(const char *start, const char *end) {
  auto iter = std::find(start, end, kHTTPPrefix);
  if (iter == end) return false;

  std::string http_version(iter + kHTTPPrefix.size(), end);
  auto iter_v = std::find(kHTTPVersions.begin(), kHTTPVersions.end(), http_version);
  if (iter_v != kHTTPVersions.end()) {
    version_ = Version(iter_v-kHTTPVersions.begin());
    return true;
  }

  return false;
}

