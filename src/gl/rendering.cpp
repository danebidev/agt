#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>
#include <agt/gl/rendering.hpp>

#include <glbinding/glbinding.h>
#include <glbinding/gl33/gl.h>

namespace agt::gl {

using namespace ::gl;

#ifndef NDEBUG
void gl_debug(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar* message, const void* userParam) {
    dwhbll::console::error("GLDEBUG {} {} {}", static_cast<uint>(type),
                           static_cast<uint>(severity), message);
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
        EGL_CONTEXT_MINOR_VERSION, 6,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifndef NDEBUG
        EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
        EGL_NONE
    };
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, ctx_attribs);

    glbinding::initialize(eglGetProcAddress, false);

#ifndef NDEBUG
    glDebugMessageCallback(gl_debug, 0);
#endif
}

Renderer::~Renderer() {
    eglDestroyContext(egl_display, egl_context);
    eglTerminate(egl_display);
}

void Renderer::make_current(EGLSurface surface) {
    ASSERT(surface);
    eglMakeCurrent(egl_display, surface, surface, egl_context);
}

} // namespace agt::gl
