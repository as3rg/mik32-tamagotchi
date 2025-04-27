#pragma once

#include "canvas.h"
#include "intrusive-list.h"

struct screen;
struct screen_tag;

template <typename Canvas>
struct drawable : list_element<screen_tag> {
  virtual constexpr ~drawable() = default;
  virtual constexpr void draw(Canvas&) = 0;
};

struct screen {
public:
  using canvas = canvas<64, 128, canvas_orientation::VERTICAL>;
private:
  canvas screen_canvas;
public:

  list<drawable<canvas>, screen_tag> drawables;

  constexpr screen(std::initializer_list<std::initializer_list<bool>> ini = {}) : screen_canvas(ini) {}

  constexpr screen(const screen&) = default;
  
  constexpr screen(screen&&) = default;

  constexpr screen& operator=(const screen&) = default;

  constexpr screen& operator=(screen&&) = default;

  constexpr void redraw() {
    for (auto it = drawables.begin(); it != drawables.end(); ++it) {
      it->draw(screen_canvas);
    }
  }

  constexpr decltype(auto) raw_data() const {
    return screen_canvas.raw_data();
  }

  constexpr size_t raw_size() const {
    return screen_canvas.raw_size();
  }
};

template <size_t h_, size_t w_>
struct image : drawable<screen::canvas> {
private:
  const canvas<h_, w_, canvas_orientation::VERTICAL> &data;
  size_t y;
  size_t x;
public:
  constexpr image(const canvas<h_, w_, canvas_orientation::VERTICAL> &data, size_t y, size_t x) : data(data), y(y), x(x) {}

  constexpr ~image() override = default;

  constexpr image(const image&) = default;
  
  constexpr image(image&&) = default;

  constexpr image& operator=(const image&) = default;

  constexpr image& operator=(image&&) = default;

  constexpr void draw(screen::canvas& screen) override {
    screen.draw(data, y, x);
  }
};
