#include <agt/wayland/display.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

namespace agt::wayland {

using namespace dwhbll::debug;
using namespace dwhbll::console;

void xdg_wm_base_ping(void* data, xdg_wm_base* xdg_wm_base, uint32_t serial) {
    trace("xdg_wm_base_ping");
    xdg_wm_base_pong(xdg_wm_base, serial);
}

const xdg_wm_base_listener xdg_wm_base_list = {
    .ping = xdg_wm_base_ping
};

void wl_registry_global(void *data, wl_registry *registry, uint32_t name, 
                        const char *interface, uint32_t version) {
    trace("wl_registry_global: {} (version {})", interface, version);

    Display *display = static_cast<Display*>(data);

    display->global_event(interface, name);
}

void wl_registry_global_remove(void *data, wl_registry *registry, uint32_t name) {}

const wl_registry_listener wl_registry_list = {
    .global = wl_registry_global,
    .global_remove = wl_registry_global_remove,
};

Display::Display() 
    : wl_display(nullptr, &wl_display_disconnect),
      xdg_wm_base(nullptr, &xdg_wm_base_destroy),
      wl_compositor(nullptr, &wl_compositor_destroy) {
    trace("connecting to display");
    wl_display.reset(wl_display_connect(nullptr));
    if(!wl_display)
        dwhbll::debug::panic("wayland: failed to connect to display");

    trace("getting registry");
    wl_registry = wl_display_get_registry(wl_display.get());
    if(!wl_registry)
        dwhbll::debug::panic("wayland: failed to create registry");

    global_event.subscribe([&](std::string interface, uint32_t name) { 
        if(interface == wl_compositor_interface.name) {
            trace("binding wl_compositor (version 5)");
            wl_compositor.reset((struct wl_compositor*) wl_registry_bind(wl_registry, name,
                                                                     &wl_compositor_interface, 5));
        }
        else if(interface == xdg_wm_base_interface.name) {
            trace("binding xdg_wm_base (version 6)");
            xdg_wm_base.reset((struct xdg_wm_base*) wl_registry_bind(wl_registry, name,
                                                                     &xdg_wm_base_interface, 6));
            xdg_wm_base_add_listener(xdg_wm_base.get(), &xdg_wm_base_list, nullptr);
        }
    });

    wl_registry_add_listener(wl_registry, &wl_registry_list, this);
}

void Display::roundtrip() {
    wl_display_roundtrip(wl_display.get());
}

void Display::dispatch_events() {
    wl_display_dispatch(wl_display.get());
}

} // namespace agt::wayland
