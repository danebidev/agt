#pragma once

#include <agt/ui/text.hpp>
#include <agt/ui/ui.hpp>

namespace agt::widget {

struct Label {
    std::string text;

    Label(std::string s = "")
        : text(s) {}

    size measure(ui::constraints, ui::Node&, ui::Data&);
    void layout(rect, ui::Node&, ui::Data&);
    void draw(ui::Node&, ui::Data&);
};

}
