// #include <agt/backend/wl.hpp>
// #include <agt/backend/wl-input.hpp>
#include <agt/ui/widgets/button.hpp>
#include <agt/gl/gl.hpp>
#include <agt/backend/x11.hpp>
#include <agt/gl/shaders.hpp>
#include <agt/ui/text.hpp>
#include <agt/ui/widgets/hbox.hpp>
#include <agt/ui/widgets/rect.hpp>
#include <agt/ui/widgets/label.hpp>
#include <agt/ui/ui.hpp>

#include <csignal>
#include <dwhbll/console/Logging.h>
#include <dwhbll/console/debug.hpp>

using namespace agt;
using namespace agt::wl;
using namespace agt::ui;
using namespace agt::draw;
using namespace agt::widget;
using namespace agt::backend;

void s(int sig) {
    dwhbll::debug::panic("Signal received: {}", sig);
}

int main() {
    signal(SIGABRT, s);
    signal(SIGSEGV, s);
    dwhbll::console::setLevel(dwhbll::console::Level::TRACE);
    dwhbll::console::setWantColors(true);

    utils::EventLoop el;
    x11::Backend backend;

    WindowPtr window = backend.create_window(1080, 720, "asdf");

    window->close.subscribe([&](auto unsub) { el.stop(); });

    agt::gl::Renderer gl_renderer(backend, el);
    TextRendering text;

    Node n = HBox {
        Label("WINE_CANONICAL_HOLE=skip_volatile_check"),
        Rectangle { 150, 75, { 0.8, 0.3, 0.6 } }
    };

    UIRoot ui_root(n, { 0.1, 0.4, 0.9 },
                   { window->state.width, window->state.height }, text);
    agt::gl::Window gl_window(gl_renderer, window.get(), ui_root);

    el.start();
}
