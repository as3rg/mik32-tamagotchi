#pragma once

#include "filter.h"

namespace tamagotchi {
struct logical_button {
public:
  enum class button_event { NONE, CLICK, HOLD };

protected:
  constexpr static size_t filter_size = 8;
  constexpr static size_t filter_threshold = filter_size / 2;
  constexpr static size_t hold_threshold = 30;

  utils::filter<filter_size, bool, size_t> filter;
  size_t hold_time = 0;

public:
  constexpr logical_button() = default;

  constexpr ~logical_button() = default;

  constexpr logical_button(const logical_button &) = default;

  constexpr logical_button(logical_button &&) = default;

  constexpr logical_button &operator=(const logical_button &) = default;

  constexpr logical_button &operator=(logical_button &&) = default;

  constexpr button_event tick(bool status) {
    if (update_status(status)) {
      ++hold_time;
      if (hold_time == hold_threshold) {
        return button_event::HOLD;
      }
    } else if (hold_time) {
      if (hold_time < hold_threshold) {
        hold_time = 0;
        return button_event::CLICK;
      }
      hold_time = 0;
    }
    return button_event::NONE;
  }

protected:
  constexpr bool update_status(bool status) {
    filter.update(status);
    return filter.sum() >= filter_threshold;
  }
};
} // namespace tamagotchi
