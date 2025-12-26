#include <agt/ui/ui.hpp>

namespace agt::ui {

const draw::DrawCtx& UIRoot::compute_layout() {
    if(!draw_ctx)
        draw_ctx = std::make_unique<draw::DrawCtx>(root_size);

    draw_ctx->clear_color = color;

    size measured = measure(root_size);
    layout({ 0, 0, 0, measured });
    paint(*draw_ctx);

    return *draw_ctx;
}

size HBox::measure(size constraint) const {
    size s;
    for(const auto& elem : elements) {
        size elem_measure = elem->measure({ constraint.width - s.width, constraint.height });
        s.height = std::max(s.height, elem_measure.height);
        s.width += elem_measure.width;
    }
    return s;
}

void HBox::layout(rect r) {
    element_rect = r;
    uint32_t x = r.x;
    for(const auto& e : elements) {
        auto measure = e->measure({ r.size.width - x, r.size.height });
        e->layout({
            x, r.y, r.z,
            measure
        });
        x += measure.width;
    }
}

void Rectangle::paint(draw::DrawCtx& ctx) const {
    glm::vec2 pos = {
        static_cast<float>(element_rect.x),
        static_cast<float>(element_rect.y)
    };

    glm::vec2 size = {
        static_cast<float>(element_rect.size.width),
        static_cast<float>(element_rect.size.height)
    };

    ctx.add_rect(pos, size, { 0, 0, 1.0 });
}

}; // namespace agt::ui
