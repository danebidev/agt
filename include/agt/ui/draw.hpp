#pragma once

#include <agt/gl/shaders.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace agt::draw {

enum class CmdType {
    TRIANGLES
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

struct Texture {
    uint32_t id;
    bool updated;
};

struct DrawCmd {
    CmdType type;

    size_t count;
    size_t first_index;

    // Index into DrawCtx::shaders
    uint16_t shader_program;
    // Index into DrawCtx::textures
    uint16_t texture;
};

/*
 * Contains all information necessary to draw a frame.
 *
 * CMDS contains all the draw commands to be executed 
 * in the order they appear in.
 * Other fields contain extra information that can be
 * used by the specific commands.
 */
struct DrawCtx {
public:
    glm::vec2 size;

    std::vector<DrawCmd> cmds;
    std::vector<gl::Shader> shaders;
    std::vector<Texture> textures;

    // The backend should clear the screen with
    // this color at start of every frame
    glm::vec3 clear_color; 

    mutable bool vertices_changed;
    std::vector<Vertex> vertices;

    mutable bool indices_changed;
    std::vector<uint16_t> indices;

    void finish_frame() const;
    void update_proj(glm::vec2 size);

    DrawCtx(glm::vec2 size);

    void set_clear_color(glm::vec3 color);

    void add_triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
    void add_rect(glm::vec2 pos, glm::vec2 size, glm::vec3 color);
};

} // namespace agt::draw
