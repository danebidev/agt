#pragma once

#include <agt/gl/shaders.hpp>
#include <agt/wayland/display.hpp>

#include <EGL/egl.h>

#include <memory>

namespace agt::gl {

class Renderer {
private:
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;

public:
    // TODO: might want to move this somewhere else?
    std::unique_ptr<Shader> shader;

    Renderer(wayland::Display& display);
    ~Renderer();

    void make_current(EGLSurface surface);

    EGLDisplay display() { return egl_display; };
    EGLConfig config() { return egl_config; };
    EGLContext context() { return egl_context; };
};

} // namespace agt::gl
