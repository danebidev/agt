#include <agt/gl/window.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

#include <glbinding/gl/gl.h>

namespace agt::gl {

using namespace dwhbll::console;
using namespace ::gl;
using namespace draw;

void Window::frame(uint32_t time_diff) {
    make_current();
    eglSwapInterval(renderer.display(), 0);

    glViewport(0, 0, wl_window.current.width, wl_window.current.height);

    glBindVertexArray(vao);

    glBufferData(GL_ARRAY_BUFFER, draw_ctx->vertices.size() * sizeof(Vertex),
                 draw_ctx->vertices.data(), GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, draw_ctx->indices.size() * sizeof(uint16_t),
                 draw_ctx->indices.data(), GL_DYNAMIC_DRAW);

    glClearColor(draw_ctx->clear_color.r, draw_ctx->clear_color.g,
                       draw_ctx->clear_color.b, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto cmd : draw_ctx->cmds) {
        switch(cmd.type) {
        case CmdType::TRIANGLES: {
            auto &shader = draw_ctx->shaders[cmd.shader_program];
            shader.use();

            // GLint projLoc = ::gl::glGetUniformLocation(shader.get(), "proj");
            // glm::mat4 id = glm::mat4(1.0f);
            // ::gl::glUniformMatrix4fv(projLoc, 1, GL_FALSE, &id[0][0]);

            glDrawElements(GL_TRIANGLES, cmd.count, GL_UNSIGNED_SHORT,
                           (void*) (cmd.first_index * sizeof(uint16_t)));
            break;
        }
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

    make_current();

    ::gl::glEnable(GL_BLEND);
    ::gl::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ::gl::glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*) sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexArray(0);
}

Window::~Window() {
    ::gl::glDeleteBuffers(1, &ebo);
    ::gl::glDeleteBuffers(1, &vbo);
    ::gl::glDeleteVertexArrays(1, &vao);

    renderer.make_current(EGL_NO_SURFACE);
    eglDestroySurface(renderer.display(), egl_surface);
    wl_egl_window_destroy(egl_window);
}

void Window::make_current() {
    renderer.make_current(egl_surface);
}

} // namespace agt::gl
