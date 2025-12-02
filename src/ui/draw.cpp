#include "dwhbll/console/debug.hpp"
#include <agt/ui/draw.hpp>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glbinding/gl33/gl.h>

namespace agt::draw {

DrawCtx::DrawCtx(glm::vec2 size) {
    shaders.emplace_back(gl::shapesVertSource, gl::shapesFragSource);
    update_proj(size);
}

void DrawCtx::update_proj(glm::vec2 size) {
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

void DrawCtx::add_triangle(std::span<Vertex> vert) {
    ASSERT(vert.size() == 3);

    vertices.insert(vertices.end(), vert.begin(), vert.end());
    indices.push_back(vertices.size() - 1);
    indices.push_back(vertices.size() - 2);
    indices.push_back(vertices.size() - 3);

    cmds.push_back(DrawCmd {
        .type = CmdType::TRIANGLES,
        .count = 3,
        .first_index = indices.size() - 3,
        .shader_program = 0,
    });
}

} // namespace agt::draw
