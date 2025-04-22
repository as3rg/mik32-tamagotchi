#include "bitarray.h"

enum class canvas_orientation {
    HORIZONTAL,
    VERTICAL
};

template <size_t height_, size_t width_, canvas_orientation orientation_ = canvas_orientation::VERTICAL>
struct canvas {
private:
    using storage_t = bitarray<height_ * width_>;
public:
    always_inline constexpr canvas() = default;

    always_inline constexpr canvas(std::initializer_list<std::initializer_list<bool>> ini) : canvas(init_storage(ini)) {}

    always_inline constexpr canvas(const canvas&) = default;
    
    always_inline constexpr canvas(canvas&&) = default;

    always_inline constexpr canvas& operator=(const canvas&) = default;

    always_inline constexpr canvas& operator=(canvas&&) = default;

    always_inline constexpr size_t height() const {
        return height_;
    }

    always_inline constexpr size_t width() const {
        return width_;
    }

    always_inline constexpr auto operator()(size_t y, size_t x) {
        if constexpr (orientation() == canvas_orientation::VERTICAL) {
            return data[x * height() + y];
        } else if constexpr (orientation() == canvas_orientation::HORIZONTAL) {
            return data[y * width() + x];
        } else {
            static_assert(false, "Unsupported canvas orientation");
        }
    }

    always_inline constexpr auto operator()(size_t y, size_t x) const {
        if constexpr (orientation() == canvas_orientation::VERTICAL) {
            return data[x * height() + y];
        } else if constexpr (orientation() == canvas_orientation::HORIZONTAL) {
            return data[y * width() + x];
        } else {
            static_assert(false, "Unsupported canvas orientation");
        }
    }

    always_inline constexpr canvas_orientation orientation() const {
        return orientation_;
    }

    always_inline constexpr storage_t::storage_type* raw_data() {
        return data.data;
    }

    always_inline constexpr const storage_t::storage_type* raw_data() const {
        return data.data;
    }

    always_inline constexpr const size_t raw_size() const {
        return data.raw_size();
    }

    template <size_t other_height, size_t other_width, canvas_orientation other_orientation> 
    always_inline constexpr canvas& draw(const canvas<other_height, other_width, other_orientation>& other, size_t y, size_t x) {
        for (size_t i = 0; x + i < width() && i < other.width(); ++i) {
            for (size_t j = 0; y + j < height() && j < other.height(); ++j) {
                (*this)(y + j, x + i) = other(j, i);
            }
        }
        return *this;
    }
private:
    storage_t data {};
    
    always_inline constexpr static auto init_storage(std::initializer_list<std::initializer_list<bool>> ini) {
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
