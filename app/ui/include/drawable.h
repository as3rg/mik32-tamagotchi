#pragma once

#include "intrusive-list.h"

namespace ui {
template <typename Canvas, typename Tag>
struct drawable : utils::list_element<Tag> {
  virtual constexpr ~drawable() = default;
  virtual constexpr void draw(Canvas &, size_t offset_y,
                              size_t offset_x) const = 0;
};
} // namespace ui
