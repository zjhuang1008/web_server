#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <string>
#include <vector>
#include <cassert>

#include "srcs/net/types.h"
#include "srcs/utils/copyable.h"
#include "srcs/net/server/http/http_literal.h"

namespace net {

class HTTPRequest : public Copyable {
public:
//  enum class Method {
//    kGet, kPost, kHead, kPut, kDelete, kInvalid
//  };
//  enum class Version {
//    kHttp10, kHttp11, kUnknown
//  };
  HTTPRequest() : method_(HttpMethod::kInvalid), version_(HttpVersion::kUnknown) {};

  bool setMethod(const char* start, const char* end);
  bool setVersion(const char* start, const char* end);
  bool setPath(const char* start, const char* end) {
    path_.assign(start, end);
    return true;
  }
  bool setQuery(const char* start, const char* end) {
    query_.assign(start, end);
    return true;
  }
  bool setHeader(const char* start, const char* colon, const char* end);

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

//  static const std::vector<std::string> gHttpMethods;
//  static const std::string gHttpPrefix;
//  static const std::vector<std::string> gHttpVersions;
};

} // namespace net

#endif // HTTP_REQUEST_H
