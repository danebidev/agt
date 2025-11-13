#pragma once

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

    wl_egl_window* egl_window;
    EGLSurface egl_surface;
    uint32_t last_frame_time;

    const draw::DrawList* draw_list;

    void frame(uint32_t diff_time);

public:
    Window(Renderer& rendering, wayland::Window& window);
    ~Window();

    void make_current();
    void set_draw_list(const draw::DrawList& list) { draw_list = &list; }
};

} // namespace agt::gl
