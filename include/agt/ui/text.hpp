#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/ext/vector_int2.hpp>
#include <map>
#include <filesystem>
#include <dwhbll/console/debug.hpp>

#include <agt/ui/ui.hpp>

namespace agt::ui {

struct Character {
    std::vector<uint8_t> pixels;

    size sz;
    size bearing;
    // in 1/64th pixels
    uint32_t advance;
    uint32_t min_y, max_y;

    Character(FT_Face& face, unsigned char c);
};

struct TextRendering {
private:
    FT_Library ft;
    FT_Face face = nullptr;

    std::map<wchar_t, Character> characters;

public:
    // TODO: load from fontconfig
    std::filesystem::path cur_font = "/usr/share/fonts/truetype/noto/NotoSerif-Light.ttf";

    int ascent = 0;
    int descent = 0;
    int line_height = 0;

    TextRendering();
    ~TextRendering();

    void load_font();

    Character& get_char(wchar_t c);
    int32_t get_glyph_texture(draw::DrawCtx& ctx, wchar_t c);
};

}
