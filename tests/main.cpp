#include <agt/ui/ui.hpp>
#include <agt/wayland/display.hpp>
#include <agt/wayland/window.hpp>
#include <agt/wayland/input.hpp>
#include <agt/gl/rendering.hpp>
#include <agt/gl/gl.hpp>
#include <agt/gl/window.hpp>
#include <agt/gl/shaders.hpp>

#include <dwhbll/console/Logging.h>

using namespace agt;
using namespace agt::wayland;
using namespace agt::ui;

int main() {
    dwhbll::console::defaultLevel = dwhbll::console::Level::TRACE;

    wayland::Display display;
    wayland::InputManager input(display);
    display.roundtrip();

    wayland::Window wayland_window(display, 1080, 720);

    bool running = true;
    wayland_window.close.subscribe([&]() { running = false; });

    agt::gl::Renderer gl_renderer(display);
    agt::gl::Window gl_window(gl_renderer, wayland_window);

    // This should eventually be returned by the UI lib
    draw::DrawList draw_list;
    draw_list.add_clear({0.0, 0.071, 0.247});
    gl_window.set_draw_list(draw_list);

    wayland_window.frame_loop();
    while(running) {
        display.dispatch_events();
    }
}
