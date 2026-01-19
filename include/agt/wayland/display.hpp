#pragma once

#include <memory>
#include <string>
#include <wayland-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <cursor-shape-v1-client-protocol.h>

#include "../event.hpp"

namespace agt::wayland {

class Display {
    friend void wl_registry_global(void *data, wl_registry *registry, uint32_t name, 
                                   const char *interface, uint32_t version);

private:
    std::unique_ptr<::wl_display, decltype(&wl_display_disconnect)> wl_display;
    std::unique_ptr<::wl_registry, decltype(&wl_registry_destroy)> wl_registry;

    // Globals
    std::unique_ptr<::wl_compositor, decltype(&wl_compositor_destroy)> wl_compositor;
    std::unique_ptr<::xdg_wm_base, decltype(&xdg_wm_base_destroy)> xdg_wm_base;

public:
    utils::Signal<std::string, uint32_t> global_event;

    Display();

    void roundtrip();
    void dispatch_events();

    struct wl_display* display() { return wl_display.get(); };
    struct wl_registry* registry() { return wl_registry.get(); };

    struct wl_compositor* compositor() { return wl_compositor.get(); };
    struct xdg_wm_base* wm_base() { return xdg_wm_base.get(); };
};

} // namespace agt::wayland
