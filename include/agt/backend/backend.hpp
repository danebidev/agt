#pragma once

#include "agt/event.hpp"
#include <agt/input/input.hpp>

namespace agt::backend {

struct WindowState {
    uint32_t width, height;
};

struct Window {
    virtual ~Window() = default;

    WindowState state;
    
    utils::Signal<> close;
    utils::Signal<uint32_t> frame; // ms since last frame
    utils::Signal<uint32_t, uint32_t> resize; // new width, height
    utils::Signal<> init_complete;

    virtual void* native_surface() = 0;
};

using WindowPtr = std::unique_ptr<Window>;

struct Backend {
    virtual ~Backend() = default;

    virtual void run(utils::EventLoop& el) = 0;
    virtual void stop() = 0;

    virtual void* native_display() = 0;

    virtual WindowPtr create_window(uint32_t width, uint32_t height,
                                    std::string title = "") = 0;
};

} // namespace agt::backend
