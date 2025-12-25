#pragma once

#include "agt/ui/ui.hpp"
#include <wayland-egl-core.h>
#include <EGL/egl.h>

#include <agt/ui/draw.hpp>
#include <agt/wayland/window.hpp>
#include <agt/gl/rendering.hpp>

namespace agt::gl {

class Window {
private:
    Renderer& renderer;
    wayland::Window& wl_window;
    ui::UIRoot& ui_root;

    wl_egl_window* egl_window;
    EGLSurface egl_surface;
    uint32_t last_frame_time;

    ::gl::GLuint vao;
    ::gl::GLuint vbo;
    ::gl::GLuint ebo;

    void frame(uint32_t diff_time);

public:
    Window(Renderer& rendering, wayland::Window& window, ui::UIRoot& ui_root);
    ~Window();

    void make_current();
};

} // namespace agt::gl
