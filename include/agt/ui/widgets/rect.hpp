#pragma once

#include "agt/ui/ui.hpp"

namespace agt::ui {

class Rectangle {
    size preferred;

public:
    Rectangle(uint32_t width, uint32_t height) : preferred({ width, height }) {};
    Rectangle(size s) : preferred(s) {}

    size measure(constraints c, Node&) const {
        return preferred;
    }
    void layout(rect r, Node& n) const {}

    void draw(draw::DrawCtx& ctx, Node& n) const {
        ctx.add_rect({ n.layout_rect.x, n.layout_rect.y }, 
                     { n.layout_rect.w, n.layout_rect.h },
                     { 0.1, 0.2, 0.3 });
    }
};

}
