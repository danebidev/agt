#pragma once

#include <agt/ui/ui.hpp>

namespace agt::ui {

struct HBox {
    std::vector<Node> children;
    uint32_t spacing = 0;

    HBox() = default;
    HBox(std::vector<Node> c, uint32_t s = 0)
        : children(std::move(c)), spacing(s) {}

    size measure(constraints c, Node&) const;
    void layout(rect r, Node&) const;
    void draw(draw::DrawCtx& ctx, Node&) const;
};

}
