#include <agt/ui/ui.hpp>

namespace agt::ui {

void UIRoot::compute_layout() {
    node.layout({ 0, 0, 1, s }, node, data);
}

draw::DrawCtx& UIRoot::draw() {
    draw_ctx.cmds.clear();
    draw_ctx.indices.clear();
    draw_ctx.vertices.clear();
    draw_ctx.clear_color = bg_color;

    node.draw(node, data);
    return draw_ctx;
}

void UIRoot::set_size(uint32_t width, uint32_t height) {
    s = { width, height };
    draw_ctx.update_size(s);
}

} // namespace agt::ui
