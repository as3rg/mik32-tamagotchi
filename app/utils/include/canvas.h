#pragma once

#include "bitarray.h"
#include <cassert>

enum class canvas_orientation {
    HORIZONTAL,
    VERTICAL
};

template <size_t height_, size_t width_, canvas_orientation orientation_ = canvas_orientation::VERTICAL>
struct canvas {
private:
    using storage_t = bitarray<height_ * width_>;
public:
    constexpr canvas() = default;

    constexpr canvas(std::initializer_list<std::initializer_list<bool>> ini) : canvas(init_storage(ini)) {}

    constexpr canvas(const canvas&) = default;
    
    constexpr canvas(canvas&&) = default;

    constexpr canvas& operator=(const canvas&) = default;

    constexpr canvas& operator=(canvas&&) = default;

    constexpr size_t height() const {
        return height_;
    }

    constexpr size_t width() const {
        return width_;
    }

    constexpr auto operator()(size_t y, size_t x) {
        assert(y < height());
        assert(x < width());
        if constexpr (orientation_ == canvas_orientation::VERTICAL) {
            return data[x * height() + y];
        } else if constexpr (orientation_ == canvas_orientation::HORIZONTAL) {
            return data[y * width() + x];
        } else {
            static_assert(false, "Unsupported canvas orientation");
        }
    }

    constexpr auto operator()(size_t y, size_t x) const {
        assert(y < height());
        assert(x < width());
        if constexpr (orientation_ == canvas_orientation::VERTICAL) {
            return data[x * height() + y];
        } else if constexpr (orientation_ == canvas_orientation::HORIZONTAL) {
            return data[y * width() + x];
        } else {
            static_assert(false, "Unsupported canvas orientation");
        }
    }

    constexpr canvas_orientation orientation() const {
        return orientation_;
    }

    constexpr typename storage_t::storage_type* raw_data() {
        return data.data;
    }

    constexpr const typename storage_t::storage_type* raw_data() const {
        return data.data;
    }

    constexpr const size_t raw_size() const {
        return data.raw_size();
    }

    template <size_t other_height, size_t other_width, canvas_orientation other_orientation> 
    constexpr canvas& draw(const canvas<other_height, other_width, other_orientation>& other, size_t y, size_t x, bool invert = false) {
        for (size_t i = 0; x + i < width() && i < other.width(); ++i) {
            for (size_t j = 0; y + j < height() && j < other.height(); ++j) {
                (*this)(y + j, x + i) = invert ^ other(j, i);
            }
        }
        return *this;
    }

    constexpr canvas& fill_rectangle(size_t y, size_t x, size_t h, size_t w, bool color=true) {
        for (size_t i = 0; x + i < width() && i < w; ++i) {
            for (size_t j = 0; y + j < height() && j < h; ++j) {
                (*this)(y + j, x + i) = color;
            }
        }
        return *this;
    }

    constexpr canvas& draw_rectangle(size_t y, size_t x, size_t h, size_t w, size_t thickness = 1, bool color=true) {
        fill_rectangle(y, x, thickness, w, color);
        fill_rectangle(y, x, h, thickness, color);
        fill_rectangle(y + h - thickness, x, thickness, w, color);
        fill_rectangle(y, x + w - thickness, h, thickness, color);
        return *this;
    }
private:
    storage_t data {};
    
    constexpr static auto init_storage(std::initializer_list<std::initializer_list<bool>> ini) {
        canvas other;
        auto it_rows = ini.begin();
        for (size_t j = 0; j < other.height() && it_rows != ini.end(); ++j, ++it_rows) {
            auto it_cols = it_rows->begin();
            for (size_t i = 0; i < other.width() && it_cols != it_rows->end(); ++i, ++it_cols) {
                other(j, i) = *it_cols;
            }
        }
        return other;
    } 

};
