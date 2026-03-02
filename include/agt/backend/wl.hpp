#pragma once

#include <agt/backend/backend.hpp>

#include <wayland-client-protocol.h>
#include <wayland-egl-core.h>
#include <xdg-shell-client-protocol.h>

#include <memory>
#include <queue>

namespace agt::wl {

struct Backend : public backend::Backend {
public:
    Backend();

    // void push_input(input::InputEvent event) override { events.push(event); }
    // std::optional<input::InputEvent> pop_input() override;

    // TODO: think of a better way
    void bind_event_loop(utils::EventLoop& el) override;

    void* display() override { return wl_display.get(); };
    struct wl_registry* registry() { return wl_registry.get(); };
    struct wl_compositor* compositor() { return wl_compositor.get(); };
    struct xdg_wm_base* xdg_wm() { return xdg_wm_base.get(); };

    utils::Signal<std::string, uint32_t> global_event;

private:
    friend void wl_registry_global(void *data, wl_registry *registry, uint32_t name, 
                                   const char *interface, uint32_t version);

    // std::queue<input::InputEvent> events;

    std::unique_ptr<::wl_display, decltype(&wl_display_disconnect)> wl_display;
    std::unique_ptr<::wl_registry, decltype(&wl_registry_destroy)> wl_registry;

    std::unique_ptr<::wl_compositor, decltype(&wl_compositor_destroy)> wl_compositor;
    std::unique_ptr<::xdg_wm_base, decltype(&xdg_wm_base_destroy)> xdg_wm_base;
};

struct Window : public backend::Window {
public:
    backend::WindowState pending_state;

    std::unique_ptr<::wl_surface, decltype(&wl_surface_destroy)> wl_surface;
    std::unique_ptr<::xdg_surface, decltype(&xdg_surface_destroy)> xdg_surface;
    std::unique_ptr<::xdg_toplevel, decltype(&xdg_toplevel_destroy)> xdg_toplevel;

    Window(Backend& display, uint32_t width, uint32_t height);
    ~Window();

    void bind_event_loop(utils::EventLoop& el);

    void* surface() override { return egl_window.get(); };

private:
    friend void wl_surface_frame(void* data, wl_callback* cb, uint32_t time);
    friend void xdg_surface_configure(void* data, struct xdg_surface* surface, uint32_t serial);
    friend void xdg_toplevel_configure(void* data, struct xdg_toplevel* toplevel, int32_t width,
                                       int32_t height, wl_array *states);

    Backend& backend;
    wl_callback* frame_cb = nullptr;
    std::unique_ptr<wl_egl_window, decltype(&wl_egl_window_destroy)> egl_window;

    void init_wl_surface();
    void start();

    // 1 = wl_surface created
    // 2 = started
    uint8_t setup_state = 0;
};

}
