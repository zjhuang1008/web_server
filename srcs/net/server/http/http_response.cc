#include "srcs/net/server/http/http_response.h"

#include "srcs/net/connection/buffer.h"

using namespace net;

static constexpr int buf_size = 64;

void HTTPResponse::toBuffer(Buffer &out_buffer) {
  char buf[buf_size];
  snprintf(
    buf, buf_size, "%s%s %d %s\r\n",
    gHttpPrefix.c_str(),
    gHttpVersions[static_cast<size_t>(version_)].c_str(),
    gHttpStatusCodes[static_cast<size_t>(status_code_)],
    gHttpStatusMessages[static_cast<size_t>(status_code_)].c_str()
  );

  out_buffer.append(buf);
  if (close_connection_) {
    out_buffer.append("Connection: close\r\n");
  } else {
    snprintf(buf, buf_size, "Content-Length: %zd\r\n", body_.size());
    out_buffer.append(buf);
    out_buffer.append("Connection: Keep-Alive\r\n");
  }

  for (const auto& header : headers_) {
    out_buffer.append(header.first);
    out_buffer.append(": ");
    out_buffer.append(header.second);
    out_buffer.append("\r\n");
  }

  out_buffer.append("\r\n");
  out_buffer.append(body_);
}
