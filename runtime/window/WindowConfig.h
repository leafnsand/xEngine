#ifndef XENGINE_WINDOW_WINDOWCONFIG_H
#define XENGINE_WINDOW_WINDOWCONFIG_H

#include "WindowDefine.h"
#include "WindowDelegate.h"

#include "core/Types.h"

#include "resource/ResourceIdentity.h"

#include <EASTL/string.h>

namespace xEngine {

struct WindowConfig {
  static WindowConfig ForWindow(WindowDelegate *delegate, int32 width, int32 height, const eastl::string &title);

  static WindowConfig ForFullScreen(WindowDelegate *delegate, const eastl::string &title);

  ResourceIdentity identity{ResourceIdentity::Unique()};

  WindowDelegate *delegate{nullptr};

  int32 width{1024};

  int32 height{768};

  int32 frame_buffer_width{1024};

  int32 frame_buffer_height{768};

  bool is_full_screen{false};

  bool is_dropfile_accepted{true};

  eastl::string title{"xEngine"};

};

inline WindowConfig WindowConfig::ForWindow(WindowDelegate *delegate, int32 width, int32 height, const eastl::string &title) {
  WindowConfig config;
  config.delegate = delegate;
  config.width = config.frame_buffer_width = width;
  config.height = config.frame_buffer_height = height;
  config.is_full_screen = false;
  config.title = title;
  return config;
}

inline WindowConfig WindowConfig::ForFullScreen(WindowDelegate *delegate, const eastl::string &title) {
  WindowConfig config;
  config.delegate = delegate;
  config.is_full_screen = true;
  config.title = title;
  return config;
}

} // namespace xEngine

#endif // XENGINE_WINDOW_WINDOWCONFIG_H