#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <inttypes.h>
#include <stddef.h>
#include <type_traits>


namespace utils {
template <size_t size_> struct bitarray {
public:
  using storage_type = uint8_t;
  constexpr static size_t BITS_IN_ST = 8 * sizeof(storage_type);
  constexpr static size_t RAW_SIZE = (size_ + BITS_IN_ST - 1) / BITS_IN_ST;

private:
  template <bool is_const> struct bitview {
  private:
    using arr_t = std::conditional_t<is_const, const bitarray, bitarray>;

    arr_t *array;
    size_t index;

    template <size_t s_> friend struct bitarray;

    constexpr bitview(arr_t *array, size_t index)
        : array(array), index(index) {}

  public:
    constexpr bitview(const bitview &) = default;

    constexpr bitview(bitview &&) = delete;

    constexpr ~bitview() = default;

    constexpr bitview &operator=(const bitview &) = default;

    constexpr bitview &operator=(bitview &&) = delete;

    constexpr operator bool() const {
      return array->data[index / BITS_IN_ST] &
             (static_cast<storage_type>(1) << (index % BITS_IN_ST));
    }

    constexpr bitview &operator=(bool b)
      requires(!is_const)
    {
      set_bit(b);
      return *this;
    }

    constexpr const bitview &operator=(bool b) const
      requires(!is_const)
    {
      set_bit(b);
      return *this;
    }

    constexpr friend void swap(bitview &l, bitview &r) {
      std::swap(l.array, r.array);
      std::swap(l.index, r.index);
    }

  private:
    constexpr void set_bit(bool b) const
      requires(!is_const)
    {
      storage_type &byte = array->data[index / BITS_IN_ST];
      storage_type mask =
          (static_cast<storage_type>(1) << (index % BITS_IN_ST));
      if (b) {
        byte |= mask;
      } else {
        byte &= ~mask;
      }
    }
  };

  using view = bitview<false>;
  using cview = bitview<true>;
  friend view;
  friend cview;

public:
  storage_type data[RAW_SIZE];

  constexpr bitarray() = default;

  constexpr bitarray(std::initializer_list<bool> ini) : bitarray() {
    size_t i = 0;
    for (auto it = ini.begin(); it < ini.end() && i < size_; ++it, ++i) {
      (*this)[i] = *it;
    }
    for (; i < size_; ++i) {
      (*this)[i] = 0;
    }
  }

  constexpr bitarray(const bitarray &) = default;

  constexpr bitarray(bitarray &&) = default;

  constexpr bitarray &operator=(const bitarray &) = default;

  constexpr bitarray &operator=(bitarray &&) = default;

  constexpr ~bitarray() = default;

  constexpr size_t size() const { return size_; }

  constexpr size_t raw_size() const { return RAW_SIZE; }

  constexpr view operator[](size_t index) {
    assert(index < size());
    return view(this, index);
  }

  constexpr cview operator[](size_t index) const {
    assert(index < size());
    return cview(this, index);
  }

  constexpr friend void swap(bitarray &l, bitarray &r) {
    std::swap_ranges(l.data, l.data + l.raw_size(), r.data);
  }
};
} // namespace utils
