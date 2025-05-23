#pragma once

#include "abstract_drawable.h"
#include "screen.h"

namespace ui {
struct container : abstract_drawable {
protected:
  utils::list<drawable<screen::canvas, screen::screen_tag>, screen::screen_tag>
      drawables_;

public:
  constexpr container(size_t y, size_t x) : abstract_drawable(y, x) {}

  constexpr container(const container &) = default;

  constexpr container(container &&) = default;

  constexpr container &operator=(const container &) = default;

  constexpr container &operator=(container &&) = default;

  constexpr utils::list<drawable<screen::canvas, screen::screen_tag>,
                        screen::screen_tag> &
  drawables() {
    return drawables_;
  }

  constexpr const utils::list<drawable<screen::canvas, screen::screen_tag>,
                              screen::screen_tag> &
  drawables() const {
    return drawables_;
  }

protected:
  constexpr void draw_impl(screen::canvas &canvas, size_t offset_y,
                           size_t offset_x) const override {
    for (auto it = drawables().begin(); it != drawables().end(); ++it) {
      it->draw(canvas, offset_y, offset_x);
    }
  }
};
} // namespace ui