#ifndef LOG_STREAM_H
#define LOG_STREAM_H

#include "srcs/utils/uncopyable.h"

#include <type_traits>
#include <string.h>

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
    if (remain() < len) {
      memcpy(curr_, buf, len);
      curr_ += len;
    }
  }
  char* curr() { return curr_; }
  void add(size_t len) { curr_ += len; };
  // used by GDB
  void setCookie(void (*cookie)()) { cookie_ = cookie; }

private:
  const char* start() { return data_; }
  const char* end() { return data_ + sizeof(data_); }
  size_t remain() { return static_cast<size_t>(end() - curr_); }

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

  template<typename T>
  void formatInt(T v);

  // self& operator<<(float);
  // self& operator<<(double);
  
  enum class OutputTypes { INTEGER, FLOAT, POINTER, STRING };

  template<typename T>
  self& operator<<(T&&);

  template<typename T, OutputTypes which>
  self& outputImplSelect(T&& v);

  // short, long, int, long long (and their unsigned version)
  template<typename T>
  self& outputImpl(T&&, std::integral_constant<OutputTypes, OutputTypes::INTEGER>);

  // float, double
  template<typename T>
  self& outputImpl(T&&, std::integral_constant<OutputTypes, OutputTypes::FLOAT>);

  // pointer
  template<typename T>
  self& outputImpl(T&&, std::integral_constant<OutputTypes, OutputTypes::POINTER>);

  // std::string
  template<typename T>
  self& outputImpl(T&&, std::integral_constant<OutputTypes, OutputTypes::STRING>);
  
  
  
  // self& operator<<(short);
  // self& operator<<(unsigned short);
  // self& operator<<(long long);
  // self& operator<<(unsigned long long);
  // self& operator<<(long);
  // self& operator<<(unsigned long);
  // self& operator<<(int);
  // self& operator<<(unsigned int);
  
private:
  static const int kMaxNumericSize = 32;

  Buffer buffer_;
};

}

#endif // LOG_STREAM_H
