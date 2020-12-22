#include "srcs/logger/log_stream.h"

#include <utility>
#include <algorithm>
#include <type_traits>
#include <string>
#include <stdexcept>

using namespace net;

namespace net {

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
static_assert(sizeof(digits) == 20, "wrong number of digits");

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

} // namespace net


template<int SIZE>
void FixedBuffer<SIZE>::cookieStart() {
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd() {
}

template<typename T>
void LogStream::formatInt(T v) {
  if (buffer_.remain() > kMaxNumericSize) {
    size_t len = convert(buffer_.curr(), v);
    buffer_.add(len);
  }
}

// void*
// template<>
// self& outputImpl(void*&&, std::integral_constant<OutputTypes, OutputTypes::POINTER>);

// LogStream& LogStream::operator<<(int) {
//   formatInt(v);
//   return *this;
// }

template<typename T>
LogStream& LogStream::operator<<(T&& v) {
  if (std::is_integral<T>::value) {
    return outputImplSelect<T, OutputTypes::INTEGER>(std::forward<T>(v));
  } else if (std::is_floating_point<T>::value) {
    return outputImplSelect<T, OutputTypes::FLOAT>(std::forward<T>(v));
  } else if (std::is_pointer<T>::value) {
    return outputImplSelect<T, OutputTypes::POINTER>(std::forward<T>(v));
  } else if (std::is_same<T, std::string>::value) {
    return outputImplSelect<T, OutputTypes::STRING>(std::forward<T>(v));
  } else {
    throw std::runtime_error("log stream got args that can not be parse");
  }
}

template<typename T, LogStream::OutputTypes which>
LogStream& LogStream::outputImplSelect(T&& v) {
  return outputImpl(std::forward<T>(v), typename std::integral_constant<OutputTypes, which>::type());
}


