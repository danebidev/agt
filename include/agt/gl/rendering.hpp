#pragma once

#include <EGL/egl.h>
#include <glbinding/gl33/gl.h>
#include <glm/glm.hpp>

#include "../wayland/display.hpp"
#include "gl.hpp"

namespace agt::gl {

class Renderer {
private:
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;

public:
    Renderer(wayland::Display& display);
    ~Renderer();

    void make_current(EGLSurface surface);

    EGLDisplay display() { return egl_display; };
    EGLConfig config() { return egl_config; };
    EGLContext context() { return egl_context; };
};

} // namespace agt::gl
