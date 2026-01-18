#pragma once

#include <glbinding/gl/types.h>
#include <glm/ext/matrix_float4x4.hpp>

namespace agt::gl {

class Shader {
private:
    ::gl::GLuint shader_prog = 0;

    void release();

public:
    Shader(std::string_view vertex_shader, std::string_view fragment_shader);
    ~Shader();

    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&other) : shader_prog(other.shader_prog) {
        other.shader_prog = 0;
    }

    Shader &operator=(Shader &&other) {
        if(this != &other) {
            other.release();
            shader_prog = other.shader_prog;
            other.shader_prog = 0;
        }
        return *this;
    }

    ::gl::GLuint get() const { return shader_prog; }
    void use() const;

    // Shader::use should be called before this
    void setmat4(const std::string& name, glm::mat4 mat);
    // TODO: other types
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc23-extensions"

inline const ::gl::GLchar shapesVertSource[] = {
    #embed "shaders/vert.glsl"
    , '\0'
};

inline const ::gl::GLchar shapesFragSource[] = {
    #embed "shaders/frag.glsl"
    , '\0'
};

#pragma clang diagnostic pop

}
