#pragma once

#include "bitarray.h"
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

  always_inline constexpr screen(std::initializer_list<std::initializer_list<bool>> ini = {}) : screen_canvas(ini) {}

  always_inline constexpr screen(const screen&) = default;
  
  always_inline constexpr screen(screen&&) = default;

  always_inline constexpr screen& operator=(const screen&) = default;

  always_inline constexpr screen& operator=(screen&&) = default;

  always_inline constexpr void redraw() {
    for (auto it = drawables.begin(); it != drawables.end(); ++it) {
      it->draw(screen_canvas);
    }
  }

  always_inline constexpr decltype(auto) raw_data() const {
    return screen_canvas.raw_data();
  }

  always_inline constexpr size_t raw_size() const {
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
  always_inline constexpr image(const canvas<h_, w_, canvas_orientation::VERTICAL> &data, size_t y, size_t x) : data(data), y(y), x(x) {}

  always_inline constexpr ~image() override = default;

  always_inline constexpr image(const image&) = default;
  
  always_inline constexpr image(image&&) = default;

  always_inline constexpr image& operator=(const image&) = default;

  always_inline constexpr image& operator=(image&&) = default;

  always_inline constexpr void draw(screen::canvas& screen) override {
    screen.draw(data, y, x);
  }
};
