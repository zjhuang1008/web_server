#ifndef LOGGER_H
#define LOGGER_H

#include <functional>

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
  using OutputFunc = std::function<void(const char*, size_t)>;
  using FlushFunc = std::function<void()>;
  
  Logger(SourceFile file, const int line, LogLevel level);
  Logger(SourceFile file, const int line, LogLevel level, bool log_errno);
  ~Logger();

  LogStream& stream() { return stream_; };

  static void setOutputFunc(OutputFunc func);
  static void setFlushFunc(FlushFunc func);
private:
  LogStream stream_;
  SourceFile file_;
  const int line_;
  LogLevel level_;
  int saved_errno_;
  Timestamp time_;

  void formatTime();
  void formatFileAndLine();
  void writeHeader();
};

extern net::LogLevel g_min_log_level;
#define LOG(level) if (level >= g_min_log_level) net::Logger(__FILE__, __LINE__, level).stream()
#define LOGSYS(level) if (level >= g_min_log_level) net::Logger(__FILE__, __LINE__, level, true).stream()

} // namespace net


#endif // LOGGER_H
