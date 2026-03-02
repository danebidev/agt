#pragma once

#include <glm/vec2.hpp>

#include <variant>

#include <linux/input-event-codes.h>

namespace agt::input {

enum class ButtonState { UP, DOWN };

struct MouseMotion {
    uint32_t time;
    glm::uvec2 pos;
};

struct MouseClick {
    uint32_t time;
    // Same codes as wayland, meaning button codes as
    // defined in <linux/input-event-codes.h>.
    // Might want to change it to some internal enum at some point in the future.
    uint32_t button;
    ButtonState state;
};

using InputEvent = std::variant<MouseMotion, MouseClick>;

}
