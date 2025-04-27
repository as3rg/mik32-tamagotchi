#pragma once

#include "drawable.h"
#include "screen.h"

namespace ui {
struct abstract_drawable : drawable<screen::canvas, screen::screen_tag> {
protected:
  size_t y_;
  size_t x_;
  bool visible_ = true;

public:
  constexpr abstract_drawable(size_t y, size_t x)
      : y_(y), x_(x), visible_(true) {}

  constexpr ~abstract_drawable() override = default;

  constexpr abstract_drawable(const abstract_drawable &) = default;

  constexpr abstract_drawable(abstract_drawable &&) = default;

  constexpr abstract_drawable &operator=(const abstract_drawable &) = default;

  constexpr abstract_drawable &operator=(abstract_drawable &&) = default;

  constexpr bool &visible() { return visible_; }

  constexpr const bool &visible() const { return visible_; }

  constexpr size_t &y() { return y_; }

  constexpr const size_t &y() const { return y_; }

  constexpr size_t &x() { return x_; }

  constexpr const size_t &x() const { return x_; }

  constexpr void draw(screen::canvas &canvas, size_t offset_y,
                      size_t offset_x) const override {
    if (visible()) {
      draw_impl(canvas, offset_y + y(), offset_x + x());
    }
  }

protected:
  virtual constexpr void draw_impl(screen::canvas &canvas, size_t offset_y,
                                   size_t offset_x) const = 0;
};
} // namespace ui