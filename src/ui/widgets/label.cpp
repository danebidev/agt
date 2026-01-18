#include <agt/ui/widgets/label.hpp>

namespace agt::ui {

size Label::measure(constraints, Node&) const {
    uint32_t width = 0;
    for(const auto c : text) {
        auto& character = render.get_char(c);
        width += (character.advance >> 6);
    }

    return { width, static_cast<uint32_t>(render.line_height) };
}

void Label::layout(rect, Node&) const {}

void Label::draw(draw::DrawCtx& ctx, Node& n) const {
    glm::vec2 cur_pos = { static_cast<float>(n.layout_rect.x), 0 };
    float baseline_y = n.layout_rect.y + render.ascent;

    for(const auto c : text) {
        auto& character = render.get_char(c);
        int32_t index = render.get_glyph_texture(ctx, c);

        float top = baseline_y - character.bearing.height;

        ctx.add_rect({ cur_pos.x + character.bearing.width, top },
                       character.sz, { 1.0f, 1.0f, 1.0f }, index);
        cur_pos.x += static_cast<float>(character.advance >> 6);
    }
}

}
