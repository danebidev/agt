#include <agt/gl/window.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

#include <glbinding/gl43/gl.h>

namespace agt::gl {

using namespace dwhbll::console;
using namespace ::gl;
using namespace draw;

void update_texture(Texture& tex) {
    ASSERT(tex.status != Texture::Status::OK);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    switch(tex.status) {
        case Texture::Status::CREATE: {
            GLuint id = 0;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.sz.width, tex.sz.height, 
                         0, GL_RGBA, GL_UNSIGNED_BYTE, tex.buf.data());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            tex.status = Texture::Status::OK;
            tex.id = id;
            break;
        }
        case Texture::Status::UPDATE: {
            // TODO
            break;
        }
        case Texture::Status::DELETE: {
            glDeleteTextures(1, &tex.id);
            break;
        }
    }
}

void Window::frame(uint32_t time_diff) {
    make_current();

    ui_root.set_size(wl_window.current.width, wl_window.current.height);
    glViewport(0, 0, wl_window.current.width, wl_window.current.height);

    glBindVertexArray(vao);
    ui_root.compute_layout();
    const DrawCtx& draw_ctx = ui_root.draw();

    // TODO: use glBufferSubData when possible
    if(draw_ctx.vertices_changed)
        glBufferData(GL_ARRAY_BUFFER, draw_ctx.vertices.size() * sizeof(Vertex),
                     draw_ctx.vertices.data(), GL_DYNAMIC_DRAW);
    if(draw_ctx.indices_changed)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, draw_ctx.indices.size() * sizeof(uint16_t),
                     draw_ctx.indices.data(), GL_DYNAMIC_DRAW);

    for(auto& tex : draw_ctx.textures) {
        if(tex.status != Texture::Status::OK)
            update_texture(tex);
    }

    renderer.shader->use();
    // TODO: only update if it changed
    renderer.shader->setmat4("proj", draw_ctx.proj);

    glClearColor(draw_ctx.clear_color.r, draw_ctx.clear_color.g,
                 draw_ctx.clear_color.b, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto cmd : draw_ctx.cmds) {
        switch(cmd.type) {
        case CmdType::TRIANGLES:
            glDrawElements(GL_TRIANGLES, cmd.count, GL_UNSIGNED_SHORT,
                           (void*) (cmd.first_index * sizeof(uint16_t)));
            break;
        case draw::CmdType::TRIANGLES_TEX:
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, draw_ctx.textures[cmd.texture].id);
            glDrawElements(GL_TRIANGLES, cmd.count, GL_UNSIGNED_SHORT,
                           (void*) (cmd.first_index * sizeof(uint16_t)));
            break;
        }
    }

    draw_ctx.finish_frame();
    eglSwapBuffers(renderer.display(), egl_surface);
}

Window::Window(Renderer& rendering_, wayland::Window& window_, ui::UIRoot& ui_root_) 
    : renderer(rendering_),
      wl_window(window_),
      ui_root(ui_root_) {
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
        ui_root.set_size(w, h);
        wl_egl_window_resize(egl_window, w, h, 0, 0);
        // wl_window.frame(0);
    });

    make_current();
    renderer.init_shader();
    renderer.shader->use();
    eglSwapInterval(renderer.display(), 0);

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

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*) (2*sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*) (2*sizeof(glm::vec3) + sizeof(glm::vec2)));
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexArray(0);
}

Window::~Window() {
    ::gl::glDeleteBuffers(1, &ebo);
    ::gl::glDeleteBuffers(1, &vbo);
    ::gl::glDeleteVertexArrays(1, &vao);

    renderer.unset_surface();
    eglDestroySurface(renderer.display(), egl_surface);
    wl_egl_window_destroy(egl_window);
}

void Window::make_current() {
    renderer.make_current(egl_surface);
}

} // namespace agt::gl
