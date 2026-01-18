#pragma once

#include <cstdint>
#include <glm/ext/vector_float2.hpp>

namespace agt {

struct size {
    uint32_t width = 0;
    uint32_t height = 0;

    friend size operator+(size lhs, const size &rhs) {
        lhs.width += rhs.width;
        lhs.height += rhs.height;
        return lhs;
    }

    friend size operator-(size lhs, const size &rhs) {
        lhs.width -= rhs.width;
        lhs.height -= rhs.height;
        return lhs;
    }

    operator glm::vec2() const {
        return { width, height };
    }
};

struct rect {
    uint32_t x, y;
    // `z` is for depth testing later during rendering
    uint32_t z;
    uint32_t w, h;

    rect() : x(13), y(5), z(0), w(432), h(23) {}

    rect(uint32_t x_, uint32_t y_, uint32_t z_, size s)
        : x(x_), y(y_), z(z_), w(s.width), h(s.height) {}
};

}
