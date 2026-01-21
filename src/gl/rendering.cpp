#include <agt/gl/rendering.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>
#include <glbinding/glbinding.h>

#include <utility>

using namespace ::gl;

namespace agt::gl {

#ifndef NDEBUG
std::string source_to_str(GLenum source) {
    switch(source) {
    case GL_DEBUG_SOURCE_API:
        return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "WM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "SHADER_COMP";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
        return "APP";
    case GL_DEBUG_SOURCE_OTHER:
        return "OTHER";
    }

    std::unreachable();
}

std::string type_to_str(GLenum type) {
    switch(type) {
    case GL_DEBUG_TYPE_ERROR:
        return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DEPRECATED";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UB";
    case GL_DEBUG_TYPE_PORTABILITY:
        return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
        return "PERFORMANCE";
    case GL_DEBUG_TYPE_MARKER:
        return "MARKER";
    case GL_DEBUG_TYPE_PUSH_GROUP:
        return "PUSH_GROUP";
    case GL_DEBUG_TYPE_POP_GROUP:
        return "POP_GROUP";
    case GL_DEBUG_TYPE_OTHER:
        return "OTHER";
    }

    std::unreachable();
}

void gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity,
              GLsizei length, const GLchar* message, const void* userParam) {
    switch(severity) {
    case GLenum::GL_DEBUG_SEVERITY_HIGH:
        dwhbll::debug::panic("OpenGL ERROR\ntype: {}\nSource: {}\n\n{}", type_to_str(type),
                             source_to_str(source), message);
        break;
    case GLenum::GL_DEBUG_SEVERITY_MEDIUM:
        dwhbll::console::warn("OpenGL WARNING\ntype: {}\nSource: {}\n\n{}", type_to_str(type),
                             source_to_str(source), message);
        break;
    case GLenum::GL_DEBUG_SEVERITY_LOW:
        dwhbll::console::debug("OpenGL Debug\ntype: {}\nSource: {}\n\n{}", type_to_str(type),
                             source_to_str(source), message);
        break;
    case GLenum::GL_DEBUG_SEVERITY_NOTIFICATION:
        dwhbll::console::debug("OpenGL Info\ntype: {}\nSource: {}\n\n{}", type_to_str(type),
                             source_to_str(source), message);
        break;
    }
}
#endif

Renderer::Renderer(wayland::Display& display) {
    egl_display = eglGetDisplay(display.display());
    if(egl_display == EGL_NO_DISPLAY)
        dwhbll::debug::panic("egl: failed to create display");

    if(!eglInitialize(egl_display, NULL, NULL))
        dwhbll::debug::panic("egl: failed to initialize");

    eglBindAPI(EGL_OPENGL_API);

    EGLint num_config;
    EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    eglChooseConfig(egl_display, attribs, &egl_config, 1, &num_config);

    EGLint ctx_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 3,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifndef NDEBUG
        EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
        EGL_NONE
    };
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, ctx_attribs);

    glbinding::initialize(eglGetProcAddress, false);
    make_current(EGL_NO_SURFACE);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&gl_debug, NULL);
#endif
}

Renderer::~Renderer() {
    glUseProgram(0);
    shader.reset();
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(egl_display, egl_context);
    eglTerminate(egl_display);
}

void Renderer::make_current(EGLSurface surface) {
    eglMakeCurrent(egl_display, surface, surface, egl_context);
}

void Renderer::init_shader() {
    if(!shader)
        shader = std::make_unique<Shader>(gl::shapesVertSource, gl::shapesFragSource);
}

} // namespace agt::gl
