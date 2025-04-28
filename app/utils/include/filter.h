#pragma once

#include <stddef.h>

namespace utils {

template <size_t size, typename T = bool, typename Tsum = size_t>
struct filter {
protected:
  T values[size];
  Tsum sum_;
  size_t i = 0;

public:
  constexpr filter(const T &default_value = T{}) {
    for (auto &v : values) {
      v = default_value;
    }
    sum_ = default_value;
    for (size_t i = 1; i < size; ++i) {
      sum_ += default_value;
    }
  }

  constexpr ~filter() = default;

  constexpr filter(const filter &) = default;

  constexpr filter(filter &&) = default;

  constexpr filter &operator=(const filter &) = default;

  constexpr filter &operator=(filter &&) = default;

  constexpr void update(const T &value) {
    size_t i_ = next();
    sum_ -= values[i_];
    values[i_] = value;
    sum_ += values[i_];
  }

  constexpr Tsum sum() const { return sum_; }

protected:
  size_t next() {
    size_t old = i;
    i += 1;
    i %= size;
    return old;
  }
};
} // namespace utils
