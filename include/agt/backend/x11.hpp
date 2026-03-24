#pragma once

#include <agt/backend/backend.hpp>

#include <X11/Xlib.h>

namespace agt::x11 {

// X11 is mainly for testing, just becaues it's faster
// to write than a proper wayland backend
struct Backend : public backend::Backend {
    Backend();

    void run(utils::EventLoop& el) override;
    void stop() override;

    void* native_display() override { return dpy; };
    int screen() { return scr; };

    backend::WindowPtr create_window(uint32_t width, uint32_t height,
                                     std::string title = "") override;

    utils::Signal<XExposeEvent> expose;
    utils::Signal<XKeyPressedEvent> key_pressed;

private:
    std::unordered_map<::Window, backend::Window*> windows;

    ::Display *dpy;
    int scr;

    ::Atom wm_delete_window = 0;

    friend struct Window;
    void register_window(::Window xid, backend::Window* win);
    void unregister_window(::Window xid);
};

struct Window : public backend::Window {
    Window(Backend& backend, uint32_t width, uint32_t height, std::string title);

    void* native_surface() { return &win; };

    void handle_event(const XEvent& ev);
    void on_expose();
    void on_configure(uint32_t w, uint32_t h);

private:
    Backend&  backend;
    ::Display* dpy;
    ::Window win;
    ::Atom wm_delete;

    friend Backend;
    bool mapped = false;
    uint32_t last_frame_ms = 0;
    uint32_t frame_interval_ms = 16;
};

}; // namespace agt::x11
