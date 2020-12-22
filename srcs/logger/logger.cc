#include "srcs/logger/logger.h"

#include <utility>
#include <stdlib.h>

#include "srcs/timer/timestamp.h"

using namespace net;

LogLevel initMinLogLevel() {
  int min_log_level = 1;
  char *min_log_level_env = ::getenv("MIN_LOG_LEVEL");
  if (min_log_level_env) min_log_level = atoi(min_log_level_env);
  
  return static_cast<LogLevel>(min_log_level);
}

LogLevel min_log_level = initMinLogLevel();

Logger::Impl::Impl(SourceFile file, const int line, LogLevel level, int old_errno) 
  : stream_(),
    file_(std::move(file)), 
    line_(line),
    level_(level),
    time_(Timestamp::now()) {
  
}

Logger::Logger(SourceFile file, const int line, LogLevel level) 
  : impl_(std::move(file), line, level, 0) {
  
}

Logger::~Logger() {

}