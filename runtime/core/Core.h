#ifndef XENGINE_CORE_CORE_H

#include "core/Log.h"

#if X_WINDOWS
#    define X_TRAP __debugbreak
#else
#    define X_TRAP __builtin_trap
#endif // X_WINDOWS

#if !__GNUC__
#    define __PRETTY_FUNCTION__ __FUNCSIG__
#endif // !__GNUC__

#if X_DEBUG
#    define x_assert(condition) do { if (!(condition)) { xEngine::Log::GetInstance().Assert(#condition, nullptr, __FILE__, __LINE__, __PRETTY_FUNCTION__); X_TRAP(); } } while(0)
#    define x_assert_msg(condition, message) do { if (!(condition)) { xEngine::Log::GetInstance().Assert(#condition, message, __FILE__, __LINE__, __PRETTY_FUNCTION__); X_TRAP(); } } while(0)
#    define x_assert_range(value, min, max) x_assert_msg((value >= min) && (value <= max), "out of range")
#    define x_static_assert(condition) static_assert((condition))
#    define x_static_assert_range(value, min, max) x_static_assert((value >= min) && (value <= max))
#    define x_error(...) do { xEngine::Log::GetInstance().Error(__VA_ARGS__); X_TRAP(); } while(0)
#else
#    define x_assert(condition) ((void)0)
#    define x_assert_msg(condition, message) ((void)0)
#    define x_assert_range(value, min, max) ((void)0)
#    define x_static_assert(condition) ((void)0)
#    define x_static_assert_range(value, min, max) ((void)0)
#    define x_error(...) do { exit(-1); } while(0)
#endif // X_DEBUG

#define CREATE_FUNC_DECLARE(clazz) \
  template<typename... ARGS> \
  static eastl::shared_ptr<clazz> Create(ARGS &&... args) { \
    return eastl::make_shared<clazz>(eastl::forward<ARGS>(args)...); \
  } \
  template<typename... ARGS> \
  static eastl::unique_ptr<clazz> CreateUnique(ARGS &&... args) { \
    return eastl::make_unique<clazz>(eastl::forward<ARGS>(args)...); \
  }

#define PTR_DECLARE(clazz) \
  using clazz##Ptr = eastl::shared_ptr<clazz>; \
  using clazz##UniquePtr = eastl::unique_ptr<clazz>;

#define DISALLOW_ASSIGN(clazz) void operator=(const clazz &) = delete;

#define DISALLOW_COPY_AND_ASSIGN(clazz) \
  clazz(const clazz &) = delete; \
  DISALLOW_ASSIGN(clazz)

#define DISALLOW_IMPLICIT_CONSTRUCTORS(clazz) \
  clazz() = delete; \
  DISALLOW_COPY_AND_ASSIGN(clazz)

#endif // XENGINE_CORE_CORE_H
