#pragma once

#include "abstract_drawable.h"

namespace ui {
struct progress_bar : abstract_drawable {
protected:
  size_t h;
  size_t w;
  uint8_t progress_;

  constexpr static size_t border_width = 1;
  constexpr static size_t offset = 2;
  constexpr static uint8_t max_progress = 0xff;

public:
  constexpr progress_bar(size_t y, size_t x, size_t h, size_t w)
      : abstract_drawable(y, x), h(h), w(w), progress_(max_progress / 2) {}

  constexpr ~progress_bar() override = default;

  constexpr progress_bar(const progress_bar &) = default;

  constexpr progress_bar(progress_bar &&) = default;

  constexpr progress_bar &operator=(const progress_bar &) = default;

  constexpr progress_bar &operator=(progress_bar &&) = default;

  constexpr uint8_t &progress() { return progress_; }

  constexpr const uint8_t &progress() const { return progress_; }

  constexpr size_t &height() { return h; }

  constexpr const size_t &height() const { return h; }

  constexpr size_t &width() { return w; }

  constexpr const size_t &width() const { return w; }

protected:
  constexpr void draw_impl(screen::canvas &screen, size_t offset_y,
                           size_t offset_x) const override {
    screen.fill_rectangle(offset_y, offset_x, height(), width(), false);
    screen.draw_rectangle(offset_y, offset_x, height(), width(), border_width);

    size_t padding = border_width + offset;
    size_t target_height = (height() - 2 * padding) * progress() / max_progress;
    screen.fill_rectangle(offset_y + height() - target_height - padding,
                          offset_x + padding, target_height,
                          width() - 2 * padding);
  }
};
} // namespace ui
