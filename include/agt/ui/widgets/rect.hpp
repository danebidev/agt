#pragma once

#include "agt/ui/ui.hpp"

namespace agt::widget {

struct Rectangle {
    size preferred;
    glm::vec3 color;

    Rectangle(uint32_t width, uint32_t height, glm::vec3 color_) 
        : preferred({ width, height }), color(color_) {};

    Rectangle(size s, glm::vec3 color_) : preferred(s), color(color_) {}

    size measure(ui::constraints c, ui::Node&) const {
        return preferred;
    }
    void layout(rect r, ui::Node& n) const {}

    void draw(draw::DrawCtx& ctx, ui::Node& n) const {
        ctx.add_rect({ n.layout_rect.x, n.layout_rect.y }, 
                     { n.layout_rect.w, n.layout_rect.h },
                       color);
    }
};

}
