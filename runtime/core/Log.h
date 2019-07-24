#ifndef XENGINE_CORE_LOG_H
#define XENGINE_CORE_LOG_H

#include "core/Core.h"
#include "core/Types.h"
#include "core/RWLock.h"

#include <cstdarg>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>

namespace xEngine {

enum class LogLevel: int32 {
  kFatal,
  kWarning,
  kInfo,
  kDebug,
};

class LoggerInterface {
 public:
  virtual ~LoggerInterface() = default;

  virtual void Assert(const char *condition, const char *message, const char *filename, int32 line, const char *function) = 0;

  virtual void Record(LogLevel mask, const char *message, va_list args) = 0;
};

PTR_DECLARE(LoggerInterface)

class Log final {
 public:
  static Log &GetInstance() {
    static Log instance;
    return instance;
  }

  void AddLogger(LoggerInterfacePtr logger);

  void SetLogLevel(LogLevel value) { level_ = value; }

  LogLevel GetLogLevel() const { return level_; }

  void Error(const char *message, ...);

  void Warning(const char *message, ...);

  void Info(const char *message, ...);

  void Debug(const char *message, ...);

  void Assert(const char *condition, const char *message, const char *filename, int32 line, const char *function);

 private:
  void Record(LogLevel mask, const char *message, va_list args);

 private:
  LogLevel level_{LogLevel::kDebug};
  RWLock rw_lock_;
  eastl::vector<LoggerInterfacePtr> loggers_;
};

} // namespace xEngine

#endif //XENGINE_CORE_LOG_H
