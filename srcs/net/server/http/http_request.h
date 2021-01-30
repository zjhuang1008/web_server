#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <string>
#include <vector>
#include <cassert>

#include "srcs/net/types.h"
#include "srcs/utils/copyable.h"
#include "srcs/net/server/http/http_literal.h"
#include "srcs/net/connection/buffer/linked_buffer.h"

namespace net {

class HTTPRequest : public Copyable {
public:
  HTTPRequest() : method_(HttpMethod::kInvalid), version_(HttpVersion::kUnknown) {};

  template<typename _RandomAccessIterator>
  bool setMethod(_RandomAccessIterator start, _RandomAccessIterator end);

  template<typename _RandomAccessIterator>
  bool setVersion(_RandomAccessIterator start, _RandomAccessIterator end);

  template<typename _RandomAccessIterator>
  bool setPath(_RandomAccessIterator start, _RandomAccessIterator end) {
    path_.assign(start, end);
    return true;
  }

  template<typename _RandomAccessIterator>
  bool setQuery(_RandomAccessIterator start, _RandomAccessIterator end) {
    query_.assign(start, end);
    return true;
  }

  template<typename _RandomAccessIterator>
  bool setHeader(_RandomAccessIterator start, _RandomAccessIterator colon, _RandomAccessIterator end);

  HttpVersion getVersion() const { return version_; }

  template<typename T>
  bool hasHeader(T&& key) const { return headers_.count(std::forward<T>(key)); }

  template<typename T>
  const std::string& getHeader(T&& key) const { return headers_.at(std::forward<T>(key)); }

  const std::string& getPath() const { return path_; }

  bool expectBody() {
    assert(method_ != HttpMethod::kInvalid);
    return method_ == HttpMethod::kPost;
  }
private:
  HttpMethod method_;
  HttpVersion version_;
  std::string path_;
  std::string query_;

  std::map<std::string, std::string> headers_;
  std::string body_;
};

template<typename _RandomAccessIterator>
bool HTTPRequest::setMethod(_RandomAccessIterator start, _RandomAccessIterator end) {
  std::string method_str(start, end);
  auto iter = std::find(gHttpMethods.begin(), gHttpMethods.end(), method_str);
  if (iter != gHttpMethods.end()) {
    method_ = HttpMethod(iter - gHttpMethods.begin());
    return true;
  }

  return false;
}

template<typename _RandomAccessIterator>
bool HTTPRequest::setVersion(_RandomAccessIterator start, _RandomAccessIterator end) {
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

template<typename _RandomAccessIterator>
bool HTTPRequest::setHeader(_RandomAccessIterator start, _RandomAccessIterator colon, _RandomAccessIterator end) {
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

} // namespace net

#endif // HTTP_REQUEST_H
