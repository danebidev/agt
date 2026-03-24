#include <agt/backend/wl.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace agt::wl {

using namespace dwhbll::console;

void wl_surface_frame(void* data, wl_callback* cb, uint32_t time);

const struct wl_callback_listener wl_surface_frame_list = {
    .done = wl_surface_frame
};

void wl_surface_frame(void* data, wl_callback* cb, uint32_t time) {
    TRACE_FUNC();

    Window* window = static_cast<Window*>(data);
    ASSERT(cb == window->frame_cb);
    wl_callback_destroy(window->frame_cb);

    window->frame_cb = wl_surface_frame(window->wl_surface.get());
    wl_callback_add_listener(window->frame_cb, &wl_surface_frame_list, window);

    window->frame(time);
    wl_surface_commit(window->wl_surface.get());
}

void xdg_surface_configure(void* data, xdg_surface* surface, uint32_t serial) {
    TRACE_FUNC();

    Window* window = static_cast<Window*>(data);

    if(window->state.width != window->pending_state.width 
       || window->state.height != window->pending_state.height) {
        window->state.width = window->pending_state.width;
        window->state.height = window->pending_state.height;
        wl_egl_window_resize(window->egl_window.get(), window->state.width,
                             window->state.height, 0, 0);
        window->resize(window->state.width, window->state.height);
    }

    xdg_surface_ack_configure(window->xdg_surface.get(), serial);

    window->frame(0);
    wl_surface_commit(window->wl_surface.get());
}

const struct xdg_surface_listener xdg_surface_list = {
    .configure = xdg_surface_configure
};

void xdg_toplevel_configure(void* data, xdg_toplevel* toplevel, 
                            int32_t width, int32_t height, wl_array *states) {
    TRACE_FUNC();

    Window* window = static_cast<Window*>(data);

    if(width != 0)
        window->pending_state.width = width;
    if(height != 0)
        window->pending_state.height = height;
}

void xdg_toplevel_close(void* data, xdg_toplevel* toplevel) {
    Window* window = static_cast<Window*>(data);

    window->close();
}

const struct xdg_toplevel_listener xdg_toplevel_list = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
    .configure_bounds = [](auto, auto, auto, auto) {},
    .wm_capabilities = [](auto, auto, auto) {}
};

Window::Window(Backend& backend_, uint32_t width, uint32_t height)
    : wl_surface(nullptr, &wl_surface_destroy),
      xdg_surface(nullptr, &xdg_surface_destroy),
      xdg_toplevel(nullptr, &xdg_toplevel_destroy),
      egl_window(nullptr, &wl_egl_window_destroy),
      backend(backend_) {
    TRACE_FUNC();
    state.width = width;
    state.height = height;
    pending_state = state;
}

Window::~Window() {
    if(frame_cb)
        wl_callback_destroy(frame_cb);
}

void Window::bind_event_loop(utils::EventLoop& el) {
    el.post_poll.subscribe([&](auto unsub) {
        // TODO: find a better way to do this
        if(backend.compositor() && setup_state == 0)
            init_wl_surface();
        else if(backend.xdg_wm() && setup_state == 1)
            start();
        else if(setup_state == 2) {
            unsub();
            init_complete();

            frame_cb = wl_surface_frame(wl_surface.get());
            wl_callback_add_listener(frame_cb, &wl_surface_frame_list, this);
            frame(0);
        }
    });
}

void Window::init_wl_surface() {
    TRACE_FUNC();
    setup_state = 1;

    wl_surface.reset(wl_compositor_create_surface(backend.compositor()));
    if(!wl_surface)
        dwhbll::debug::panic("Failed to create wl_surface");
    egl_window.reset(wl_egl_window_create(wl_surface.get(), state.width,
                                          state.height));
}

void Window::start() {
    TRACE_FUNC();
    setup_state = 2;

    xdg_surface.reset(xdg_wm_base_get_xdg_surface(backend.xdg_wm(), wl_surface.get()));
    if(!xdg_surface)
        dwhbll::debug::panic("Failed to create xdg_surface");
    xdg_surface_add_listener(xdg_surface.get(), &xdg_surface_list, this);

    xdg_toplevel.reset(xdg_surface_get_toplevel(xdg_surface.get()));
    if(!xdg_toplevel)
        dwhbll::debug::panic("Failed to create xdg_toplevel");
    xdg_toplevel_add_listener(xdg_toplevel.get(), &xdg_toplevel_list, this);

    wl_surface_commit(wl_surface.get());
}

} // namespace agt::wl
