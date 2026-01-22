#pragma once

#include <agt/ui/text.hpp>
#include <agt/ui/ui.hpp>

namespace agt::widget {

struct Label {
    ui::TextRendering& render;
    std::string text;

    Label(ui::TextRendering& render_, std::string s = "")
        : render(render_), text(s) {}

    size measure(ui::constraints, ui::Node&) const;
    void layout(rect, ui::Node&) const;
    void draw(draw::DrawCtx&, ui::Node&) const;
};

}
