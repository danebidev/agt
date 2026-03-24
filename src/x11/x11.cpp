#include <agt/backend/x11.hpp>

#include <chrono>
#include <sys/timerfd.h>

#include <dwhbll/console/debug.hpp>
#include <unistd.h>

using namespace dwhbll::debug;

namespace agt::x11 {

static uint32_t now() {
    using namespace std::chrono;
    return static_cast<uint32_t>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count()
    );
}

Backend::Backend() {
    dpy = XOpenDisplay(NULL);
    if (!dpy)
        panic("x11: failed to open display");
    
    scr = DefaultScreen(dpy);
    wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
}

void Backend::register_window(::Window xid, backend::Window* win) {
    windows[xid] = win;
}

void Backend::unregister_window(::Window xid) {
    windows.erase(xid);
}

void Backend::run(utils::EventLoop& el) {
    int fd = ConnectionNumber(dpy);
    el.add(fd, EPOLLIN, [this](uint32_t) {
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);

            ::Window target = ev.xany.window;
            auto it = windows.find(target);
            if (it == windows.end()) continue;

            static_cast<Window*>(it->second)->handle_event(ev);
        }
    });

    int timer = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

    itimerspec spec{};
    spec.it_interval.tv_sec = 0;
    spec.it_interval.tv_nsec = 16 * 1000 * 1000; // 16ms
    spec.it_value = spec.it_interval;

    timerfd_settime(timer, 0, &spec, nullptr);

    el.add(timer, EPOLLIN, [&](uint32_t) {
        uint64_t expirations;
        read(timer, &expirations, sizeof(expirations));

        uint32_t t = now();

        for (auto& [xid, win] : windows) {
            auto* w = static_cast<Window*>(win);

            if (!w->mapped) continue;

            uint32_t diff = t - w->last_frame_ms;
            if (diff >= w->frame_interval_ms) {
                w->last_frame_ms = t;
                w->frame(diff);
            }
        }
    });
}

void Backend::stop() {
    // TODO
}

backend::WindowPtr Backend::create_window(uint32_t width, uint32_t height,
                                          std::string title) {
    return std::make_unique<Window>(*this, width, height, std::move(title));
}

Window::Window(Backend& backend_, uint32_t width, uint32_t height, std::string title)
    : backend(backend_), 
      dpy((::Display*) backend_.native_display()) {
    state.width = width;
    state.height = height;

    ::Screen* scr = ScreenOfDisplay(dpy, backend.screen());

    XSetWindowAttributes attrs{};
    attrs.event_mask = ExposureMask
                     | StructureNotifyMask;
                     // | KeyPressMask
                     // | KeyReleaseMask
                     // | ButtonPressMask
                     // | ButtonReleaseMask
                     // | PointerMotionMask;

    win = XCreateWindow(
        dpy,
        RootWindowOfScreen(scr),
        0, 0,
        width, height,
        0,
        DefaultDepthOfScreen(scr),
        InputOutput,
        DefaultVisualOfScreen(scr),
        CWEventMask,
        &attrs
    );

    wm_delete = backend.wm_delete_window;
    XSetWMProtocols(dpy, win, &wm_delete, 1);

    XStoreName(dpy, win, title.c_str());
    backend.register_window(win, this);

    XMapWindow(dpy, win);
    XFlush(dpy);
}


void Window::handle_event(const XEvent& ev) {
    switch (ev.type) {
        case Expose:
            // We only care about the last expose
            if (ev.xexpose.count == 0)
                on_expose();
            break;

        case ConfigureNotify:
            on_configure(
                static_cast<uint32_t>(ev.xconfigure.width),
                static_cast<uint32_t>(ev.xconfigure.height)
            );
            break;

        case ClientMessage:
            // WM_DELETE_WINDOW
            if (static_cast<::Atom>(ev.xclient.data.l[0]) == wm_delete)
                close();
            break;

        // TODO: input events

        default:
            break;
    }
}

void Window::on_expose() {
    if (mapped) return;
    mapped = true;
    last_frame_ms = now();
    init_complete();
}

void Window::on_configure(uint32_t w, uint32_t h) {
    if (w == state.width && h == state.height) return;
    state.width  = w;
    state.height = h;
    resize(w, h);
}

}
