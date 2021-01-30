#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <unordered_map>

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"
#include "srcs/net/server/http/http_literal.h"

namespace net {

class HTTPResponse : private Uncopyable {
public:
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

  template<typename T1, typename T2>
  void setHeader(T1&& key, T2&& value) {
    headers_.emplace(std::forward<T1>(key), std::forward<T2>(value));
  }

  template<typename BufferType>
  void appendToBuffer(BufferType& buffer);
private:
  HttpVersion version_;
  HttpStatusCode status_code_;
  bool close_connection_;

  std::unordered_map<std::string, std::string> headers_;
  std::string body_;
  static constexpr int buf_size = 64;
};

template<typename BufferType>
void HTTPResponse::appendToBuffer(BufferType &buffer) {
  char buf[buf_size];
  snprintf(
    buf, buf_size, "%s%s %d %s\r\n",
    gHttpPrefix.c_str(),
    gHttpVersions[static_cast<size_t>(version_)].c_str(),
    gHttpStatusCodes[static_cast<size_t>(status_code_)],
    gHttpStatusMessages[static_cast<size_t>(status_code_)].c_str()
  );

  buffer.append(buf);
  if (close_connection_) {
    buffer.append("Connection: close\r\n");
  } else {
    snprintf(buf, buf_size, "Content-Length: %zd\r\n", body_.size());
    buffer.append(buf);
    buffer.append("Connection: Keep-Alive\r\n");
  }

  for (const auto& header : headers_) {
    buffer.append(header.first);
    buffer.append(": ");
    buffer.append(header.second);
    buffer.append("\r\n");
  }

  buffer.append("\r\n");
  buffer.append(body_);
}


} // namespace net

#endif // HTTP_RESPONSE_H
