#include "srcs/logger/log_stream.h"

#include <utility>
#include <algorithm>
#include <type_traits>
#include <string>
#include <stdexcept>

#include <assert.h>

using namespace net;

namespace net {

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof digits == 20, "wrong number of digits");

const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof digitsHex == 17, "wrong number of digitsHex");

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value)
{
  T i = value;
  char* p = buf;

  do
  {
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while (i != 0);

  if (value < 0)
  {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
  uintptr_t i = value;
  char* p = buf;

  do
  {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

} // namespace net

// FixedBuffer implementation
template<int SIZE>
void FixedBuffer<SIZE>::cookieStart() {
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd() {
}

// LogStream implementation
template<typename T>
void LogStream::formatInt(T v) {
  if (buffer_.remain() >= kMaxNumericSize) {
    size_t len = convert(buffer_.curr(), v);
    buffer_.add(len);
  }
}

LogStream& LogStream::operator<<(short v) {
  *this << static_cast<int>(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v) {
  formatInt(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
  formatInt(v);
  return *this;
}

LogStream& LogStream::operator<<(long v) {
  formatInt(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
  formatInt(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v) {
  formatInt(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
  formatInt(v);
  return *this;
}

LogStream& LogStream::operator<<(float v) {
  *this << static_cast<double>(v);
  return *this;
}

LogStream& LogStream::operator<<(double v) {
  if (buffer_.remain() >= kMaxNumericSize) {
    int len = snprintf(buffer_.curr(), kMaxNumericSize, "%.12g", v);
    buffer_.add(static_cast<size_t>(len));
  }
  return *this;
}

LogStream& LogStream::operator<<(char v) {
  if (buffer_.remain() >= 1) {
    buffer_.append(&v, 1);
  }
  return *this;
}

LogStream& LogStream::operator<<(bool v) {
  buffer_.append(v ? "1" : "0", 1);
  return *this;
}

LogStream& LogStream::operator<<(const std::string& v) {
  buffer_.append(v.c_str(), v.size());
  return *this;
}

LogStream& LogStream::operator<<(const char* str) {
  if (str) {
    buffer_.append(str, strlen(str));
  } else {
    buffer_.append("(null)", 6);
  }
  return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str) {
  *this << reinterpret_cast<const char*>(str);
  return *this;
}

LogStream& LogStream::operator<<(const void* p) {
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  if (buffer_.remain() >= kMaxNumericSize)
  {
    char* buf = buffer_.curr();
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = convertHex(buf+2, v);
    buffer_.add(len+2);
  }
  return *this;
}

LogStream& LogStream::operator<<(const Fmt& fmt) {
  buffer_.append(fmt.buf(), fmt.len());
  return *this;
}

LogStream& LogStream::operator<<(const StaticStrHolder& str) {
  buffer_.append(str.str_, str.len_);
  return *this;
}

LogStream& LogStream::operator<<(const SourceFile& file) {
  buffer_.append(file.data_, file.len_);
  return *this;
}

template<typename T>
Fmt::Fmt(const char* fmt, T v) {
  static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");

  len_ = static_cast<size_t>(snprintf(buf_, sizeof buf_, fmt, v));
  assert(len_ < sizeof buf_);
}

// LogStream& operator<<(LogStream& log_stream, const Fmt& fmt) {
//   log_stream.append(fmt.buf(), fmt.len());
//   return log_stream;
// }

// LogStream& operator<<(LogStream& log_stream, const StaticStrHolder& str) {
//   log_stream.append(str.str_, str.len_);
//   return log_stream;
// }

// LogStream& operator<<(LogStream& log_stream, const SourceFile& file) {
//   log_stream.append(file.data_, file.len_);
//   return log_stream;
// }

// Explicit instantiations
template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);
