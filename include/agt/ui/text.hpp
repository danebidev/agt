#pragma once

#include <agt/ui/ui.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <filesystem>

namespace agt::ui {

struct Character {
    std::vector<uint8_t> pixels;

    size sz;
    size bearing;
    // in 1/64th pixels
    uint32_t advance;
    uint32_t min_y, max_y;

    Character(FT_Face& face, int c);
};

struct TextRendering {
private:
    FT_Library ft;
    FT_Face face = nullptr;

    std::map<int, Character> characters;

public:
    // TODO: load from fontconfig
    std::filesystem::path cur_font = "/usr/share/fonts/truetype/noto/NotoSerif-Light.ttf";

    int ascent = 0;
    int descent = 0;
    int line_height = 0;

    TextRendering();
    ~TextRendering();

    void load_font();

    Character& get_char(int c);
    int32_t get_glyph_texture(draw::DrawCtx& ctx, int c);
};

}
