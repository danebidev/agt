#include <agt/ui/draw.hpp>
#include <dwhbll/console/debug.hpp>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glbinding/gl33/gl.h>

namespace agt::draw {

DrawCtx::DrawCtx(glm::vec2 size_)
    : vertices_changed(true),
      indices_changed(true)  {
    shaders.emplace_back(gl::shapesVertSource, gl::shapesFragSource);

    update_proj(size_);
}

void DrawCtx::finish_frame() const {
    // vertices_changed = false;
    // indices_changed = false;
}

void DrawCtx::update_proj(glm::vec2 size_) {
    if(size == size_)
        return;
    size = size_;

    glm::mat4 proj = glm::ortho(0.0f, size.x, 0.0f, size.y, 0.0f, 100.0f);
    for(auto &shader : shaders) {
        shader.use();
        int projLocation = ::gl::glGetUniformLocation(shader.get(), "proj");
        ASSERT(projLocation != -1);
        if(projLocation != -1) {
            ::gl::glUniformMatrix4fv(projLocation, 1, ::gl::GL_FALSE,
                                     &proj[0][0]);
            auto err = ::gl::glGetError();
            if(err != ::gl::GL_ZERO) {
                dwhbll::debug::panic("Failed to set projection matrix for shader: {}",
                                     static_cast<int>(err));
            }
        }
    }
}

void DrawCtx::set_clear_color(glm::vec3 color) {
    clear_color = color;
}

void DrawCtx::add_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {
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
        .type = CmdType::TRIANGLES,
        .count = 3,
        .first_index = indices.size() - 3,
        .shader_program = 0,
        .texture = 0
    });
}

void DrawCtx::add_rect(glm::vec2 pos, glm::vec2 size, glm::vec3 color) {
    glm::vec2 p0 = pos;
    glm::vec2 p1 = pos + glm::vec2(size.x, 0);
    glm::vec2 p2 = pos + size;
    glm::vec2 p3 = pos + glm::vec2(0, size.y);

    Vertex v0 { { p0.x, p0.y, 0.0f }, color };
    Vertex v1 { { p1.x, p1.y, 0.0f }, color };
    Vertex v2 { { p2.x, p2.y, 0.0f }, color };
    Vertex v3 { { p3.x, p3.y, 0.0f }, color };

    add_triangle(v0, v1, v2);
    add_triangle(v2, v3, v0);
}

} // namespace agt::draw
