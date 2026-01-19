#pragma once

#include <memory>
#include <wayland-client-protocol.h>
#include <xdg-shell-client-protocol.h>

#include "../event.hpp"
#include "display.hpp"

namespace agt::wayland {

struct WindowState {
    uint32_t width, height;
};

class Window {
    friend void wl_surface_frame(void* data, wl_callback* cb, uint32_t time);
    friend void xdg_surface_configure(void* data, xdg_surface* surface, uint32_t serial);
    friend void xdg_toplevel_configure(void* data, xdg_toplevel* toplevel, int32_t width,
                                       int32_t height, wl_array *states);

public:
    WindowState current;
    WindowState pending;

    std::unique_ptr<::wl_surface, decltype(&wl_surface_destroy)> wl_surface;
    std::unique_ptr<::xdg_surface, decltype(&xdg_surface_destroy)> xdg_surface;
    std::unique_ptr<::xdg_toplevel, decltype(&xdg_toplevel_destroy)> xdg_toplevel;

    wl_callback* frame_cb = nullptr;

    utils::Signal<> close;
    utils::Signal<uint32_t> frame;
    utils::Signal<uint32_t, uint32_t> resize;

    Window(Display& display, uint32_t width, uint32_t height);
    ~Window();

    void surface_commit();
    void frame_loop();
};

} // namespace agt::wayland
