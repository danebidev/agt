#pragma once

#include "agt/gl/shaders.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec3.hpp>
#include <span>
#include <vector>

namespace agt::draw {

enum class CmdType {
    TRIANGLES
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

struct DrawCmd {
    CmdType type;

    size_t count;
    size_t first_index;

    // Index into DrawCtx::shaders
    uint16_t shader_program;
};

/*
 * Contains all information necessary to draw a frame.
 *
 * CMDS contains all the draw commands to be executed 
 * in the order they appear in.
 * Other fields contain extra information that can be
 * used by the specific commands.
 *
 * The UI lib should return a const reference to this, so only
 * CMDS can be modified, since it gets recreated on each frame
 */
class DrawCtx {
public:
    mutable std::vector<DrawCmd> cmds;
    std::vector<gl::Shader> shaders;

    // The backend should clear the screen with
    // this color at start of every frame
    glm::vec3 clear_color; 

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    DrawCtx(glm::vec2 size);
    
    void update_proj(glm::vec2 size);
    void set_clear_color(glm::vec3 color);

    void add_triangle(std::span<Vertex> vertices);
};

} // namespace agt::draw
