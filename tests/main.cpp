#include <agt/gl/rendering.hpp>
#include <agt/gl/window.hpp>
#include <agt/gl/shaders.hpp>
#include <agt/ui/text.hpp>
#include <agt/ui/widgets/hbox.hpp>
#include <agt/ui/widgets/rect.hpp>
#include <agt/ui/widgets/label.hpp>
#include <agt/ui/ui.hpp>
#include <agt/wayland/display.hpp>
#include <agt/wayland/window.hpp>
#include <agt/wayland/input.hpp>

#include <csignal>
#include <dwhbll/console/Logging.h>
#include <dwhbll/console/debug.hpp>

using namespace agt;
using namespace agt::wayland;
using namespace agt::ui;
using namespace agt::draw;
using namespace agt::widget;

void s(int sig) {
    dwhbll::debug::panic("Signal received: {}", sig);
}

int main() {
    signal(SIGABRT, s);
    signal(SIGSEGV, s);
    dwhbll::console::setLevel(dwhbll::console::Level::TRACE);

    wayland::Display display;
    wayland::InputManager input(display);
    display.roundtrip();

    wayland::Window wayland_window(display, 1080, 720);

    bool running = true;
    wayland_window.close.subscribe([&]() { running = false; });

    agt::gl::Renderer gl_renderer(display);
    TextRendering text;

    Node n = HBox {
        Rectangle { 150, 75, { 0.3, 0.2, 0.3} },
        Rectangle { 320, 45, { 0.6, 0.7, 0.8} },
        Label { text, "The quick brown fox jumps over the lazy dog à°ç" }
    };

    UIRoot ui_root(n, { 0.1, 0.4, 0.9 }, { wayland_window.current.width, 
                                           wayland_window.current.height });
    agt::gl::Window gl_window(gl_renderer, wayland_window, ui_root);

    wayland_window.frame_loop();
    while(running) {
        display.dispatch_events();
    }
}
