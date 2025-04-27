#pragma once

#include "canvas.h"
#include "drawable.h"
#include "intrusive-list.h"

namespace ui {
struct screen {
public:
  struct screen_tag;
  using canvas = utils::canvas<64, 128, utils::canvas_orientation::VERTICAL>;

protected:
  canvas screen_canvas;
  utils::list<drawable<canvas, screen_tag>, screen_tag> drawables_;

public:
  constexpr screen(std::initializer_list<std::initializer_list<bool>> ini = {})
      : screen_canvas(ini) {}

  constexpr screen(const screen &) = default;

  constexpr screen(screen &&) = default;

  constexpr screen &operator=(const screen &) = default;

  constexpr screen &operator=(screen &&) = default;

  constexpr void redraw() {
    screen_canvas.fill_rectangle(0, 0, screen_canvas.height(),
                                 screen_canvas.width(), false);
    for (auto it = drawables().begin(); it != drawables().end(); ++it) {
      it->draw(screen_canvas, 0, 0);
    }
  }

  constexpr decltype(auto) raw_data() const { return screen_canvas.raw_data(); }

  constexpr size_t raw_size() const { return screen_canvas.raw_size(); }

  constexpr utils::list<drawable<canvas, screen_tag>, screen_tag> &drawables() {
    return drawables_;
  }

  constexpr const utils::list<drawable<canvas, screen_tag>, screen_tag> &
  drawables() const {
    return drawables_;
  }
};
} // namespace ui
