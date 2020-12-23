#ifndef LOG_STREAM_H
#define LOG_STREAM_H

#include "srcs/utils/uncopyable.h"

#include <type_traits>
#include <string.h>

#include <string>

namespace net {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

template<int SIZE>
class FixedBuffer : private Uncopyable {
public:
  FixedBuffer() : curr_(data_) {
    setCookie(cookieStart);    
  };
  ~FixedBuffer() {
    setCookie(cookieEnd);
  }
  
  void append(const char* buf, size_t len) {
    // simply discards content out of the buffer
    if (remain() > len) {
      memcpy(curr_, buf, len);
      curr_ += len;
    }
  }
  char* curr() const { return curr_; }
  void add(size_t len) { curr_ += len; };
  size_t remain() const { return static_cast<size_t>(end() - curr()); }

  const char* data() const { return data_; }
  size_t len() const { return curr() - start(); }

  // used by GDB
  void setCookie(void (*cookie)()) { cookie_ = cookie; }
private:
  const char* start() const { return data_; }
  const char* end() const { return data_ + sizeof(data_); }

  // used for GDB
  void (*cookie_)();
  // must be outlined
  static void cookieStart();
  static void cookieEnd();

  char data_[SIZE];
  char* curr_;
};

class LogStream : private Uncopyable {
public:
  using self = LogStream;
  using Buffer = FixedBuffer<kSmallBuffer>;
  LogStream() = default;
  ~LogStream() = default;

  const Buffer& buffer() const { return buffer_; }

  void append(const char* str, size_t len) {
    buffer_.append(str, len);
  }

  template<typename T>
  void formatInt(T v);
  
  self& operator<<(short);
  self& operator<<(unsigned short);
  self& operator<<(int);
  self& operator<<(unsigned int);
  self& operator<<(long);
  self& operator<<(unsigned long);
  self& operator<<(long long);
  self& operator<<(unsigned long long);

  self& operator<<(float);
  self& operator<<(double);
  
  self& operator<<(char);
  self& operator<<(bool);
  
  self& operator<<(const std::string&);
  self& operator<<(const char*);
  self& operator<<(const unsigned char*);

  self& operator<<(const void*);

private:
  static const int kMaxNumericSize = 32;

  Buffer buffer_;
};

class Fmt : private Uncopyable {
public:
  template<typename T>
  Fmt(const char* fmt, T v);

  const char* buf() const { return buf_; }
  size_t len() const { return len_; }
private:
  char buf_[32];
  size_t len_;
};

template<typename T>
LogStream& operator<<(LogStream& log_stream, const Fmt& fmt);

// used for str that has known length at compile time
class StaticStrHolder {
public:
  StaticStrHolder(const char* str, size_t len) : str_(str), len_(len) {};
  const char* str_;
  size_t len_;
};

LogStream& operator<<(LogStream& log_stream, const StaticStrHolder& str);

class SourceFile {
public:
  template<int N>
  SourceFile(const char (&file)[N]) : data_(file), len_(N-1) {
    const char *slash = strrchr(file, '/');
    if (slash) {
      data_ = slash + 1;
      len_ -= static_cast<int>(data_ - file);
    }
  }

  explicit SourceFile(const char *file) : data_(file) {
    const char *slash = strrchr(file, '/');
    if (slash) {
      data_ = slash + 1;
    }
    len_ = static_cast<int>(strlen(data_));
  }

  const char *data_;
  int len_;
};

LogStream& operator<<(LogStream& log_stream, const SourceFile& file);

} // namespace net

#endif // LOG_STREAM_H
