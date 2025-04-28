#pragma once

#include "abstract_drawable.h"

namespace ui {
struct indicator_bar : abstract_drawable {
protected:
  size_t h;
  size_t w;
  uint8_t indicator_;
  uint8_t lvl_2_;
  uint8_t lvl_1_;

  constexpr static size_t border_width = 1;
  constexpr static size_t lvl_thickness = 1;
  constexpr static size_t lvl_dash_len = 1;
  constexpr static size_t indicator_thickness = 3;
public:
  constexpr static uint8_t max_value = 0xff;

  constexpr indicator_bar(size_t y, size_t x, size_t h, size_t w)
      : abstract_drawable(y, x), h(h), w(w), indicator_(max_value), lvl_2_(0),
        lvl_1_(max_value) {}

  constexpr ~indicator_bar() override = default;

  constexpr indicator_bar(const indicator_bar &) = default;

  constexpr indicator_bar(indicator_bar &&) = default;

  constexpr indicator_bar &operator=(const indicator_bar &) = default;

  constexpr indicator_bar &operator=(indicator_bar &&) = default;

  constexpr uint8_t &lvl_2() { return lvl_2_; }

  constexpr uint8_t &lvl_1() { return lvl_1_; }

  constexpr uint8_t &indicator() { return indicator_; }

  constexpr const uint8_t &lvl_2() const { return lvl_2_; }

  constexpr const uint8_t &lvl_1() const { return lvl_1_; }

  constexpr const uint8_t &indicator() const { return indicator_; }

  constexpr size_t &height() { return h; }

  constexpr const size_t &height() const { return h; }

  constexpr size_t &width() { return w; }

  constexpr const size_t &width() const { return w; }

protected:
  constexpr void draw_impl(screen::canvas &screen, size_t offset_y,
                           size_t offset_x) const override {
    screen.fill_rectangle(offset_y, offset_x, height(), width(), false);
    screen.draw_rectangle(offset_y, offset_x, height(), width(), border_width);

    {
      size_t target_y = offset_y + height() - lvl_thickness -
                        (height() - lvl_thickness) * lvl_1() / max_value;
      dash_line(screen, target_y, offset_x, offset_x + width(), lvl_thickness,
                lvl_dash_len);
    }
    {
      size_t target_y = offset_y + height() - lvl_thickness -
                        (height() - lvl_thickness) * lvl_2() / max_value;
      dash_line(screen, target_y, offset_x, offset_x + width(), lvl_thickness,
                lvl_dash_len);
    }
    {
      size_t target_y =
          offset_y + height() - indicator_thickness -
          (height() - indicator_thickness) * indicator_ / max_value;
      screen.fill_rectangle(target_y, offset_x, indicator_thickness, width());
    }
  }

  constexpr static void dash_line(screen::canvas &screen, size_t y, size_t x1,
                                  size_t x2, size_t thickness,
                                  size_t dash_len) {
    size_t x = x1;
    for (; x + dash_len < x2; x += dash_len * 2) {
      screen.fill_rectangle(y, x, thickness, dash_len);
    }

    if (x < x2) {
      screen.fill_rectangle(y, x, thickness, x2 - x);
    }
  }
};
} // namespace ui