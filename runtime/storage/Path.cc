#include "Path.h"

#include "core/Types.h"

#include <sys/stat.h>

#if X_WINDOWS
#include <Windows.h>
#include <direct.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) &_S_IFMT) == _S_IFDIR)
#endif
#elif X_MACOS
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif

namespace xEngine {

#if X_WINDOWS
eastl::string Path::separator_ = "\\";
#else
eastl::string Path::separator_ = "/";
#endif

Path Path::ExecutablePath() {
#if X_WINDOWS
  char path[MAX_PATH];
  GetModuleFileName(nullptr, path, MAX_PATH);
  return eastl::string(path);
#elif X_MACOS
  char path[PATH_MAX];
  uint32 length;
  _NSGetExecutablePath(path, &length);
  return eastl::string(path);
#endif
}

Path Path::CurrentDirectory() {
  auto result = getcwd(nullptr, 0);
  eastl::string string(result);
  free(result);
  return string;
}

bool Path::Exist(const eastl::string &path) {
  struct stat status;
  return stat(path.c_str(), &status) == 0;
}

bool Path::IsDirectory(const eastl::string &path) {
  struct stat status;
  return stat(path.c_str(), &status) == 0 && S_ISDIR(status.st_mode);
}

void Path::Create(const eastl::string &path) {
  if (IsDirectory(path)) {
    mkdir(path.c_str()
#if !X_WINDOWS
      , 0644
#endif
    );
  } else {
    auto fd = fopen(path.c_str(), "w");
    if (fd) fclose(fd);
  }
}

void Path::Delete(const eastl::string &path) {
  if (Exist(path)) {
    if (IsDirectory(path)) {
      rmdir(path.c_str());
    } else {
      remove(path.c_str());
    }
  }
}

void Path::Move(const eastl::string &from, const eastl::string &to) {
  rename(from.c_str(), to.c_str());
}

Path &Path::ParentDirectory() {
  string_ = string_.substr(0, string_.find_last_of(separator_));
  return *this;
}

Path &Path::Append(const eastl::string &name) {
  string_.append(separator_).append(name);
  return *this;
}

}