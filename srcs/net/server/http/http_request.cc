#include "srcs/net/server/http/http_request.h"

#include <algorithm>

using namespace net;

//const std::vector<std::string> HTTPRequest::gHttpMethods({"GET", "POST", "HEAD", "PUT", "DELETE"});
//const std::string HTTPRequest::gHttpPrefix("HTTP/");
//const std::vector<std::string> HTTPRequest::gHttpVersions({"1.0", "1.1"});

bool HTTPRequest::setMethod(const char* start, const char* end) {
  std::string method_str(start, end);
  auto iter = std::find(gHttpMethods.begin(), gHttpMethods.end(), method_str);
  if (iter != gHttpMethods.end()) {
    method_ = HttpMethod(iter - gHttpMethods.begin());
    return true;
  }

  return false;
}

bool HTTPRequest::setVersion(const char *start, const char *end) {
  auto iter = std::search(start, end, gHttpPrefix.begin(), gHttpPrefix.end());
  if (iter == end) return false;

  std::string http_version(iter + gHttpPrefix.size(), end);
  auto iter_v = std::find(gHttpVersions.begin(), gHttpVersions.end(), http_version);
  if (iter_v != gHttpVersions.end()) {
    version_ = HttpVersion(iter_v - gHttpVersions.begin());
    return true;
  }

  return false;
}

bool HTTPRequest::setHeader(const char *start, const char *colon, const char *end) {
  std::string key(start, colon);

  ++ colon;
  // remove prefix space
  for (; *colon == ' '; ++ colon);
  // remove postfix space
  for (; *end == ' '; -- end);

  std::string value(colon, end);

  headers_.emplace(std::move(key), std::move(value));

  return true;
}

