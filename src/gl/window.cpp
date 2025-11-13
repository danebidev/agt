#include <agt/gl/window.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

#include <glbinding/gl/gl.h>

namespace agt::gl {

using namespace dwhbll::console;
using namespace ::gl;
using namespace draw;

void Window::frame(uint32_t time_diff) {
    // trace("frame: {}", time_diff);

    make_current();
    eglSwapInterval(renderer.display(), 0);

    glViewport(0, 0, wl_window.current.width, wl_window.current.height);

    for(auto draw_cmd : draw_list->cmds) {
        switch(draw_cmd.type) {
            case CmdType::CLEAR_SCREEN:
                glClearColor(draw_cmd.clear_color.r, draw_cmd.clear_color.g, 
                             draw_cmd.clear_color.b, 1.0);
                glClear(GL_COLOR_BUFFER_BIT);
                break;
        }
    }

    eglSwapBuffers(renderer.display(), egl_surface);
}

Window::Window(Renderer& rendering_, wayland::Window& window_) 
    : renderer(rendering_),
      wl_window(window_) {
    egl_window = wl_egl_window_create(wl_window.wl_surface.get(),
                                      wl_window.current.width, wl_window.current.height);
    egl_surface = eglCreateWindowSurface(renderer.display(), renderer.config(), 
                                         (EGLNativeWindowType) egl_window, NULL);

    wl_window.frame.subscribe([&](uint32_t time) {
        uint32_t diff = time;
        // We use time=0 for manually sent redraws (like on resize)
        if(time != 0) {
            ASSERT(time >= last_frame_time);
            diff -= last_frame_time;
            last_frame_time = time;
        }
        frame(diff);
    });

    wl_window.resize.subscribe([&](uint32_t w, uint32_t h) {
        wl_egl_window_resize(egl_window, w, h, 0, 0);
        // wl_window.frame(0);
    });
}

Window::~Window() {
    eglDestroySurface(renderer.display(), egl_surface);
    wl_egl_window_destroy(egl_window);
}

void Window::make_current() {
    renderer.make_current(egl_surface);
}

}
