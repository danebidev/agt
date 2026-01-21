#include <agt/ui/draw.hpp>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glbinding/gl43/gl.h>
#include <dwhbll/console/debug.hpp>

namespace agt::draw {

DrawCtx::DrawCtx(glm::vec2 size_)
    : vertices_changed(true),
      indices_changed(true)  {
    update_size(size_);
}

void DrawCtx::init_frame() const {
    // TODO: do various optimizations (deduplicate vertices, etc.)
}

void DrawCtx::finish_frame() const {
    textures.erase(std::remove_if(textures.begin(), textures.end(), [](Texture& tex) {
        return tex.status == Texture::Status::DELETE;
    }), textures.end());
    // vertices_changed = false;
    // indices_changed = false;
}

void DrawCtx::update_size(glm::vec2 size_) {
    if(size == size_)
        return;
    size = size_;
    proj = glm::ortho(0.0f, size.x, size.y, 0.0f, 0.0f, 100.0f);
}

void DrawCtx::set_clear_color(glm::vec3 color) {
    clear_color = color;
}

void DrawCtx::add_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3, int32_t tex) {
    vertices_changed = true;
    indices_changed = true;

    size_t size = vertices.size();

    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);

    indices.push_back(size);
    indices.push_back(size + 1);
    indices.push_back(size + 2);

    cmds.push_back(DrawCmd {
        .type = tex == -1 ? CmdType::TRIANGLES : CmdType::TRIANGLES_TEX,
        .count = 3,
        .first_index = indices.size() - 3,
        .texture = tex
    });
}

void DrawCtx::add_rect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, int32_t tex) {
    glm::vec2 p0 = pos;
    glm::vec2 p1 = pos + glm::vec2(size.x, 0);
    glm::vec2 p2 = pos + size;
    glm::vec2 p3 = pos + glm::vec2(0, size.y);

    float texWeight = tex == -1 ? 0.0f : 1.0f;

    Vertex v0 { { p0.x, p0.y, 0.0f }, color, { 0, 0 }, texWeight };
    Vertex v1 { { p1.x, p1.y, 0.0f }, color, { 1, 0 }, texWeight };
    Vertex v2 { { p2.x, p2.y, 0.0f }, color, { 1, 1 }, texWeight };
    Vertex v3 { { p3.x, p3.y, 0.0f }, color, { 0, 1 }, texWeight };

    add_triangle(v0, v1, v2, tex);
    add_triangle(v2, v3, v0, tex);
}

} // namespace agt::draw
