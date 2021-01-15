#include "srcs/logger/logger.h"

#include <utility>
#include <thread>
#include <sstream>

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "srcs/timer/timestamp.h"
#include "srcs/timer/time_zone.h"
#include "srcs/logger/log_stream.h"

namespace net {

namespace curr_thread {

static thread_local char t_errnobuf[512];
static thread_local char t_time[64];
static thread_local time_t t_lastSecond;

static thread_local char tid_str[32];
static thread_local size_t tid_str_len = 0;

void cacheID() {
  if (__builtin_expect(tid_str_len == 0, 0)) {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    tid_str_len = static_cast<size_t>(snprintf(tid_str, sizeof tid_str, "%s ", ss.str().c_str()));
  }
}

inline const char* getTidStr() {
  return tid_str;
}

inline size_t getTidStrLen() {
  return tid_str_len;
}

} // namespace curr_thread


TimeZone g_logTimeZone;

const char* strerror_tl(int saved_errno) {
  return strerror_r(saved_errno, curr_thread::t_errnobuf, sizeof curr_thread::t_errnobuf);
}

const constexpr char* kLogLevelName[NUM_LOG_LEVELS] = {
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

LogLevel initMinLogLevel() {
  int min_log_lvl = 0; // default log level
  char *min_log_level_env = ::getenv("MIN_LOG_LEVEL");
  if (min_log_level_env) min_log_lvl = atoi(min_log_level_env);
  
  return static_cast<LogLevel>(min_log_lvl);
}

void defaultOutputFunc(const char* str, size_t len) {
  fwrite(str, 1, len, stdout);
}

void defaultFlushFunc() {
  fflush(stdout);
}

} // namespace net

using namespace net;

LogLevel net::g_min_log_level = initMinLogLevel();

Logger::OutputFunc g_output_func = defaultOutputFunc;
Logger::FlushFunc g_flush_func = defaultFlushFunc;

Logger::Logger(SourceFile file, const int line, LogLevel level) 
  : stream_(),
    file_(std::move(file)),
    line_(line),
    level_(level),
    saved_errno_(0),
    time_(Timestamp::now()) {
  writeHeader();
}

Logger::Logger(SourceFile file, const int line, LogLevel level, bool log_errno) 
  : stream_(),
    file_(std::move(file)),
    line_(line),
    level_(level),
    saved_errno_(errno),
    time_(Timestamp::now()) {
  writeHeader();
}

Logger::~Logger() {
  stream_ << '\n';
  const LogStream::Buffer& buf = stream_.buffer();
  g_output_func(buf.data(), buf.len());
  // TODO: when use remote debug of clion, flush is needed
//  g_flush_func();
  if (level_ == FATAL) {
    g_flush_func();
  }
}

void Logger::setOutputFunc(OutputFunc func) {
  g_output_func = func;
}

void Logger::setFlushFunc(FlushFunc func) {
  g_flush_func = func;
}

void Logger::writeHeader() {
  formatTime();  
  curr_thread::cacheID();
  stream_ << StaticStrHolder(curr_thread::getTidStr(), curr_thread::getTidStrLen());
  // stream_ << file_ << ':' << line_ << ' ';
  formatFileAndLine();
  stream_ << StaticStrHolder(kLogLevelName[level_], 6) << ": ";
  
  if (saved_errno_ != 0) {
    stream_ << strerror_tl(saved_errno_) << " (errno=" << saved_errno_ << ") ";
  }
}

void Logger::formatTime() {
  int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
  if (seconds != curr_thread::t_lastSecond) {
    curr_thread::t_lastSecond = seconds;
    struct tm tm_time;
    if (g_logTimeZone.valid()) {
      tm_time = g_logTimeZone.toLocalTime(seconds);
    }
    else
    {
      ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime
    }

    int len = snprintf(curr_thread::t_time, sizeof(curr_thread::t_time), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17); (void)len;
  }

  if (g_logTimeZone.valid()) {
    Fmt us(".%06d ", microseconds);
    assert(us.len() == 8);
    stream_ << StaticStrHolder(curr_thread::t_time, 17) << StaticStrHolder(us.buf(), 8);
  }
  else {
    Fmt us(".%06dZ ", microseconds);
    assert(us.len() == 9);
    stream_ << StaticStrHolder(curr_thread::t_time, 17) << StaticStrHolder(us.buf(), 9);
  }
}

void Logger::formatFileAndLine() {
  char buf[64];
  snprintf(buf, sizeof buf, "%20s:%d ", file_.data_, line_);
  stream_ << buf;
}