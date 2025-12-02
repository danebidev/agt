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
    agt::gl::Window gl_window(gl_renderer, wayland_window);

    // This should eventually be returned by the UI lib
    DrawCtx ctx({ wayland_window.current.width, wayland_window.current.height });
    ctx.set_clear_color({ 0.7, 0.071, 0.247 });
    std::vector<Vertex> vertices = {
        { { 13.f, 51.f, -2.f }, { 1.0, 0.0, 0.0 } },
        { { 56.f, 123.f, -2.f }, { 0.0, 1.0, 0.0 } },
        { { 325.f, 85.f, -2.f }, { 0.0, 0.0, 1.0 } }
    };
    ctx.add_triangle(vertices);
    gl_window.set_draw_ctx(ctx);

    wayland_window.frame_loop();
    while(running) {
        display.dispatch_events();
    }
}
