#pragma once

#include "agt/event.hpp"
#include <agt/input/input.hpp>

#include <optional>

namespace agt::backend {

struct WindowState {
    uint32_t width, height;
};

struct Window {
    WindowState state;

    utils::Signal<> close;
    /**
     * @param ms since the last frame
     */
    utils::Signal<uint32_t> frame;
    utils::Signal<> resize;
    utils::Signal<> init_complete;

    virtual void* surface() = 0;
};

struct Backend {
    virtual ~Backend() = default;

    virtual void bind_event_loop(utils::EventLoop& el) = 0;
    // virtual void push_input(input::InputEvent) = 0;
    // virtual std::optional<input::InputEvent> pop_input() = 0;

    virtual void* display() = 0;
};

} // namespace agt::backend
