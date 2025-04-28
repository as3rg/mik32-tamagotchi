#pragma once

#include "abstract_drawable.h"
#include "assets.h"
#include "canvas.h"
#include "container.h"
#include "image.h"
#include "indicator_bar.h"
#include "progress_bar.h"
#include "screen.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>

namespace tamagotchi {
struct metric_pb : ui::abstract_drawable {
protected:
  constexpr static size_t icon_w = 8;
  constexpr static size_t icon_h = 8;
  constexpr static size_t icon_y = 0;
  constexpr static size_t icon_x = 0;

  constexpr static size_t bar_w = 8;
  constexpr static size_t bar_h = 54;
  constexpr static size_t bar_y = 10;
  constexpr static size_t bar_x = 0;

public:
  ui::container container;
  ui::image<icon_h, icon_w> icon;
  ui::progress_bar bar;

  constexpr metric_pb(
      size_t y, size_t x,
      const utils::canvas<icon_h, icon_w, utils::canvas_orientation::VERTICAL>
          &img)
      : abstract_drawable(y, x), container(0, 0), icon(icon_y, icon_x, img),
        bar(bar_y, bar_x, bar_h, bar_w) {
    container.drawables().push_back(icon);
    container.drawables().push_back(bar);
  }

protected:
  constexpr void draw_impl(ui::screen::canvas &canvas, size_t offset_y,
                           size_t offset_x) const override {
    container.draw(canvas, offset_y, offset_x);
  }
};

struct metric_ind : ui::abstract_drawable {
protected:
  constexpr static size_t icon_w = 8;
  constexpr static size_t icon_h = 8;
  constexpr static size_t icon_y = 0;
  constexpr static size_t icon_x = 0;

  constexpr static size_t bar_w = 8;
  constexpr static size_t bar_h = 54;
  constexpr static size_t bar_y = 10;
  constexpr static size_t bar_x = 0;

public:
  ui::container container;
  ui::image<icon_h, icon_w> icon;
  ui::indicator_bar bar;

  constexpr metric_ind(
      size_t y, size_t x,
      const utils::canvas<icon_h, icon_w, utils::canvas_orientation::VERTICAL>
          &img)
      : abstract_drawable(y, x), container(0, 0), icon(icon_y, icon_x, img),
        bar(bar_y, bar_x, bar_h, bar_w) {
    container.drawables().push_back(icon);
    container.drawables().push_back(bar);
  }

protected:
  constexpr void draw_impl(ui::screen::canvas &canvas, size_t offset_y,
                           size_t offset_x) const override {
    container.draw(canvas, offset_y, offset_x);
  }
};

struct clock : ui::abstract_drawable {
protected:
  constexpr static size_t digit_width = 5;

  ui::container container;
  std::array<ui::image<7, 4>, 5> digits{
      ui::image(0, digit_width * 0, assets::digits[0]),
      ui::image(0, digit_width * 1, assets::digits[2]),
      ui::image(0, digit_width * 2, assets::digits_colon),
      ui::image(0, digit_width * 3, assets::digits[3]),
      ui::image(0, digit_width * 4, assets::digits[4]),
  };

public:
  constexpr clock(size_t y, size_t x)
      : ui::abstract_drawable(y, x), container(0, 0) {
    for (auto &digit : digits) {
      container.drawables().push_back(digit);
    }
  }

  constexpr void set(uint8_t hours, uint8_t minutes) {
    digits[0].icon() = assets::digits[hours / 10 % 10];
    digits[1].icon() = assets::digits[hours % 10];

    digits[3].icon() = assets::digits[minutes / 10 % 10];
    digits[4].icon() = assets::digits[minutes % 10];
  }

protected:
  constexpr void draw_impl(ui::screen::canvas &canvas, size_t offset_y,
                           size_t offset_x) const override {
    container.draw(canvas, offset_y, offset_x);
  }
};

template <size_t asset_h, size_t asset_w,
          const utils::canvas<asset_h, asset_w> &asset, size_t asset_offset,
          size_t count>
struct icon_bar : ui::abstract_drawable {
protected:
  ui::container container;
  std::array<ui::image<asset_h, asset_w>, count> icons =
      make_array(std::make_index_sequence<count>());

public:
  static constexpr uint8_t max_progress = 0xff;

  constexpr icon_bar(size_t y, size_t x)
      : ui::abstract_drawable(y, x), container(0, 0) {
    for (auto &i : icons) {
      container.drawables().push_back(i);
    }
  }

  constexpr ~icon_bar() override = default;

  constexpr icon_bar(const icon_bar &) = default;

  constexpr icon_bar(icon_bar &&) = default;

  constexpr icon_bar &operator=(const icon_bar &) = default;

  constexpr icon_bar &operator=(icon_bar &&) = default;

  constexpr void set_progress(uint8_t progress) {
    for (size_t i = 0; i < icons.size(); ++i) {
      icons[i].visible() = i * max_progress < icons.size() * progress;
    }
  }

protected:
  constexpr void draw_impl(ui::screen::canvas &canvas, size_t offset_y,
                           size_t offset_x) const override {
    container.draw(canvas, offset_y, offset_x);
  }

  template <size_t... Is>
  constexpr std::array<ui::image<asset_h, asset_w>, sizeof...(Is)>
  make_array(std::index_sequence<Is...>) {
    return {(ui::image(0, asset_offset * Is, asset))...};
  }
};

using heart_bar = icon_bar<6, 7, assets::heart, 8, 10>;
using energy_bar = icon_bar<6, 4, assets::energy, 5, 10>;

struct info_panel : ui::abstract_drawable {
  tamagotchi::metric_ind temperature;
  tamagotchi::metric_ind brightness;
  tamagotchi::metric_pb satiety;
  tamagotchi::metric_pb happiness;
  tamagotchi::heart_bar health;
  tamagotchi::energy_bar energy;
  tamagotchi::clock clock;
  ui::image<50, 84> eyes;
  ui::container container;

  constexpr info_panel(size_t y, size_t x)
      : ui::abstract_drawable(y, x), temperature(0, 0, assets::temperature),
        brightness(0, 10, assets::sun), satiety(0, 20, assets::cherry),
        happiness(0, 30, assets::smile), health(58, 44), energy(0, 44),
        clock(0, 103), eyes(8, 44, assets::eyes_center), container(0, 0) {
    container.drawables().push_back(temperature);
    container.drawables().push_back(brightness);
    container.drawables().push_back(health);
    container.drawables().push_back(energy);
    container.drawables().push_back(happiness);
    container.drawables().push_back(satiety);
    container.drawables().push_back(clock);
    container.drawables().push_back(eyes);
  }

protected:
  constexpr void draw_impl(ui::screen::canvas &canvas, size_t offset_y,
                           size_t offset_x) const override {
    container.draw(canvas, offset_y, offset_x);
  }
};
} // namespace tamagotchi