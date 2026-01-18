#pragma once

#include <EGL/egl.h>
#include <glbinding/gl43/gl.h>
#include <glm/glm.hpp>

#include "../wayland/display.hpp"
#include "agt/gl/shaders.hpp"

namespace agt::gl {

class Renderer {
private:
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;

public:
    // TODO: this is an hack, move this out of here
    std::unique_ptr<Shader> shader;

    Renderer(wayland::Display& display);
    ~Renderer();

    void make_current(EGLSurface surface);
    void unset_surface();
    void init_shader();

    EGLDisplay display() { return egl_display; };
    EGLConfig config() { return egl_config; };
    EGLContext context() { return egl_context; };
};

} // namespace agt::gl
