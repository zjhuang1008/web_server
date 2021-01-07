#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <unordered_map>

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"
#include "srcs/net/server/http/http_literal.h"

namespace net {

class HTTPResponse : private Uncopyable {
public:
//  enum class HttpStatusCode {
//    k200Ok = 200,
//    k301MovedPermanently = 301,
//    k400BadRequest = 400,
//    k404NotFound = 404,
//    kUnknown,
//  };
//  enum class Version {
//    kHttp10, kHttp11, kUnknown
//  };

  HTTPResponse()
    : version_(HttpVersion::kHttp11),
      status_code_(HttpStatusCode::kUnknown),
      close_connection_(false) {

  };

  void setVersion(const HttpVersion& version) { version_ = version; }
  void setStatusCode(const HttpStatusCode& status_code) { status_code_ = status_code; }

  bool getCloseConnection() const { return close_connection_; }
  void setCloseConnection(bool close) { close_connection_ = close; }

  // use perfect forward, in order to keep efficient when string literal is passed in.
  template<typename T>
  void addHeader(T&& key, T&& value) { headers_[std::forward<T>(key)] = std::forward<T>(value); }

  template<typename T>
  void setBody(T&& body) { body_ = std::forward<T>(body); }

  void toBuffer(Buffer& out_buffer);
private:
  HttpVersion version_;
  HttpStatusCode status_code_;
  bool close_connection_;

  std::unordered_map<std::string, std::string> headers_;
  std::string body_;
};

} // namespace net

#endif // HTTP_RESPONSE_H
