#include <agt/ui/draw.hpp>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glbinding/gl43/gl.h>
#include <dwhbll/console/debug.hpp>

namespace agt::draw {

// TODO: There should be a better way to do this.
//       Perhaps keep the list sorted so we can do a binary search?
size_t DrawCtx::add_vertex(Vertex& v) {
    for(size_t i = 0; i < vertices.size(); ++i) {
        if(v == vertices[i])
            return i;
    }
    vertices.push_back(v);
    return vertices.size() - 1;
}

std::vector<size_t> DrawCtx::add_vertices(const std::vector<Vertex>& v) {
    constexpr auto M = std::numeric_limits<size_t>::max();
    std::vector<size_t> pos(v.size(), M);
    for(size_t i = 0; i < vertices.size(); ++i) {
        const auto& vert = vertices[i];
        for(size_t j = 0; j < v.size(); ++j) {
            const auto& c = v[j];
            if(vert == c) {
                ASSERT(pos[j] == M);
                pos[j] = i;
            }
        }
    }

    for(size_t i = 0; i < v.size(); ++i) {
        if(pos[i] == M) {
            vertices.push_back(v[i]);
            pos[i] = vertices.size() - 1;
        }
    }

    return pos;
}

DrawCtx::DrawCtx(glm::vec2 size_)
    : ctx_changed(true) {
    update_size(size_);
}

void DrawCtx::finish_frame() const {
    textures.erase(std::remove_if(textures.begin(), textures.end(), [](Texture& tex) {
        return tex.status == Texture::Status::DELETE;
    }), textures.end());
    ctx_changed = false;
}

void DrawCtx::update_size(glm::vec2 size_) {
    if(size == size_)
        return;
    size = size_;
    proj = glm::ortho(0.0f, size.x, size.y, 0.0f, 0.0f, 100.0f);
    ctx_changed = true;
}

void DrawCtx::set_clear_color(glm::vec3 color) {
    clear_color = color;
}

void DrawCtx::add_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3, int32_t tex) {
    std::vector<size_t> n = add_vertices({ v1, v2, v3 });

    indices.insert(indices.end(), n.begin(), n.end());

    cmds.push_back(DrawCmd {
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
