#include "srcs/net/server/http/http_literal.h"

namespace net {

const std::vector<std::string> gHttpMethods({"GET", "POST", "HEAD", "PUT", "DELETE"});
const std::string gHttpPrefix("HTTP/");
const std::vector<std::string> gHttpVersions({"1.0", "1.1"});
const std::vector<int> gHttpStatusCodes({
  200, 301, 400, 404
});
const std::vector<std::string> gHttpStatusMessages({
  "OK", "Moved Permanently", "Bad Request", "Not Found"
});

} // namespace net
