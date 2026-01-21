#include <agt/wayland/window.hpp>
#include <agt/wayland/display.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace agt::wayland {

using namespace dwhbll::console;

void wl_surface_frame(void* data, wl_callback* cb, uint32_t time);

const struct wl_callback_listener wl_surface_frame_list = {
    .done = wl_surface_frame
};

void wl_surface_frame(void* data, wl_callback* cb, uint32_t time) {
    // trace("wl_surface_frame");

    Window* window = static_cast<Window*>(data);
    ASSERT(cb == window->frame_cb);
    wl_callback_destroy(window->frame_cb);

    window->frame_cb = wl_surface_frame(window->wl_surface.get());
    wl_callback_add_listener(window->frame_cb, &wl_surface_frame_list, window);

    window->frame(time);
}

void xdg_surface_configure(void* data, xdg_surface* surface, uint32_t serial) {
    trace("xdg_surface_configure");

    Window* window = static_cast<Window*>(data);

    if(window->current.width != window->pending.width 
       || window->current.height != window->pending.height) {
        window->current.width = window->pending.width;
        window->current.height = window->pending.height;
        window->resize(window->pending.width, window->pending.height);
    }

    xdg_surface_ack_configure(window->xdg_surface.get(), serial);

    // window->frame(0);
    wl_surface_commit(window->wl_surface.get());
}

const struct xdg_surface_listener xdg_surface_list = {
    .configure = xdg_surface_configure
};

void xdg_toplevel_configure(void* data, xdg_toplevel* toplevel, 
                            int32_t width, int32_t height, wl_array *states) {
    trace("xdg_toplevel_configure");

    Window* window = static_cast<Window*>(data);

    if(width != 0)
        window->pending.width = width;
    if(height != 0)
        window->pending.height = height;
}

void xdg_toplevel_close(void* data, xdg_toplevel* toplevel) {
    Window* window = static_cast<Window*>(data);

    window->close();
}

const struct xdg_toplevel_listener xdg_toplevel_list = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
    .configure_bounds = [](auto foo, auto bar, auto baz, auto f) {},
    .wm_capabilities = [](auto foo, auto bar, auto baz) {}
};

Window::Window(Display& display, uint32_t width, uint32_t height)
    : wl_surface(nullptr, &wl_surface_destroy),
      xdg_surface(nullptr, &xdg_surface_destroy),
      xdg_toplevel(nullptr, &xdg_toplevel_destroy) {
    current.width = width;
    current.height = height;
    pending = current;

    wl_surface.reset(wl_compositor_create_surface(display.compositor()));
    if(!wl_surface)
        dwhbll::debug::panic("Failed to create wl_surface");

    xdg_surface.reset(xdg_wm_base_get_xdg_surface(display.wm_base(), wl_surface.get()));
    if(!xdg_surface)
        dwhbll::debug::panic("Failed to create xdg_surface");
    xdg_surface_add_listener(xdg_surface.get(), &xdg_surface_list, this);

    xdg_toplevel.reset(xdg_surface_get_toplevel(xdg_surface.get()));
    if(!xdg_toplevel)
        dwhbll::debug::panic("Failed to create xdg_toplevel");
    xdg_toplevel_add_listener(xdg_toplevel.get(), &xdg_toplevel_list, this);

    display.roundtrip();
    surface_commit();
}

Window::~Window() {
    if(frame_cb)
        wl_callback_destroy(frame_cb);
}

void Window::surface_commit() {
    wl_surface_commit(wl_surface.get());
}

void Window::frame_loop() {
    frame(0);

    frame_cb = wl_surface_frame(wl_surface.get());
    wl_callback_add_listener(frame_cb, &wl_surface_frame_list, this);
}

} // namespace agt::wayland
