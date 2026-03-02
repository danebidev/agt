#include <agt/gl/gl.hpp>

#include <agt/ui/draw.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>
#include <glbinding/glbinding.h>

#include <EGL/eglext.h>

#include <utility>

using namespace ::gl;
using namespace dwhbll::console;
using namespace dwhbll::debug;

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
        panic("OpenGL ERROR\ntype: {}\nSource: {}\n\n{}", type_to_str(type),
                             source_to_str(source), message);
        break;
    case GLenum::GL_DEBUG_SEVERITY_MEDIUM:
        warn("OpenGL WARNING\ntype: {}\nSource: {}\n\n{}", type_to_str(type),
                             source_to_str(source), message);
        break;
    case GLenum::GL_DEBUG_SEVERITY_LOW:
        debug("OpenGL Debug\ntype: {}\nSource: {}\n\n{}", type_to_str(type),
                             source_to_str(source), message);
        break;
    case GLenum::GL_DEBUG_SEVERITY_NOTIFICATION:
        debug("OpenGL Info\ntype: {}\nSource: {}\n\n{}", type_to_str(type),
                             source_to_str(source), message);
        break;
    }
}

void egl_debug(EGLenum error,const char *command,EGLint messageType,EGLLabelKHR threadLabel,EGLLabelKHR objectLabel,const char* message) {
    panic("{:X}: EGL_DEBUG", error);
    // TODO
}

PFNEGLDEBUGMESSAGECONTROLKHRPROC eglDebugMessageControlKHR;
#endif

Renderer::Renderer(backend::Backend& backend_, utils::EventLoop& el)
    : backend(backend_), running(false) {
    TRACE_FUNC();
    egl_display = eglGetDisplay((EGLNativeDisplayType) backend.display());
    if(egl_display == EGL_NO_DISPLAY)
        panic("egl: failed to create display");

    el.post_poll.subscribe([&](auto unsub) {
        unsub();
        start();
    });
}

Renderer::~Renderer() {
    glUseProgram(0);
    shader.reset();
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(egl_display, egl_context);
    eglTerminate(egl_display);
}

// We can't do all this stuff in the constructor because mesa's eglInitialize
// roundtrips the platform backend, sending the wayland events before the
// event loop is started by the user.
// I have at the very least seen this break the wayland backend.
void Renderer::start() {
    if(!eglInitialize(egl_display, NULL, NULL))
        panic("egl: failed to initialize");

    eglBindAPI(EGL_OPENGL_API);

#ifndef NDEBUG
    EGLAttrib debug_attribs[] = {
        EGL_DEBUG_MSG_CRITICAL_KHR, EGL_TRUE,
        EGL_DEBUG_MSG_ERROR_KHR, EGL_TRUE,
        EGL_DEBUG_MSG_WARN_KHR, EGL_TRUE,
        EGL_DEBUG_MSG_INFO_KHR, EGL_TRUE,
        EGL_NONE
    };
    eglDebugMessageControlKHR = (PFNEGLDEBUGMESSAGECONTROLKHRPROC)(eglGetProcAddress("eglDebugMessageControlKHR"));
    if(eglDebugMessageControlKHR)
        eglDebugMessageControlKHR(egl_debug, debug_attribs);
#endif

    EGLint num_config;
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    eglChooseConfig(egl_display, config_attribs, &egl_config, 1, &num_config);

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

    make_current(EGL_NO_SURFACE);
    TRACE_FUNC("loading gl func pointers");
    glbinding::initialize(eglGetProcAddress, false);

    ::gl::glEnable(GL_BLEND);
    ::gl::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ::gl::glEnable(GL_DEPTH_TEST);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&gl_debug, NULL);
#endif

    shader = std::make_unique<Shader>(gl::shapesVertSource, gl::shapesFragSource);
}

void Renderer::make_current(EGLSurface surface) {
    eglMakeCurrent(egl_display, surface, surface, egl_context);
}

} // namespace agt::gl
