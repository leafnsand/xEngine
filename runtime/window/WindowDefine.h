#ifndef XENGINE_WINDOW_WINDOWDEFINE_H
#define XENGINE_WINDOW_WINDOWDEFINE_H

#include "core/Types.h"

namespace xEngine {

enum class MouseCursorMode: uint8 {
  kNormal,
  kHidden,
  kDisable,
};

enum class MouseButtonType: uint8 {
  kUnknown = 0,
  kLeft = 1,
  kRight = 2,
  kMiddle = 4,
};

enum class KeyboardButtonType: uint8 {
  UNKNOWN,
  ESC,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  NUM_1,
  NUM_2,
  NUM_3,
  NUM_4,
  NUM_5,
  NUM_6,
  NUM_7,
  NUM_8,
  NUM_9,
  NUM_0,
  NUM_PAD_1,
  NUM_PAD_2,
  NUM_PAD_3,
  NUM_PAD_4,
  NUM_PAD_5,
  NUM_PAD_6,
  NUM_PAD_7,
  NUM_PAD_8,
  NUM_PAD_9,
  NUM_PAD_0,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,

  COUNT,
};

} // namespace xEngine

#endif // XENGINE_WINDOW_WINDOWDEFINE_H