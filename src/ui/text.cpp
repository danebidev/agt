#include <agt/ui/text.hpp>

#include <glm/vec2.hpp>
#include <glbinding/gl43/gl.h>
#include <dwhbll/console/debug.hpp>

using namespace ::gl;

namespace agt::ui {

Character::Character(FT_Face& face, int c) {
    FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);
    if(error)
        dwhbll::debug::panic("Failed to load character '{}' while loading font: {:X}",
                             static_cast<char>(c), error); // hacky, but good enough

    sz = { face->glyph->bitmap.width, face->glyph->bitmap.rows };
    bearing = { static_cast<uint32_t>(face->glyph->bitmap_left),
                static_cast<uint32_t>(face->glyph->bitmap_top) };
    advance = face->glyph->advance.x;

    size_t buf_size = sz.width * sz.height;
    if(buf_size)
        pixels.assign(face->glyph->bitmap.buffer, face->glyph->bitmap.buffer + buf_size);
}

TextRendering::TextRendering() {
    FT_Error error = FT_Init_FreeType(&ft);
    if(error)
        dwhbll::debug::panic("Failed loading freetype: {:X}", error);
    load_font();
}

TextRendering::~TextRendering() {
    if(face)
        FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRendering::load_font() {
    if(face)
        FT_Done_Face(face);
    if(FT_New_Face(ft, cur_font.c_str(), 0, &face))
        dwhbll::debug::panic("Failed loading font");

    // TODO: make this configurable
    FT_Set_Pixel_Sizes(face, 0, 24);  

    ascent = face->size->metrics.ascender >> 6;
    descent = face->size->metrics.descender >> 6;
    line_height = face->size->metrics.height >> 6;

    characters.clear();

    // Only load the first 128 ascii characters initially.
    // Other characters will be lazy loaded when necessary
    for (unsigned char c = 0; c < 128; c++) {
        characters.insert(std::pair<char8_t, Character>(c, Character(face, c)));
    }
}

Character& TextRendering::get_char(int c) {
    auto it = characters.find(c);
    if (it == characters.end()) {
        characters.emplace(c, Character(face, static_cast<uint8_t>(c)));
        it = characters.find(c);
    }
    return it->second;
}

int TextRendering::get_glyph_texture(draw::DrawCtx& ctx, int c) {
    Character& ch = get_char(c);

    for (int i = 0; i < ctx.textures.size(); ++i) {
        if (ctx.textures[i].src_buf == ch.pixels.data() && 
            ctx.textures[i].sz.width == ch.sz.width &&
            ctx.textures[i].sz.height == ch.sz.height) {
            return i;
        }
    }

    // Not found
    draw::Texture t;
    t.id = 0;
    t.sz = ch.sz;
    t.src_buf = ch.pixels.data();
    t.status = draw::Texture::Status::CREATE;

    size_t size = t.sz.width * t.sz.height;
    t.buf.resize(size * 4);
    for (size_t i = 0; i < size; ++i) {
        uint8_t a = ch.pixels[i];
        size_t out = i * 4;
        t.buf[out + 0] = 255;
        t.buf[out + 1] = 255;
        t.buf[out + 2] = 255;
        t.buf[out + 3] = a;
    }

    ctx.textures.push_back(std::move(t));
    return ctx.textures.size() - 1;
}

}
