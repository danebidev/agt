#pragma once

#include <dwhbll/console/debug.hpp>
#include <dwhbll/utils/utils.hpp>
#include <glbinding/gl33/gl.h>
#include <glm/glm.hpp>

namespace agt::gl {

class Renderer;

enum class GLObject {
    VBO,
    VAO,
    EBO
};

template <GLObject T>
class GLHandle {
private:
    void release() {
        if(!id)
            return;

        if constexpr (T == GLObject::VBO || T == GLObject::EBO)
            ::gl::glDeleteBuffers(1, &id);
        else if constexpr (T == GLObject::VAO)
            ::gl::glDeleteVertexArrays(1, &id);

        id = 0;
    }

public:
    ::gl::GLuint id = 0;

    GLHandle() {
        if constexpr (T == GLObject::VBO || T == GLObject::EBO)
            ::gl::glGenBuffers(1, &id);
        else if constexpr (T == GLObject::VAO)
            ::gl::glGenVertexArrays(1, &id);
    }

    ~GLHandle() {
        release();
    }

    GLHandle(const GLHandle &) = delete;
    GLHandle &operator=(const GLHandle &) = delete;

    GLHandle(GLHandle &&other) : id(other.id) {
        other.id = 0;
    }

    GLHandle &operator=(GLHandle &&other) {
        if(this != &other) {
            if(id != 0)
                release();
            id = other.id;
            other.id = 0;
        }
        return *this;
    }
};

} // namespace agt::gl
