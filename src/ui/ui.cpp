#include <agt/ui/ui.hpp>

namespace agt::ui {

const draw::DrawCtx* UIRoot::compute_layout() {
    if(!draw_ctx)
        draw_ctx = new draw::DrawCtx(root_size);

    size measured_size = measure(root_size);
    layout({ 0, 0, 0, measured_size });

    draw_ctx->clear_color = color;

    return draw_ctx;
}

size HBox::measure(size constraint) {
    size s;
    for(const auto& elem : elements) {
        size elem_measure = elem->measure({ constraint.width - s.width, constraint.height });
        s.height = std::max(s.height, elem_measure.height);
        s.width += elem_measure.width;
    }
    return s;
}

void HBox::layout(rect rect) {
    element_rect = rect;

}

}; // namespace agt::ui
