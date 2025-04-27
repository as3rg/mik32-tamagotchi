#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace utils {
class list_default_tag;

template <typename T, typename Tag_ = list_default_tag> class list;

class list_element_base {
  template <typename T, typename Tag_> friend class list;

protected:
  list_element_base *prev;
  list_element_base *next;

  constexpr list_element_base() noexcept : prev(this), next(this) {}

  constexpr list_element_base(const list_element_base &) noexcept
      : list_element_base() {}

  constexpr list_element_base &
  operator=(const list_element_base &other) noexcept {
    if (&other != this) {
      unlink();
    }
    return *this;
  }

  constexpr list_element_base(list_element_base &&other) noexcept
      : list_element_base() {
    *this = std::move(other);
  }

  constexpr list_element_base &operator=(list_element_base &&other) noexcept {
    if (&other != this) {
      unlink();

      if (other.is_linked()) {
        prev = std::exchange(other.prev, &other);
        next = std::exchange(other.next, &other);
        prev->next = next->prev = this;
      }
    }

    return *this;
  }

  constexpr bool is_linked() const noexcept {
    assert((prev == this) == (next == this));
    return prev != this;
  }

  constexpr void unlink() noexcept {
    prev->next = next;
    next->prev = prev;
    prev = next = this;
  }

  constexpr ~list_element_base() { unlink(); }

  constexpr static void link(list_element_base *first,
                             list_element_base *second) noexcept {
    first->next = second;
    second->prev = first;
  }
};

template <class Tag> class list_element : list_element_base {
  template <typename T, typename Tag_> friend class list;

public:
  constexpr list_element() = default;
};

template <typename T, typename Tag> class list {
  using element_t = list_element<Tag>;

  static_assert(std::is_base_of_v<element_t, T>,
                "T must derive from list_element");

  template <class K> class list_iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = K;
    using reference = K &;
    using pointer = K *;
    using difference_type = ptrdiff_t;

    friend list_iterator<const K>;
    friend list;

    constexpr list_iterator() = default;

  private:
    constexpr explicit list_iterator(const list_element_base *el)
        : element(const_cast<list_element_base *>(el)) {}

  public:
    constexpr operator list_iterator<const K>() const noexcept {
      return list_iterator<const K>(element);
    }

    constexpr reference operator*() const { return *operator->(); }

    constexpr pointer operator->() const {
      return static_cast<K *>(static_cast<element_t *>(element));
    }

    constexpr list_iterator operator++(int) noexcept {
      list_iterator old = *this;
      ++(*this);
      return old;
    }

    constexpr list_iterator &operator++() noexcept {
      element = element->next;
      return *this;
    }

    constexpr list_iterator operator--(int) noexcept {
      list_iterator old = *this;
      --(*this);
      return old;
    }

    constexpr list_iterator &operator--() noexcept {
      element = element->prev;
      return *this;
    }

    constexpr friend bool operator==(const list_iterator &left,
                                     const list_iterator &right) noexcept {
      return left.element == right.element;
    }

    constexpr friend bool operator!=(const list_iterator &left,
                                     const list_iterator &right) noexcept {
      return !(left == right);
    }

    constexpr friend void swap(list_iterator &left,
                               list_iterator &right) noexcept {
      std::swap(left.element, right.element);
    }

  private:
    list_element_base *element;
  };

private:
  list_element_base bound;

public:
  using iterator = list_iterator<T>;
  using const_iterator = list_iterator<const T>;

public:
  constexpr list() noexcept = default;

  constexpr ~list() noexcept = default;

  constexpr list(const list &) = delete;

  constexpr list &operator=(const list &) = delete;

  constexpr list(list &&other) noexcept = default;

  constexpr list &operator=(list &&other) noexcept = default;

  constexpr bool empty() const noexcept { return begin() == end(); }

  constexpr size_t size() const noexcept {
    return std::distance(begin(), end());
  }

  constexpr T &front() noexcept {
    assert(!empty());
    return *begin();
  }

  constexpr const T &front() const noexcept {
    assert(!empty());
    return *begin();
  }

  constexpr T &back() noexcept {
    assert(!empty());
    return *last();
  }

  constexpr const T &back() const noexcept {
    assert(!empty());
    return *last();
  }

  constexpr void push_front(T &value) noexcept { insert(begin(), value); }

  constexpr void push_back(T &value) noexcept { insert(end(), value); }

  constexpr void pop_front() noexcept { erase(begin()); }

  constexpr void pop_back() noexcept { erase(last()); }

  constexpr void clear() noexcept { bound.unlink(); }

  constexpr iterator begin() noexcept { return iterator(bound.next); }

  constexpr const_iterator begin() const noexcept {
    return const_iterator(bound.next);
  }

  constexpr iterator end() noexcept { return iterator(&bound); }

  constexpr const_iterator end() const noexcept {
    return const_iterator(&bound);
  }

  constexpr iterator last() noexcept { return --end(); }

  constexpr const_iterator last() const noexcept { return --end(); }

  constexpr iterator insert(const_iterator pos, T &value) noexcept {
    element_t *val = &value;
    auto *next = get_base_ptr(pos);
    if (val != next) {
      val->unlink();
      list_element_base::link(next->prev, val);
      list_element_base::link(val, next);
    }
    return iterator(val);
  }

  constexpr iterator erase(const_iterator pos) noexcept {
    auto *node = get_base_ptr(pos);
    auto *next = node->next;
    node->unlink();
    return iterator(next);
  }

private:
  constexpr static list_element_base *
  get_base_ptr(const_iterator &it) noexcept {
    return const_cast<list_element_base *>(it.element);
  }
};
} // namespace utils
