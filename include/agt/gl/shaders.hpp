#pragma once

#include <glbinding/gl/types.h>

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
        }
        return *this;
    }

    ::gl::GLuint get() { return shader_prog; }
    void use();
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc23-extensions"

inline const ::gl::GLchar shapesVertSource[] = {
    #embed "shaders/shapes/vert.glsl"
};

inline const ::gl::GLchar shapesFragSource[] = {
    #embed "shaders/shapes/frag.glsl"
};

#pragma clang diagnostic pop

}
