#include <agt/ui/widgets/label.hpp>

#include <dwhbll/console/Logging.h>

namespace agt::widget {

size Label::measure(ui::constraints, ui::Node&, ui::Data& data) {
    uint32_t width = 0;
    for(const auto c : text) {
        auto& character = data.text_render.get_char(c);
        width += (character.advance >> 6);
    }
    return { width, static_cast<uint32_t>(data.text_render.line_height) };
}

void Label::layout(rect, ui::Node&, ui::Data&) {}

void Label::draw(ui::Node& n, ui::Data& data) {
    glm::vec2 cur_pos = { static_cast<float>(n.layout_rect.x), 0 };
    float baseline_y = n.layout_rect.y + data.text_render.ascent;

    for(int i = 0; i < text.size(); ++i) {
        char c = text[i];
        int codepoint = 0;
        int extraBytes = 0;

        if ((c & 0b10000000) == 0) {
            codepoint = c;
        } 
        else if ((c & 0b11100000) == 0b11000000) {
            codepoint = c & 0b00011111;
            extraBytes = 1;
        } 
        else if ((c & 0b11110000) == 0b11100000) {
            codepoint = c & 0b00001111;
            extraBytes = 2;
        } 
        else if ((c & 0b11111000) == 0b11110000) {
            codepoint = c & 0b00000111;
            extraBytes = 3;
        } 
        else {
            dwhbll::console::warn("Invalid utf-8 character - skipping");
            continue;
        }

        for (int j = 0; j < extraBytes; ++j) {
            ++i;
            if (i >= text.size()) break;

            char t = text[i];
            codepoint = (codepoint << 6) | (t & 0b00111111);
        }

        auto& character = data.text_render.get_char(codepoint);
        int32_t index = data.text_render.get_glyph_texture(data.draw_ctx, codepoint);

        float top = baseline_y - character.bearing.height;

        data.draw_ctx.add_rect_fill({ cur_pos.x + character.bearing.width, top },
                                 character.sz, { 1.0f, 1.0f, 1.0f }, index);
        cur_pos.x += static_cast<float>(character.advance >> 6);
    }
}

}
