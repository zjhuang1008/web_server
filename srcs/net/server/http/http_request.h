#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <string>
#include <vector>

#include "srcs/net/types.h"
#include "srcs/utils/copyable.h"

namespace net {

class HTTPRequest : public Copyable {
public:
  enum class Method {
    kGet, kPost, kHead, kPut, kDelete, kInvalid
  };
  enum class Version {
    kUnknown, kHttp10, kHttp11
  };
  HTTPRequest() : method_(Method::kInvalid), version_(Version::kUnknown) {};

  bool setMethod(const char* start, const char* end);
  void setVersion(const char* start, const char* end);
  void setPath(const char* start, const char* end);
  void setQuery(const char* start, const char* end);
private:
  Method method_;
  Version version_;
  std::string path_;
  std::string query_;

  std::map<std::string, std::string> headers_;
  std::string body_;

  static const std::vector<std::string> kMethods;
};

} // namespace net

#endif // HTTP_REQUEST_H
