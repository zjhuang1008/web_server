#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <boost/functional/hash.hpp>

#include "srcs/net/types.h"
#include "srcs/utils/copyable.h"
#include "srcs/net/server/http/http_literal.h"
#include "srcs/net/connection/buffer/linked_buffer.h"

namespace net {

class HTTPRequest : public Copyable {
public:
  HTTPRequest() : method_(HttpMethod::kInvalid), version_(HttpVersion::kUnknown) {};

//  bool operator==(const HTTPRequest& rhs) const {
//    return method_ == rhs.method_ && version_ == rhs.version_ &&
//           path_ == rhs.path_ && query_ == rhs.query_ &&
//           headers_ == rhs.headers_ && body_ == rhs.body_;
//  }

  size_t to_hash_value() const {
    using boost::hash_value;
    using boost::hash_combine;

    size_t seed = 0;

    hash_combine(seed, hash_value(method_));
    hash_combine(seed, hash_value(version_));
    hash_combine(seed, hash_value(path_));
    hash_combine(seed, hash_value(query_));
    hash_combine(seed, hash_value(headers_));
    hash_combine(seed, hash_value(body_));

    return seed;
  }

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

struct HTTPRequstPtrHash {
  size_t operator()(const HTTPRequestPtr& p) const {
    return p->to_hash_value();
  }
};

struct HTTPRequstPtrEqualTo {
  size_t operator()(const HTTPRequestPtr& x, const HTTPRequestPtr& y) const {
    return x->to_hash_value() == y->to_hash_value();
  }
};

} // namespace net

namespace std {
  template<>
  struct hash<net::HTTPRequest> {
    size_t operator()(const net::HTTPRequest& req) const {
      return req.to_hash_value();
    }
  };
};

#endif // HTTP_REQUEST_H
