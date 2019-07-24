#ifndef XENGINE_IO_LOCATION_H
#define XENGINE_IO_LOCATION_H

#include "core/Core.h"

#include <EASTL/string.h>

namespace xEngine {

class Location {
 public:
  Location();

  Location(const char *c_str);

  Location(const eastl::string &string);

  Location(const Location &other);

  Location(Location &&other);

  void operator=(const char *c_str);

  void operator=(const eastl::string &string);

  void operator=(const Location &other);

  void operator=(Location &&other);

  bool operator==(const char *c_str) const;

  bool operator==(const eastl::string &string) const;

  bool operator==(const Location &other) const;

  const eastl::string &filesystem_name() const;

  const eastl::string &dir() const;

  const eastl::string &path() const;

  const eastl::string &raw_string() const;

 private:
  void update();

 protected:
  eastl::string raw_string_;
  eastl::string filesystem_name_;
  eastl::string path_;
  eastl::string dir_;
};

inline Location::Location() {
}

inline Location::Location(const char *c_str) :
    raw_string_(c_str) {
  update();
}

inline Location::Location(const eastl::string &string) :
    raw_string_(string) {
  update();
}

inline Location::Location(const Location &other) :
    raw_string_(other.raw_string_),
    filesystem_name_(other.filesystem_name_),
    path_(other.path_),
    dir_(other.dir_) {
}

inline Location::Location(Location &&other) {
  raw_string_ = eastl::move(other.raw_string_);
  filesystem_name_ = eastl::move(other.filesystem_name_);
  path_ = eastl::move(other.path_);
  dir_ = eastl::move(other.dir_);
}

inline void Location::operator=(const char *c_str) {
  raw_string_ = c_str;
  update();
}

inline void Location::operator=(const eastl::string &string) {
  raw_string_ = string;
  update();
}

inline void Location::operator=(const Location &other) {
  raw_string_ = other.raw_string_;
  filesystem_name_ = other.filesystem_name_;
  path_ = other.path_;
  dir_ = other.dir_;
}

inline void Location::operator=(Location &&other) {
  raw_string_ = eastl::move(other.raw_string_);
  filesystem_name_ = eastl::move(other.filesystem_name_);
  path_ = eastl::move(other.path_);
  dir_ = eastl::move(other.dir_);
}

inline bool Location::operator==(const char *c_str) const {
  return raw_string_ == c_str;
}

inline bool Location::operator==(const eastl::string &string) const {
  return raw_string_ == string;
}

inline bool Location::operator==(const Location &other) const {
  return raw_string_ == other.raw_string_;
}

inline const eastl::string &Location::filesystem_name() const {
  return filesystem_name_;
}

inline const eastl::string &Location::dir() const {
  return dir_;
}

inline const eastl::string &Location::path() const {
  return path_;
}

inline const eastl::string &Location::raw_string() const {
  return raw_string_;
}

inline void Location::update() {
  x_assert(raw_string_.find("://") != eastl::string::npos);
  filesystem_name_ = raw_string_.substr(0, raw_string_.find("://"));
  path_ = raw_string_.substr(raw_string_.find("://") + 3);
  dir_ = raw_string_.substr(0, raw_string_.rfind("/"));
}

} // namespace xEngine

#endif // XENGINE_IO_LOCATION_H