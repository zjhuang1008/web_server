#ifndef LOGGER_H
#define LOGGER_H

#include "srcs/logger/log_stream.h"
#include "srcs/utils/uncopyable.h"
#include "srcs/timer/timestamp.h"

namespace net {

enum LogLevel {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUM_LOG_LEVELS,
};

class Logger : private Uncopyable {
public:
  class SourceFile {
  public:
    template<int N>
    SourceFile(const char (&file)[N]) : data_(file), size_(N-1) {
      const char *slash = strrchr(file, '/');
      if (slash) {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - file);
      }
    }
  
    // explicit SourceFile(const char *file) : data_(file) {
    //   const char *slash = strrchr(file, '/');
    //   if (slash) {
    //     data_ = slash + 1;
    //   }
    //   size_ = static_cast<int>(strlen(data_));
    // }

    const char *data_;
    int size_;
  };

  Logger(SourceFile file, const int line, LogLevel level);
  ~Logger();
private:
  class Impl {
  public:
    Impl(SourceFile file, const int line, LogLevel level, int old_errno);

    LogStream stream_;
    
    SourceFile file_;
    int line_;
    LogLevel level_;
    Timestamp time_;

  };
  Impl impl_;
};

extern LogLevel min_log_level;


#define LOG(severity) Logger(__FILE__, __LINE__, severity)

} // namespace net

#endif // LOGGER_H
