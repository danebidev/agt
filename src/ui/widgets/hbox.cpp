#include <agt/ui/ui.hpp>
#include <agt/ui/widgets/hbox.hpp>

namespace agt::widget {

size HBox::measure(ui::constraints c, ui::Node&) const {
    uint32_t total_w = 0;
    uint32_t max_h   = 0;

    bool first = true;
    for (auto& child : children) {
        size cs = child.measure(c, const_cast<ui::Node&>(child));

        if (!first)
            total_w += spacing;
        first = false;

        total_w += cs.width;
        max_h = std::max(max_h, cs.height);
    }

    total_w = std::clamp(total_w, c.min_w, c.max_w);
    max_h = std::clamp(max_h,   c.min_h, c.max_h);

    return { total_w, max_h };
}

void HBox::layout(rect r, ui::Node&) const {
    uint32_t cursor_x = r.x;

    for (auto& child : children) {
        size cs = child.measure(
            ui::constraints{
                .min_w = 0,
                .max_w = r.w,
                .min_h = 0,
                .max_h = r.h
            },
            const_cast<ui::Node&>(child)
        );

        rect child_rect{
            cursor_x,
            r.y,
            r.z + 1,
            cs
        };

        child.layout(child_rect, const_cast<ui::Node&>(child));
        cursor_x += cs.width + spacing;
    }
}

void HBox::draw(draw::DrawCtx& ctx, ui::Node&) const {
    for (auto& child : children) {
        child.draw(ctx, const_cast<ui::Node&>(child));
    }
}


}; // namespace agt::ui
