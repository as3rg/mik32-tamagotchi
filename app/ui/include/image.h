#pragma once

#include "abstract_drawable.h"
#include <cstddef>
#include <functional>

namespace ui {
template <size_t h_, size_t w_> struct image : abstract_drawable {
protected:
  std::reference_wrapper<const utils::canvas<h_, w_, utils::canvas_orientation::VERTICAL>> data;
  bool inverted_;

public:
  constexpr image(
      size_t y, size_t x,
      const utils::canvas<h_, w_, utils::canvas_orientation::VERTICAL> &data)
      : abstract_drawable(y, x), data(data), inverted_(false) {}

  constexpr ~image() override = default;

  constexpr image(const image &) = default;

  constexpr image(image &&) = default;

  constexpr image &operator=(const image &) = default;

  constexpr image &operator=(image &&) = default;

  constexpr bool &inverted() { return inverted_; }

  constexpr const bool &inverted() const { return inverted_; }

  constexpr auto &icon() {
    return data;
  } 

  constexpr const auto &icon() const {
    return data;
  } 

protected:
  constexpr void draw_impl(screen::canvas &screen, size_t offset_y,
                           size_t offset_x) const override {
    screen.draw(data.get(), offset_y, offset_x, inverted());
  }
};
} // namespace ui
