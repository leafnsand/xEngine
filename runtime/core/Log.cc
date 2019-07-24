#include "core/Log.h"

#include <cstdio>

#if X_WINDOWS
#   include <Windows.h>
#   define LOG_BUFFER_SIZE 2048
#endif // X_WINDOWS

namespace xEngine {

void Log::AddLogger(LoggerInterfacePtr logger) {
  RWLockWriteScope scope(rw_lock_);
  if (logger != nullptr && eastl::find(loggers_.begin(), loggers_.end(), logger) == loggers_.end()) {
    loggers_.push_back(logger);
  }
}

void Log::Error(const char *message, ...) {
  if (level_ >= LogLevel::kFatal) {
    va_list args;
    va_start(args, message);
    Record(LogLevel::kFatal, message, args);
    va_end(args);
  }
}

void Log::Warning(const char *message, ...) {
  if (level_ >= LogLevel::kWarning) {
    va_list args;
    va_start(args, message);
    Record(LogLevel::kWarning, message, args);
    va_end(args);
  }
}

void Log::Info(const char *message, ...) {
  if (level_ >= LogLevel::kInfo) {
    va_list args;
    va_start(args, message);
    Record(LogLevel::kInfo, message, args);
    va_end(args);
  }
}

void Log::Debug(const char *message, ...) {
  if (level_ >= LogLevel::kDebug) {
    va_list args;
    va_start(args, message);
    Record(LogLevel::kDebug, message, args);
    va_end(args);
  }
}

void Log::Assert(const char *condition, const char *message, const char *filename, int32 line, const char *function) {
#if X_DEBUG
  printf("engine assert: \n\tcondition: %s\n\tmessage: %s\n\tfilename: %s\n\tline: %d\n\tfunction: %s\n'",
         condition,
         message,
         filename,
         line,
         function);
#if X_WINDOWS
  char buffer[LOG_BUFFER_SIZE];
  snprintf(buffer,
           sizeof(buffer),
           "engine assert: \n\tcondition: %s\n\tmessage: %s\n\tfilename: %s\n\tline: %d\n\tfunction: %s\n'",
           condition,
           message,
           filename,
           line,
           function);
  buffer[LOG_BUFFER_SIZE - 1] = '\0';
  OutputDebugString(buffer);
#endif // X_WINDOWS
#endif // X_DEBUG

  eastl::vector<LoggerInterfacePtr> loggers;
  {
    RWLockReadScope scope(rw_lock_);
    loggers = loggers_;
  }

  for (auto &logger : loggers) {
    logger->Assert(condition, message, filename, line, function);
  }
}

void Log::Record(LogLevel mask, const char *message, va_list args) {

#if X_DEBUG

#if X_WINDOWS
  va_list argsCopy;
  va_copy(argsCopy, args);
#endif // X_WINDOWS

  vprintf(message, args);

#if X_WINDOWS
  char buffer[LOG_BUFFER_SIZE];
  vsnprintf(buffer, sizeof(buffer), message, argsCopy);
  buffer[LOG_BUFFER_SIZE - 1] = '\0';
  OutputDebugString(buffer);
#endif // X_WINDOWS

#endif // X_DEBUG

  eastl::vector<LoggerInterfacePtr> loggers;
  {
    RWLockReadScope scope(rw_lock_);
    loggers = loggers_;
  }

  for (auto &logger : loggers) {
    logger->Record(mask, message, args);
  }
}

} // namespace xEngine
