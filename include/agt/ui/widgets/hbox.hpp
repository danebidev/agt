#pragma once

#include <agt/ui/ui.hpp>

namespace agt::widget {

struct HBox {
    std::vector<ui::Node> children;
    uint32_t spacing = 0;

    HBox() = default;

    template<typename... Ts>
    HBox(Ts&&... args) {
        (children.emplace_back(std::forward<Ts>(args)), ...);
    }

    size measure(ui::constraints, ui::Node&, ui::Data&);
    void layout(rect, ui::Node&, ui::Data&);
    void draw(ui::Node&, ui::Data&);
};

}
