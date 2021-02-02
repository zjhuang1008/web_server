#ifndef WEB_SERVER_HTTP_LITERAL_H
#define WEB_SERVER_HTTP_LITERAL_H

#include <vector>
#include <string>

namespace net {

enum class HttpVersion {
  kHttp10, kHttp11, kUnknown
};

enum class HttpMethod {
  kGet, kPost, kHead, kPut, kDelete, kInvalid
};

enum class HttpStatusCode {
  k200Ok,
  k301MovedPermanently,
  k400BadRequest,
  k404NotFound,
  kUnknown,
};

extern const std::vector<std::string> gHttpMethods;
extern const std::string gHttpPrefix;
extern const std::vector<std::string> gHttpVersions;
extern const std::vector<int> gHttpStatusCodes;
extern const std::vector<std::string> gHttpStatusMessages;

extern const char *kCRLF;
} // namespace net



#endif //WEB_SERVER_HTTP_LITERAL_H
