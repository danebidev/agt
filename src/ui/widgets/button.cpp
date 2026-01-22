#include <agt/ui/widgets/button.hpp>

#include <agt/ui/ui.hpp>

namespace agt::widget {

size Button::measure(ui::constraints c, ui::Node&, ui::Data& d) {
    size s = text.measure(c, text, d);
    return { s.width + 2 * padding, s.height + 2 * padding };
}

void Button::layout(rect r, ui::Node&, ui::Data& d) {
    rect v { r.x + padding, r.y + padding, r.z,
             r.w - 2 * padding, r.h - 2 * padding };
    text.layout(v, text, d);
}

void Button::draw(ui::Node& n, ui::Data& data) {
    text.draw(text, data);
    data.draw_ctx.add_rect({ n.layout_rect.x, n.layout_rect.y },
                           { n.layout_rect.w, n.layout_rect.h },
                           { 0.0f, 0.0f, 0.0f });
}

}
