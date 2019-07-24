#ifndef XENGINE_STORAGE_PATH_H
#define XENGINE_STORAGE_PATH_H

#include <EASTL/string.h>

namespace xEngine {

class Path {
 public:
  static const eastl::string &separator() { return separator_ ;}

  static Path ExecutablePath();

  static Path CurrentDirectory();

  static bool Exist(const eastl::string &path);

  static bool IsDirectory(const eastl::string &path);

  static void Create(const eastl::string &path);

  static void Delete(const eastl::string &path);

  static void Move(const eastl::string &from, const eastl::string &to);

 public:
  Path(const eastl::string &path) : string_(path) {}

  Path &ParentDirectory();

  Path &Append(const eastl::string &name);

  operator const eastl::string &() const { return string_; }

  const eastl::string &string() const { return string_; }

 private:
  static eastl::string separator_;

 private:
  eastl::string string_;
};

} // namespace xEngine

#endif // XENGINE_STORAGE_PATH_H