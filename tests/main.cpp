#include <agt/ui/widgets/hbox.hpp>
#include <agt/ui/widgets/rect.hpp>
#include <agt/ui/ui.hpp>
#include <agt/wayland/display.hpp>
#include <agt/wayland/window.hpp>
#include <agt/wayland/input.hpp>
#include <agt/gl/rendering.hpp>
#include <agt/gl/window.hpp>
#include <agt/gl/shaders.hpp>

#include <dwhbll/console/Logging.h>

using namespace agt;
using namespace agt::wayland;
using namespace agt::ui;
using namespace agt::draw;

int main() {
    dwhbll::console::defaultLevel = dwhbll::console::Level::TRACE;

    wayland::Display display;
    wayland::InputManager input(display);
    display.roundtrip();

    wayland::Window wayland_window(display, 1080, 720);

    bool running = true;
    wayland_window.close.subscribe([&]() { running = false; });

    agt::gl::Renderer gl_renderer(display);

    Node n = HBox {
        Rectangle { 150, 75, { 0.3, 0.2, 0.3} },
        Rectangle { 320, 45, { 0.6, 0.7, 0.8} }
    };
    UIRoot ui_root(n, { 0.1, 0.4, 0.9 },
                   { wayland_window.current.width, 
                     wayland_window.current.height });
    agt::gl::Window gl_window(gl_renderer, wayland_window, ui_root);

    wayland_window.frame_loop();
    while(running) {
        display.dispatch_events();
    }
}
