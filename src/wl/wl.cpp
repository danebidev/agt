#include <agt/backend/wl.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

namespace agt::wl {

using namespace dwhbll::debug;
using namespace dwhbll::console;

void xdg_wm_base_ping(void* data, xdg_wm_base* xdg_wm_base, uint32_t serial) {
    TRACE_FUNC();
    xdg_wm_base_pong(xdg_wm_base, serial);
}

const xdg_wm_base_listener xdg_wm_base_list = {
    .ping = xdg_wm_base_ping
};

void wl_registry_global(void *data, wl_registry *registry, uint32_t name, 
                        const char *interface, uint32_t version) {
    TRACE_FUNC("{} (version {})", interface, version);

    Backend *display = static_cast<Backend*>(data);
    display->global_event(interface, name);
}

void wl_registry_global_remove(void *data, wl_registry *registry, uint32_t name) {}

const wl_registry_listener wl_registry_list = {
    .global = wl_registry_global,
    .global_remove = wl_registry_global_remove,
};

Backend::Backend() 
    : wl_display(nullptr, &wl_display_disconnect),
      wl_registry(nullptr, &wl_registry_destroy),
      xdg_wm_base(nullptr, &xdg_wm_base_destroy),
      wl_compositor(nullptr, &wl_compositor_destroy) {
    global_event.subscribe([&](auto unsub, std::string interface, uint32_t name) { 
        if(interface == wl_compositor_interface.name) {
            TRACE_FUNC("binding wl_compositor (version 5)");
            wl_compositor.reset((struct wl_compositor*) wl_registry_bind(wl_registry.get(), name,
                                                                  &wl_compositor_interface, 5));
        }
        else if(interface == xdg_wm_base_interface.name) {
            TRACE_FUNC("binding xdg_wm_base (version 5)");
            xdg_wm_base.reset((struct xdg_wm_base*) wl_registry_bind(wl_registry.get(), name,
                                                                     &xdg_wm_base_interface, 5));
            xdg_wm_base_add_listener(xdg_wm_base.get(), &xdg_wm_base_list, nullptr);
        }
    });

    TRACE_FUNC("connecting to wl display");
    wl_display.reset(wl_display_connect(nullptr));
    if(!wl_display)
        dwhbll::debug::panic("wl: failed to connect to display");

    TRACE_FUNC("getting registry");
    wl_registry.reset(wl_display_get_registry(wl_display.get()));
    if(!wl_registry)
        dwhbll::debug::panic("wl: failed to create registry");

    wl_registry_add_listener(wl_registry.get(), &wl_registry_list, this);
}

// std::optional<input::InputEvent> Backend::pop_input() {
//     if(!events.empty()) {
//         input::InputEvent e = events.front();
//         events.pop();
//         return e;
//     }
//     return {};
// }

void Backend::run(utils::EventLoop& el) {
    int fd = wl_display_get_fd(wl_display.get());

    el.pre_poll.subscribe([&](auto unsub) {
        wl_display_flush(wl_display.get());
    });

    el.add(fd, EPOLLIN, [&](uint32_t events) {
        if(events & (EPOLLHUP | EPOLLERR)) {
            ERROR_FUNC("{}", events);
        } else if(events & EPOLLIN) {
            wl_display_dispatch(wl_display.get());
        } else {
            panic("Invalid event");
        }
    });
}

} // namespace agt::wl
