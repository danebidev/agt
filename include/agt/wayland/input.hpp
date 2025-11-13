#pragma once

#include <wayland-client-protocol.h>
#include <cursor-shape-v1-client-protocol.h>
#include <cursor-shape-v1-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

#include "display.hpp"
#include "../event.hpp"

struct App;

namespace agt::wayland {

enum class PointerMask : uint32_t {
    ENTER = 1 << 0,
    LEAVE = 1 << 1,
    MOTION = 1 << 2,
    BUTTON = 1 << 3,
    AXIS = 1 << 4,
    AXIS_SOURCE = 1 << 5,
    // AXIS_STOP = 1 << 6,
};


struct PointerState {
    // Bitmask of PointerMask
    uint32_t events;
    uint32_t serial;
    // Keep this for stuff like setting cursor shape
    uint32_t enter_serial;
    uint32_t button, button_state;
    uint32_t time;

    wl_surface *enter_surface, *leave_surface;
    double x, y;

    struct {
        bool valid;
        int value;
    } axes[2];

    uint32_t axis_source;

    void reset() {
        events = 0;
        axes[0].valid = false;
        axes[1].valid = false;
    }
};

class InputManager;

class Pointer {
    friend void wl_pointer_enter(void* data, wl_pointer* wl_pointer, uint32_t serial,
                                 wl_surface* surface, wl_fixed_t x, wl_fixed_t y);
    friend void wl_pointer_leave(void* data, wl_pointer* wl_pointer, uint32_t serial,
                                 wl_surface* surface);
    friend void wl_pointer_motion(void* data, wl_pointer* pointer, uint32_t serial, 
                                  wl_fixed_t x, wl_fixed_t y);
    friend void wl_pointer_button(void* data, wl_pointer* wl_pointer, uint32_t serial, 
                                  uint32_t time, uint32_t button, uint32_t state);
    friend void wl_pointer_axis(void* data, wl_pointer* wl_pointer, uint32_t time,
                                uint32_t axis, wl_fixed_t value);
    friend void wl_pointer_frame(void* data, wl_pointer* wl_pointer);
    friend void wl_pointer_axis_source(void* data, wl_pointer* wl_pointer, uint32_t source);

private:
    InputManager& input;
    std::unique_ptr<wl_pointer, decltype(&wl_pointer_release)> pointer;

    std::unique_ptr<wp_cursor_shape_device_v1, 
                    decltype(&wp_cursor_shape_device_v1_destroy)> cursor_shape_pointer;

    PointerState ptr_state = {0};

public:
    Pointer(InputManager& input, wl_pointer* pointer);
};

class Keyboard {
    friend void wl_keyboard_keymap(void* data, wl_keyboard* wl_keyboard, uint32_t format,
                                   int fd, uint32_t size);
    friend void wl_keyboard_enter(void* data, wl_keyboard* keyboard, uint32_t serial,
                                  wl_surface* surface, wl_array* keys);
    friend void wl_keyboard_leave(void* data, wl_keyboard* keyboard, uint32_t serial,
                                  wl_surface* surface);
    friend void wl_keyboard_key(void* data, wl_keyboard* keyboard, uint32_t serial,
                                uint32_t time, uint32_t key, uint32_t state);

private:
    InputManager& input;
    std::unique_ptr<wl_keyboard, decltype(&wl_keyboard_release)> keyboard;

    std::unique_ptr<::xkb_context, decltype(&xkb_context_unref)> xkb_context;
    std::unique_ptr<::xkb_keymap, decltype(&xkb_keymap_unref)> xkb_keymap;
    std::unique_ptr<::xkb_state, decltype(&xkb_state_unref)> xkb_state;

public:
    Keyboard(InputManager& input, wl_keyboard* keyboard);
};

class InputManager {
    friend class Keyboard;
    friend class Pointer;
    friend void wl_seat_capabilities(void* data, wl_seat* seat, uint32_t cap);

private:
    Display& display;
    std::unique_ptr<wl_seat, decltype(&wl_seat_release)> seat;
    std::unique_ptr<Pointer> pointer;
    std::unique_ptr<Keyboard> keyboard;

    std::unique_ptr<wp_cursor_shape_manager_v1, 
                    decltype(&wp_cursor_shape_manager_v1_destroy)> cursor_shape;

public:
    utils::Signal<uint32_t, wl_surface*> ptr_enter;
    utils::Signal<uint32_t, wl_surface*> ptr_leave;
    utils::Signal<uint32_t, uint32_t, uint32_t> ptr_motion;
    utils::Signal<uint32_t, uint32_t, uint32_t, uint32_t> ptr_click;
    utils::Signal<uint32_t, int32_t, int32_t> ptr_scroll;

    utils::Signal<uint32_t, wl_surface*> kb_enter;
    utils::Signal<uint32_t, wl_surface*> kb_leave;
    utils::Signal<uint32_t, uint32_t, uint32_t, uint32_t> kb_key;

    InputManager(Display& display);
};

} // namespace agt::wayland
