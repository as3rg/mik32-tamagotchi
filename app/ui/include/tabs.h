#pragma once

#include "abstract_drawable.h"

namespace ui {
struct tabs : abstract_drawable {
protected:
  list<drawable<screen::canvas, screen::screen_tag>, screen::screen_tag>
      drawables_;
  size_t index_;

public:
  constexpr tabs(size_t y, size_t x) : abstract_drawable(y, x) {}

  constexpr tabs(const tabs &) = default;

  constexpr tabs(tabs &&) = default;

  constexpr tabs &operator=(const tabs &) = default;

  constexpr tabs &operator=(tabs &&) = default;

  constexpr list<drawable<screen::canvas, screen::screen_tag>,
                 screen::screen_tag> &
  drawables() {
    return drawables_;
  }

  constexpr const list<drawable<screen::canvas, screen::screen_tag>,
                       screen::screen_tag> &
  drawables() const {
    return drawables_;
  }

  constexpr size_t &index() { return index_; }

  constexpr const size_t &index() const { return index_; }

protected:
  constexpr void draw_impl(screen::canvas &canvas, size_t offset_y,
                           size_t offset_x) const override {
    auto it = drawables().begin();
    for (size_t i = 0; i < index() && it != drawables().end(); ++it, ++i) {
    }

    if (it != drawables().end()) {
      it->draw(canvas, offset_y, offset_x);
    }
  }
};
} // namespace ui
