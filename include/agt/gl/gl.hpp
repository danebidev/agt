#pragma once

#include <agt/ui/ui.hpp>
#include <agt/gl/shaders.hpp>
#include <agt/backend/wl.hpp>

#include <EGL/egl.h>

namespace agt::gl {

class Renderer {
public:
    // TODO: might want to move this somewhere else?
    std::unique_ptr<Shader> shader;

    Renderer(backend::Backend& backend_, utils::EventLoop& el);
    ~Renderer();

    void start();
    void make_current(EGLSurface surface);

    EGLDisplay display() { return egl_display; };
    EGLConfig config() { return egl_config; };
    EGLContext context() { return egl_context; };

private:
    backend::Backend& backend;
    bool running;

    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
};

class Window {
private:
    Renderer& renderer;
    backend::Window& backend_window;
    ui::UIRoot& ui_root;

    EGLSurface egl_surface = nullptr;
    uint32_t last_frame_time;

    bool init_done = false;

    ::gl::GLuint vao;
    ::gl::GLuint vbo;
    ::gl::GLuint ebo;

    void frame(uint32_t diff_time);

public:
    Window(Renderer& rendering, backend::Window& window, ui::UIRoot& ui_root);
    ~Window();

    void make_current();
};

} // namespace agt::gl
