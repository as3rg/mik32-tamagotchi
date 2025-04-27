#pragma once

#include "intrusive-list.h"

template <typename Canvas, typename Tag> struct drawable : list_element<Tag> {
  virtual constexpr ~drawable() = default;
  virtual constexpr void draw(Canvas &, size_t offset_y, size_t offset_x) const = 0;
};
