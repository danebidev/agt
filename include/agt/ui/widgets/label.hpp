#pragma once

#include <agt/ui/text.hpp>
#include <agt/ui/ui.hpp>

namespace agt::ui {

struct Label {
    TextRendering& render;
    std::string text;

    Label(TextRendering& render_, std::string s = "")
        : render(render_), text(s) {}

    size measure(constraints, Node&) const;
    void layout(rect, Node&) const;
    void draw(draw::DrawCtx&, Node&) const;
};

}
