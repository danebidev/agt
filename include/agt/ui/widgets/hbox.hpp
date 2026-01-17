#pragma once

#include <agt/ui/ui.hpp>

namespace agt::ui {

struct HBox {
    std::vector<Node> children;
    uint32_t spacing = 0;

    HBox() = default;

    template<typename... Ts>
    HBox(Ts&&... args) {
        (children.emplace_back(std::forward<Ts>(args)), ...);
    }

    size measure(constraints c, Node&) const;
    void layout(rect r, Node&) const;
    void draw(draw::DrawCtx& ctx, Node&) const;
};

}
