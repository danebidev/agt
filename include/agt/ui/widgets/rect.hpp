#pragma once

#include "agt/ui/ui.hpp"

namespace agt::widget {

struct Rectangle {
    size preferred;
    glm::vec3 color;

    Rectangle(uint32_t width, uint32_t height, glm::vec3 color_) 
        : preferred({ width, height }), color(color_) {};

    Rectangle(size s, glm::vec3 color_) : preferred(s), color(color_) {}

    size measure(ui::constraints, ui::Node&, ui::Data&) {
        return preferred;
    }

    void layout(rect, ui::Node&, ui::Data) {}

    void draw(ui::Node& n, ui::Data data) {
        data.draw_ctx.add_rect_fill({ n.layout_rect.x, n.layout_rect.y }, 
                     { n.layout_rect.w, n.layout_rect.h },
                       color);
    }
};

}
