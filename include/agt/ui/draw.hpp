#pragma once

#include <agt/utils.hpp>
#include <agt/gl/shaders.hpp>

namespace agt::draw {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;
    float texWeight;

    friend bool operator==(const Vertex& lhs, const Vertex& rhs) {
        return lhs.pos == rhs.pos 
                && lhs.color == rhs.color 
                && lhs.uv == rhs.uv
                && lhs.texWeight == rhs.texWeight;
    }

    friend bool operator!=(const Vertex& lhs, const Vertex& rhs) { return !(lhs == rhs); }
};

struct Texture {
    enum class Status {
        OK,
        CREATE,
        UPDATE,
        DELETE
    };

    uint32_t id;
    size sz;
    Status status;

    // RGBA data that must be uploaded
    std::vector<uint8_t> buf;

    /**
     * For certain textures (like text glyphs) we have to convert to RGBA from
     * some other format (like RED), so `buf` will contain the converted buffer
     * while `src_buf` contains the original unconverted buffer for deduplication
     */
    const void* src_buf = nullptr;
};

struct DrawCmd {
    size_t count;
    size_t first_index;

    // Index into DrawCtx::textures
    // -1 if a texture shouldn't be used
    int texture;
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
private:
    size_t add_vertex(Vertex& v);
    std::vector<size_t> add_vertices(const std::vector<Vertex>& v);

public:
    glm::vec2 size;
    glm::mat4 proj;

    std::vector<DrawCmd> cmds;
    // TODO: maybe instead of having multiple textures
    //       they should be combined in an atlas?
    mutable std::vector<Texture> textures;

    // The backend should clear the screen with
    // this color at start of every frame
    glm::vec3 clear_color; 

    mutable bool ctx_changed;
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    void finish_frame() const;
    void update_size(glm::vec2 size);

    DrawCtx(glm::vec2 size);

    void set_clear_color(glm::vec3 color);

    // If tex == -1, don't use a texture
    void add_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3, int32_t tex = -1);
    void add_rect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, int32_t tex = -1);
};

} // namespace agt::draw
