#include <agt/gl/shaders.hpp>

#include <dwhbll/console/Logging.h>
#include <dwhbll/console/debug.hpp>
#include <glbinding/gl43/gl.h>

using namespace gl;

namespace agt::gl {

void Shader::release() {
    if(shader_prog != 0) {
        glDeleteProgram(shader_prog);
        shader_prog = 0;
    }
}

Shader::Shader(std::string_view vertex_shader, std::string_view fragment_shader) {
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* source = vertex_shader.data();
    glShaderSource(vertex, 1, (GLchar**)&source, nullptr);
    glCompileShader(vertex);

    int succ;
    char message[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &succ);
    if(!succ) {
        glGetShaderInfoLog(vertex, sizeof(message), nullptr, message);
        dwhbll::debug::panic(message);
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_shader.data();
    glShaderSource(fragment, 1, (GLchar**)&source, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &succ);
    if(!succ) {
        glGetShaderInfoLog(fragment, sizeof(message), nullptr, message);
        dwhbll::debug::panic(message);
    }

    shader_prog = glCreateProgram();
    if(!shader_prog)
        dwhbll::debug::panic("Failed to create shader program: {}", 
                             static_cast<uint>(glGetError()));

    glAttachShader(shader_prog, vertex);
    glAttachShader(shader_prog, fragment);
    glLinkProgram(shader_prog);

    glGetProgramiv(shader_prog, GL_LINK_STATUS, &succ);
    if(!succ) {
        glGetProgramInfoLog(shader_prog, sizeof(message), nullptr, message);
        dwhbll::debug::panic(message);
    }

    use();
    int loc = glGetUniformLocation(shader_prog, "tex");
    if (loc != -1)
        glUniform1i(loc, 0);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    release();
}

void Shader::use() const {
    ASSERT(shader_prog != 0);
    glUseProgram(shader_prog);
}

void Shader::setmat4(const std::string& name, glm::mat4 mat) {
    int loc = glGetUniformLocation(shader_prog, name.c_str());
    ASSERT(loc != -1);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
    auto err = glGetError();
    if(err != GL_NO_ERROR) {
        dwhbll::debug::panic("Failed to set projection matrix for shader: {}",
                             static_cast<int>(err));
    }
}

}
